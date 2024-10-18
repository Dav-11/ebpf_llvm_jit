# E04: Handlers call

This example runs an XDP program with a bpf_printk() helper function call.

Example output:
```
root@devbox:~/git/ebpf_llvm_jit/examples/04_qemu_riscv_helpers_calls# make run
Ctrl-A C for QEMU console, then quit to exit
qemu-system-riscv64 -nographic -serial mon:stdio -machine virt -bios .output/hello.elf
Started runtime
Start MM Region: 0x800022B4 
End MM Region: 0x8000235C 
++++ Packet content ++++

[0x800022B4]: 0C00 AA29 FFFD E000 684C 5406 0008 0045 3400 0000 0040 0640 C6B3 A8C0 B902 A8C0
[0x800022D4]: F402 F8CF 1600 7FE3 F3ED 90A9 AA2C 1080 FE07 2ECE 0000 0101 0A08 EC4D BCA5 2B45
[0x800022F4]: 0169 FFFF FFFF FFFF FFFF

++++ END +++++

Start Pkt: 0x800022B4
End Pkt: 0x800022FE
Hello World from XDP
XDP result: 2
```