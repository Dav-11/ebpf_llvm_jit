# E03: Local calls

This example runs an XDP program with some local function calls.

Example output:
```
root@devbox:~/git/ebpf_llvm_jit/examples/03_qemu_riscv_local_calls# make run
Ctrl-A C for QEMU console, then quit to exit
qemu-system-riscv64 -nographic -serial mon:stdio -machine virt -bios .output/hello.elf
Started runtime
Start MM Region: 0x8000229C 
End MM Region: 0x80002344 
++++ Packet content ++++

[0x8000229C]: 0C00 AA29 FFFD E000 684C 5406 0008 0045 3400 0000 0040 0640 C6B3 A8C0 B902 A8C0
[0x800022BC]: F402 F8CF 1600 7FE3 F3ED 90A9 AA2C 1080 FE07 2ECE 0000 0101 0A08 EC4D BCA5 2B45
[0x800022DC]: 0169 FFFF FFFF FFFF FFFF

++++ END +++++

Start Pkt: 0x8000229C
End Pkt: 0x800022E6
XDP result: 91
```