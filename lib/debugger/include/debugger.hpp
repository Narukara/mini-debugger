#pragma once

#include <sys/types.h>
#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>

#include "breakpoint.hpp"

// same order as in user_regs_struct
enum class reg {
    r15,
    r14,
    r13,
    r12,
    rbp,
    rbx,
    r11,
    r10,
    r9,
    r8,
    rax,
    rcx,
    rdx,
    rsi,
    rdi,
    orig_rax,
    rip,
    cs,
    // RFLAGS, EFLAGS and FLAGS are the same register;
    // it's just that RFLAGS is the whole register, EFLAGS is the lower 32 bits only,
    // and FLAGS is the lower 16 bits only.
    eflags,
    rsp,
    ss,
    fs_base,
    gs_base,
    ds,
    es,
    fs,
    gs,
};

constexpr size_t reg_num = 27;

struct reg_info {
    reg r;
    int dwarf;  // see https://www.uclibc.org/docs/psABI-x86_64.pdf
    std::string name;
};

const std::array<reg_info, reg_num> all_reg_info = {{
    {reg::rax, 0, "rax"},
    {reg::rbx, 3, "rbx"},
    {reg::rcx, 2, "rcx"},
    {reg::rdx, 1, "rdx"},
    {reg::rsi, 4, "rsi"},
    {reg::rdi, 5, "rdi"},
    {reg::rbp, 6, "rbp"},
    {reg::rsp, 7, "rsp"},
    {reg::r8, 8, "r8"},
    {reg::r9, 9, "r9"},
    {reg::r10, 10, "r10"},
    {reg::r11, 11, "r11"},
    {reg::r12, 12, "r12"},
    {reg::r13, 13, "r13"},
    {reg::r14, 14, "r14"},
    {reg::r15, 15, "r15"},
    {reg::rip, -1, "rip"},
    {reg::eflags, 49, "eflags"},
    {reg::cs, 51, "cs"},
    {reg::orig_rax, -1, "orig_rax"},
    {reg::fs_base, 58, "fs_base"},
    {reg::gs_base, 59, "gs_base"},
    {reg::fs, 54, "fs"},
    {reg::gs, 55, "gs"},
    {reg::ss, 52, "ss"},
    {reg::ds, 53, "ds"},
    {reg::es, 50, "es"},
}};

class debugger {
    pid_t pid;
    std::string name;
    std::unordered_map<uintptr_t, breakpoint> breakpoints;

    uint64_t read_memory(uintptr_t addr);
    void write_memory(uintptr_t addr, uint64_t data);

    uint64_t get_pc() { return get_register(reg::rip); }
    void set_pc(uint64_t pc) { set_register(reg::rip, pc); }

   public:
    debugger(pid_t pid, const std::string& name);

    void command(const std::string& cmd);
    void continue_execution();
    void set_breakpoint(uintptr_t addr);
    void print_breakpoints();

    static void eval_status(int status);

    uint64_t get_register(reg r);
    void set_register(reg r, uint64_t value);
    static std::optional<reg> str_to_reg(const std::string& name);
    static std::string reg_to_str(reg r);
};