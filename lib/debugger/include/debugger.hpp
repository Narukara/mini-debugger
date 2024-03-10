#pragma once

#include <sys/types.h>
#include <cstdint>
#include <string>
#include <unordered_map>

#include "breakpoint.hpp"

class Debugger {
    pid_t pid;
    std::string name;
    std::unordered_map<intptr_t, Breakpoint> breakpoints;

   public:
    Debugger(pid_t pid, const std::string& name);

    void command(const std::string& cmd);
    void continue_execution();
    void set_breakpoint(intptr_t addr);
    void print_breakpoints();

    static void eval_status(int status);
};