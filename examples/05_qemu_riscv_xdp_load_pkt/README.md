# E05: Handle one packet

This example load a single packet from the memory and runs on it an XDP program that checks if the packet is IPv4 or not.

Example output:
```
root@devbox:~/git/ebpf_llvm_jit/examples/05_qemu_riscv_xdp_load_pkt# make run
Ctrl-A C for QEMU console, then quit to exit
qemu-system-riscv64 -nographic -serial mon:stdio -machine virt -bios .output/hello.elf
Started runtime
Start MM Region: 0x8000240C 
End MM Region: 0x800024B4 
packet->data = 0x8000240C
++++ Packet content ++++

[0x000000008000240C]: 0C00 AA29 FFFD E000 684C 5406 0008 0045 3400 0000 0040 0640 C6B3 A8C0 B902 A8C0
[0x000000008000242C]: F402 F8CF 1600 7FE3 F3ED 90A9 AA2C 1080 FE07 2ECE 0000 0101 0A08 EC4D BCA5 2B45
[0x000000008000244C]: 0169
packet->data_end = 0x8000244E


++++ END +++++

Start Pkt: 0x8000240C
End Pkt: 0x8000244E
Hello World from XDP
ctx = 0x0000000080402460
ctx->data       = 0x8000240C
ctx->data_end   = 0x8000244E
data.data       = 0x000000008000240C
data.data_end   = 0x000000008000244E
ETH_HDR_BASE    = 0x000000008000240C
ETH_HDR_END     = 0x000000008000241A
eth->h_proto = 8 (bpf_ntohs(eth->h_proto) = 800)
ETH_P_IP = 800 (2048)
Packet is IP
XDP result: 1
```