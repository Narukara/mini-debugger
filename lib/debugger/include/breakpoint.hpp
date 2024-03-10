#pragma once

#include <sys/types.h>
#include <cstdint>

class Breakpoint {
    pid_t pid;
    intptr_t addr;
    uint64_t origin_data;
    bool enabled;

    constexpr static uint8_t int3 = 0xcc;

   public:
    Breakpoint(pid_t pid, intptr_t addr);
    Breakpoint(Breakpoint& b) = delete;
    Breakpoint(Breakpoint&& b);
    Breakpoint& operator=(Breakpoint& rhs) = delete;
    ~Breakpoint();

    void enable();
    void disable();
    bool is_enabled() const { return enabled; }
};