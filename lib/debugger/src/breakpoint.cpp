#include <sys/ptrace.h>

#include "breakpoint.hpp"

Breakpoint::Breakpoint(pid_t pid, intptr_t addr) : pid(pid), addr(addr), enabled(false) {
    origin_data = ptrace(PTRACE_PEEKDATA, pid, addr, nullptr);
}

void Breakpoint::enable() {
    auto data_with_int3 = ((origin_data & ~0xff) | int3);
    ptrace(PTRACE_POKEDATA, pid, addr, data_with_int3);
    enabled = true;
}

void Breakpoint::disable() {
    ptrace(PTRACE_POKEDATA, pid, addr, origin_data);
    enabled = false;
}

Breakpoint::~Breakpoint() {
    if (enabled) {
        disable();
    }
}

/**
 * @note don't touch moved object anymore
 */
Breakpoint::Breakpoint(Breakpoint&& b) : pid(b.pid), addr(b.addr), origin_data(b.origin_data), enabled(b.enabled) {
    b.pid = 0;
    b.addr = 0;
    b.enabled = false;
}