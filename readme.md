Toy debugger.

https://blog.tartanllama.xyz/writing-a-linux-debugger-setup/

https://github.com/TartanLlama/minidbg

---

#### Note

被调试的程序是 `lib/sample`，反汇编会 dump 到 `build/lib/sample/sample.asm`

读取 `/proc/<pid>/maps` 可以得到程序的加载地址

把反汇编中的偏移量加到加载地址上，就可以得到实际地址。在这里可以打断点
