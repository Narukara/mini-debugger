#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>

#include "linenoise.h"

using std::cout, std::endl;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Program name not specified" << endl;
        return -1;
    }

    auto prog = argv[1];
    cout << "Debugging " << prog << endl;

    auto pid = fork();
    if (pid == 0) {
        /***
         * child, tracee
         *
         * https://man7.org/linux/man-pages/man2/ptrace.2.html
         *
         * If the PTRACE_O_TRACEEXEC option is not in effect, all successful
         * calls to execve(2) by the traced process will cause it to be sent
         * a SIGTRAP signal, giving the parent a chance to gain control
         * before the new program begins execution.
         */
        ptrace(PTRACE_TRACEME, 0, nullptr, nullptr);  // allow its parent to trace it
        execl(prog, prog, nullptr);                   // execute the given program

        // execl() return only if it fails
        std::cerr << "Failed to execute " << prog << endl;
        return -1;
    } else {
        // parent, tracer
        cout << "Child pid: " << pid << endl;

        int wait_status;
        auto options = 0;
        waitpid(pid, &wait_status, options);

        char* line = nullptr;
        while ((line = linenoise("minidbg> ")) != nullptr) {
            cout << "Got: " << line << endl;

            ptrace(PTRACE_CONT, pid, nullptr, nullptr);

            linenoiseHistoryAdd(line);
            linenoiseFree(line);
        }
    }
}