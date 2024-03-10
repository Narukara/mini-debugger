#include <sys/ptrace.h>
#include <sys/wait.h>
#include <algorithm>
#include <iostream>
#include <vector>

#include "debugger.hpp"

using std::cout, std::endl, std::string, std::vector;

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

Debugger::Debugger(pid_t pid, const string& name) : pid(pid), name(name) {
    int status;
    waitpid(pid, &status, 0);
    eval_status(status);
}

void Debugger::command(const string& cmd) {
    auto w = split(cmd);
    if (is_prefix(w[0], "continue")) {
        continue_execution();
    } else if (is_prefix(w[0], "breakpoint")) {
        if (w.size() == 1) {
            print_breakpoints();
        } else if (w.size() == 2) {
            set_breakpoint(stol(w[1], 0, 16));
        }
    } else {
        cout << "Unknown command" << endl;
    }
}

void Debugger::continue_execution() {
    ptrace(PTRACE_CONT, pid, nullptr, nullptr);
    int status;
    waitpid(pid, &status, 0);
    eval_status(status);
}

void Debugger::set_breakpoint(intptr_t addr) {
    cout << "Set breakpoint at address 0x" << std::hex << addr << endl;
    Breakpoint bp(pid, addr);
    bp.enable();
    breakpoints.emplace(addr, std::move(bp));
}

void Debugger::print_breakpoints() {
    for (const auto& [addr, bp] : breakpoints) {
        cout << "0x" << std::hex << addr << " " << (bp.is_enabled() ? "enabled" : "disabled") << endl;
    }
}

void Debugger::eval_status(int status) {
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
