#pragma once

#include <sys/types.h>
#include <cstdint>

class breakpoint {
    pid_t pid;
    intptr_t addr;
    uint64_t origin_data;
    bool enabled;

    constexpr static uint8_t int3 = 0xcc;

   public:
    breakpoint(pid_t pid, intptr_t addr);
    breakpoint(breakpoint& b) = delete;
    breakpoint(breakpoint&& b);
    breakpoint& operator=(breakpoint& rhs) = delete;
    ~breakpoint();

    void enable();
    void disable();
    bool is_enabled() const { return enabled; }
};