# E06: Handle multiple packets

This example load multiple packets from the memory and for each one runs an XDP program that, if the packet is IPv4, prints the source and dest IPs.

Example output:
```
root@devbox:~/git/ebpf_llvm_jit/examples/06_qemu_riscv_xdp_multiple_pkts# make run
Ctrl-A C for QEMU console, then quit to exit
qemu-system-riscv64 -nographic -serial mon:stdio -machine virt -bios .output/hello.elf
Started runtime
Start MM Region: 0x80003478 
End MM Region: 0x80003520 

COMPUTING SIZE x PACKET # 0

[0x0000000080003478]: 0C00 AA29 FFFD E000 684C 5406 0008 0045 3400 0000 0040 0640 C6B3 A8C0 B902 A8C0
[0x0000000080003498]: F402 F8CF 1600 7FE3 F3ED 90A9 AA2C 1080 FE07 2ECE 0000 0101 0A08 EC4D BCA5 2B45
[0x00000000800034B8]: 0169
END: 0x00000000800034C2

COMPUTING SIZE x PACKET # 1

[0x00000000800034C2]: A948 348A BA52 0C00 AA29 FFFD 0008 0045 4800 5C13 0040 1140 9D53 A8C0 F402 0808
[0x00000000800034E2]: 0808 E386 3500 3400 F1D3 3658 0001 0100 0000 0000 0000 3103 3538 3201 3103 3836
[0x0000000080003502]: 3103 3239 6907 2D6E 6461 7264 6104 7072 0061 0C00 0100
END: 0x0000000080003520



EXECUTING XDP_MD # 0...
Hello World from XDP
ctx = 0x00000000804034B0
ctx->data       = 0x80003478
ctx->data_end   = 0x800034C2
data.data       = 0x0000000080003478
data.data_end   = 0x00000000800034C2
ETH_HDR_BASE    = 0x0000000080003478
ETH_HDR_END     = 0x0000000080003486
eth->h_proto = 8 (bpf_ntohs(eth->h_proto) = 800)
ETH_P_IP = 800 (2048)
Packet is IP
source_address: 192.168.2.185 (0xB902A8C0)
dest_address:   192.168.2.244 (0xF402A8C0)
XDP result: 2

EXECUTING XDP_MD # 1...
Hello World from XDP
ctx = 0x00000000804034C8
ctx->data       = 0x800034C2
ctx->data_end   = 0x80003520
data.data       = 0x00000000800034C2
data.data_end   = 0x0000000080003520
ETH_HDR_BASE    = 0x00000000800034C2
ETH_HDR_END     = 0x00000000800034D0
eth->h_proto = 8 (bpf_ntohs(eth->h_proto) = 800)
ETH_P_IP = 800 (2048)
Packet is IP
source_address: 192.168.2.244 (0xF402A8C0)
dest_address:   8.8.8.8 (0x8080808)
XDP result: 2
```