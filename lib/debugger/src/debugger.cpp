#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <vector>

#include "debugger.hpp"

using std::cout, std::endl, std::string, std::vector, std::find_if, std::optional;

inline bool is_prefix(const string& s, const string& pattern) {
    if (s.size() > pattern.size()) {
        return false;
    }
    return equal(s.begin(), s.end(), pattern.begin());
}

vector<string> split(const string& s) {
    vector<string> out;
    auto b = s.begin();
    while (1) {
        auto e = find_if(b, s.end(), [](char c) { return isspace(c); });
        if (b != e) {
            out.emplace_back(b, e);
        }
        if (e == s.end()) {
            break;
        }
        b = e + 1;
    }
    return out;
}

debugger::debugger(pid_t pid, const string& name) : pid(pid), name(name) {
    int status;
    waitpid(pid, &status, 0);
    eval_status(status);
}

uint64_t debugger::read_memory(uintptr_t addr) {
    return ptrace(PTRACE_PEEKDATA, pid, addr, nullptr);
}

void debugger::write_memory(uintptr_t addr, uint64_t data) {
    ptrace(PTRACE_POKEDATA, pid, addr, data);
}

void debugger::command(const string& cmd) {
    auto w = split(cmd);
    if (is_prefix(w[0], "continue")) {
        continue_execution();
    } else if (is_prefix(w[0], "breakpoint")) {
        if (w.size() == 1) {  // breakpoint
            print_breakpoints();
        } else if (w.size() == 2) {  // breakpoint address(base 16)
            set_breakpoint(stol(w[1], nullptr, 16));
        } else if (w.size() == 3) {  // breakpoint enable/disable index
            size_t index = stol(w[2], nullptr, 10);
            if (index >= breakpoints.size()) {
                cout << "Invalid breakpoint index" << endl;
                return;
            }
            auto it = breakpoints.begin();
            std::advance(it, index);
            if (is_prefix(w[1], "enable")) {
                it->second.enable();
            } else if (is_prefix(w[1], "disable")) {
                it->second.disable();
            } else {
                cout << "Unknown command" << endl;
            }
        }
    } else if (is_prefix(w[0], "register")) {
        if (w.size() == 1) {  // register
            for (const auto& ri : all_reg_info) {
                cout << std::setfill(' ') << std::setw(8) << std::right << ri.name;
                cout << " 0x" << std::setfill('0') << std::setw(16) << std::hex << get_register(ri.r) << endl;
            }
            return;
        }
        // register reg_name ...
        auto r = str_to_reg(w[1]);
        if (!r) {
            cout << "Unknown register" << endl;
            return;
        }
        if (w.size() == 2) {  // register reg_name
            cout << "0x" << std::setfill('0') << std::setw(16) << std::hex << get_register(*r) << endl;
        } else if (w.size() == 3) {             // register reg_name value
            auto val = stol(w[2], nullptr, 0);  // 0 means auto-detect base
            set_register(*r, val);
        }
    } else if (is_prefix(w[0], "memory")) {
        if (w.size() == 2) {  // memory address(base 16)
            auto addr = stol(w[1], nullptr, 16);
            cout << "0x" << std::hex << read_memory(addr) << endl;
        } else if (w.size() == 3) {  // memory address(base 16) value
            auto addr = stol(w[1], nullptr, 16);
            auto val = stol(w[2], nullptr, 0);  // 0 means auto-detect base
            write_memory(addr, val);
        }
    } else if (is_prefix(w[0], "quit")) {
        exit(0);
    } else if (is_prefix(w[0], "help")) {
        cout << "Available commands:" << endl;
        cout << "  continue" << endl;
        cout << "  breakpoint [address]" << endl;
        cout << "  breakpoint enable|disable index" << endl;
        cout << "  register [reg_name [value]]" << endl;
        cout << "  memory address [value]" << endl;
        cout << "  quit" << endl;
        cout << "  help" << endl;
    } else {
        cout << "Unknown command" << endl;
    }
}

void debugger::continue_execution() {
    /**
     * handler for breakpoint
     *              rip
     *               v
     * xxx xxx int3 xxx xxx
     */
    auto breakpoint_addr = get_pc() - 1;
    auto it = breakpoints.find(breakpoint_addr);
    if (it != breakpoints.end()) {
        auto& bp = it->second;
        set_pc(breakpoint_addr);
        if (bp.is_enabled()) {
            bp.disable();
            ptrace(PTRACE_SINGLESTEP, pid, nullptr, nullptr);
            waitpid(pid, nullptr, 0);
            bp.enable();
        }
    }
    // continue execution
    ptrace(PTRACE_CONT, pid, nullptr, nullptr);
    int status;
    waitpid(pid, &status, 0);
    eval_status(status);
}

void debugger::set_breakpoint(uintptr_t addr) {
    cout << "Set breakpoint at address 0x" << std::hex << addr << endl;
    breakpoint bp(pid, addr);
    bp.enable();
    breakpoints.emplace(addr, std::move(bp));
}

void debugger::print_breakpoints() {
    size_t i = 0;
    for (const auto& [addr, bp] : breakpoints) {
        cout << std::dec << i++ << ": ";
        cout << "0x" << std::hex << addr << " " << (bp.is_enabled() ? "enabled" : "disabled") << endl;
    }
}

void debugger::eval_status(int status) {
    if (WIFEXITED(status)) {
        cout << "Child exited, status=" << WEXITSTATUS(status) << endl;
    } else if (WIFSIGNALED(status)) {
        cout << "Child killed by signal " << std::dec << WTERMSIG(status) << endl;
    } else if (WIFSTOPPED(status)) {
        cout << "Child stopped by signal " << std::dec << WSTOPSIG(status) << endl;
    } else if (WIFCONTINUED(status)) {
        cout << "Child continued" << endl;
    } else { /* Non-standard case -- may never happen */
        cout << "Unexpected status=0x" << std::hex << status << endl;
    }
}

uint64_t debugger::get_register(reg r) {
    user_regs_struct regs;  // from <sys/user.h>
    ptrace(PTRACE_GETREGS, pid, nullptr, &regs);
    auto index = static_cast<size_t>(r);
    return reinterpret_cast<uint64_t*>(&regs)[index];
}

void debugger::set_register(reg r, uint64_t value) {
    user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, nullptr, &regs);
    auto index = static_cast<size_t>(r);
    reinterpret_cast<uint64_t*>(&regs)[index] = value;
    ptrace(PTRACE_SETREGS, pid, nullptr, &regs);
}

optional<reg> debugger::str_to_reg(const string& name) {
    auto it = find_if(all_reg_info.begin(), all_reg_info.end(), [&](const reg_info& ri) { return ri.name == name; });
    if (it == all_reg_info.end()) {
        return {};
    }
    return it->r;
}

string debugger::reg_to_str(reg r) {
    auto it = find_if(all_reg_info.begin(), all_reg_info.end(), [&](const reg_info& ri) { return ri.r == r; });
    return it->name;
}