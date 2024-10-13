# bpf_printk()

## BPF calling convention
eBPF has 10 general purpose registers and a read-only frame pointer register, all of which are 64-bits wide.

The eBPF calling convention is defined as:
- R0: return value from function calls, and exit value for eBPF programs
- R1 - R5: arguments for function calls
- R6 - R9: callee saved registers that function calls will preserve
- R10: read-only frame pointer to access stack

> R0 - R5 are scratch registers and eBPF programs needs to spill/fill them if necessary across calls.

## Example programs
### 01 - only fmt
```C
SEC("xdp")
int hello_world(struct xdp_md *ctx) {

    bpf_printk("Hello World!\n");
    return XDP_PASS;
}
```
Gets compiled to:
```
$ llvm-objdump -d .output/main.bpf.o

.output/main.bpf.o:	file format elf64-bpf

Disassembly of section xdp:

0000000000000000 <hello_world>:
       0:	18 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00	r1 = 0 ll
       2:	b7 02 00 00 0e 00 00 00	r2 = 14
       3:	85 00 00 00 06 00 00 00	call 6
       4:	b7 00 00 00 02 00 00 00	r0 = 2
       5:	95 00 00 00 00 00 00 00	exit
```

.rodata
```
$ llvm-objdump -s -j .rodata .output/main.bpf.o

.output/main.bpf.o:	file format elf64-bpf
Contents of section .rodata:
 0000 48656c6c 6f20576f 726c6421 0a00      Hello World!..
```

### 02 - fmt (declared before) + integer
```C
SEC("xdp")
int hello_world(struct xdp_md *ctx) {

    static const char fmt[] = "Hello World from XDP: %d\n";
    bpf_trace_printk(fmt, sizeof(fmt), 5);

    return XDP_PASS;
}
```

> If you declare the fmt string before, you need to specify the string size.
> See : https://docs.ebpf.io/linux/helper-function/bpf_trace_printk/

Gets compiled to:
```
$ llvm-objdump -d .output/main.bpf.o

.output/main.bpf.o:	file format elf64-bpf

Disassembly of section xdp:

0000000000000000 <hello_world>:
       0:	18 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00	r1 = 0 ll
       2:	b7 02 00 00 1a 00 00 00	r2 = 26
       3:	b7 03 00 00 05 00 00 00	r3 = 5
       4:	85 00 00 00 06 00 00 00	call 6
       5:	b7 00 00 00 02 00 00 00	r0 = 2
       6:	95 00 00 00 00 00 00 00	exit
```

.rodata
```
$ llvm-objdump -s -j .rodata .output/main.bpf.o

.output/main.bpf.o:	file format elf64-bpf
Contents of section .rodata:
 0000 48656c6c 6f20576f 726c6420 66726f6d  Hello World from
 0010 20584450 3a202564 0a00                XDP: %d..
```

### 03 - fmt + big string
I tried to create a big string as a param to see if It would have been placed in the .rodata like the fmt string

```C
SEC("xdp")
int hello_world(struct xdp_md *ctx) {

    char buf[120];
    strcpy(buf, "abcdefghilmopqrstuvzyxwkbcdefghilmopqrstuvzyxwkacdefghilmopqrstuvzyxwkab\n");

    bpf_printk("Hello World from XDP: %s\n", buf);

    return XDP_PASS;
}
```
Gets compiled to:
```
$ llvm-objdump -d .output/main.bpf.o

.output/main.bpf.o:	file format elf64-bpf

Disassembly of section xdp:

0000000000000000 <hello_world>:
       0:	b7 01 00 00 0a 00 00 00	r1 = 10
       1:	6b 1a d0 ff 00 00 00 00	*(u16 *)(r10 - 48) = r1
       2:	18 01 00 00 76 7a 79 78 00 00 00 00 77 6b 61 62	r1 = 7089065449324378742 ll
       4:	7b 1a c8 ff 00 00 00 00	*(u64 *)(r10 - 56) = r1
       5:	18 01 00 00 6d 6f 70 71 00 00 00 00 72 73 74 75	r1 = 8463516535102664557 ll
       7:	7b 1a c0 ff 00 00 00 00	*(u64 *)(r10 - 64) = r1
       8:	18 01 00 00 63 64 65 66 00 00 00 00 67 68 69 6c	r1 = 7811889821959677027 ll
      10:	7b 1a b8 ff 00 00 00 00	*(u64 *)(r10 - 72) = r1
      11:	18 01 00 00 75 76 7a 79 00 00 00 00 78 77 6b 61	r1 = 7019835803504899701 ll
      13:	7b 1a b0 ff 00 00 00 00	*(u64 *)(r10 - 80) = r1
      14:	18 01 00 00 6c 6d 6f 70 00 00 00 00 71 72 73 74	r1 = 8391176362264587628 ll
      16:	7b 1a a8 ff 00 00 00 00	*(u64 *)(r10 - 88) = r1
      17:	18 01 00 00 62 63 64 65 00 00 00 00 66 67 68 69	r1 = 7595434461045744482 ll
      19:	7b 1a a0 ff 00 00 00 00	*(u64 *)(r10 - 96) = r1
      20:	18 01 00 00 74 75 76 7a 00 00 00 00 79 78 77 6b	r1 = 7743790547427816820 ll
      22:	7b 1a 98 ff 00 00 00 00	*(u64 *)(r10 - 104) = r1
      23:	18 01 00 00 69 6c 6d 6f 00 00 00 00 70 71 72 73	r1 = 8318836189426445417 ll
      25:	7b 1a 90 ff 00 00 00 00	*(u64 *)(r10 - 112) = r1
      26:	18 01 00 00 61 62 63 64 00 00 00 00 65 66 67 68	r1 = 7523094288207667809 ll
      28:	7b 1a 88 ff 00 00 00 00	*(u64 *)(r10 - 120) = r1
      29:	bf a3 00 00 00 00 00 00	r3 = r10
      30:	07 03 00 00 88 ff ff ff	r3 += -120
      31:	18 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00	r1 = 0 ll
      33:	b7 02 00 00 1a 00 00 00	r2 = 26
      34:	85 00 00 00 06 00 00 00	call 6
      35:	b7 00 00 00 02 00 00 00	r0 = 2
      36:	95 00 00 00 00 00 00 00	exit
```

.rodata section:
```
$ llvm-objdump -s -j .rodata .output/main.bpf.o

.output/main.bpf.o:	file format elf64-bpf
Contents of section .rodata:
 0000 48656c6c 6f20576f 726c6420 66726f6d  Hello World from
 0010 20584450 3a202573 0a00                XDP: %s..
```

### 04 - fmt (pre declared) + big string
```C
SEC("xdp")
int hello_world(struct xdp_md *ctx) {

    char buf[120];
    strcpy(buf, "abcdefghilmopqrstuvzyxwkbcdefghilmopqrstuvzyxwkacdefghilmopqrstuvzyxwkab\n");

    static const char fmt[] = "Hello World from XDP: %s\n";
    bpf_trace_printk(fmt, sizeof(fmt), buf);

    return XDP_PASS;
}
```
Gets compiled to:
```
$ llvm-objdump -d .output/main.bpf.o

.output/main.bpf.o:	file format elf64-bpf

Disassembly of section xdp:

0000000000000000 <hello_world>:
       0:	b7 01 00 00 0a 00 00 00	r1 = 10
       1:	6b 1a d0 ff 00 00 00 00	*(u16 *)(r10 - 48) = r1
       2:	18 01 00 00 76 7a 79 78 00 00 00 00 77 6b 61 62	r1 = 7089065449324378742 ll
       4:	7b 1a c8 ff 00 00 00 00	*(u64 *)(r10 - 56) = r1
       5:	18 01 00 00 6d 6f 70 71 00 00 00 00 72 73 74 75	r1 = 8463516535102664557 ll
       7:	7b 1a c0 ff 00 00 00 00	*(u64 *)(r10 - 64) = r1
       8:	18 01 00 00 63 64 65 66 00 00 00 00 67 68 69 6c	r1 = 7811889821959677027 ll
      10:	7b 1a b8 ff 00 00 00 00	*(u64 *)(r10 - 72) = r1
      11:	18 01 00 00 75 76 7a 79 00 00 00 00 78 77 6b 61	r1 = 7019835803504899701 ll
      13:	7b 1a b0 ff 00 00 00 00	*(u64 *)(r10 - 80) = r1
      14:	18 01 00 00 6c 6d 6f 70 00 00 00 00 71 72 73 74	r1 = 8391176362264587628 ll
      16:	7b 1a a8 ff 00 00 00 00	*(u64 *)(r10 - 88) = r1
      17:	18 01 00 00 62 63 64 65 00 00 00 00 66 67 68 69	r1 = 7595434461045744482 ll
      19:	7b 1a a0 ff 00 00 00 00	*(u64 *)(r10 - 96) = r1
      20:	18 01 00 00 74 75 76 7a 00 00 00 00 79 78 77 6b	r1 = 7743790547427816820 ll
      22:	7b 1a 98 ff 00 00 00 00	*(u64 *)(r10 - 104) = r1
      23:	18 01 00 00 69 6c 6d 6f 00 00 00 00 70 71 72 73	r1 = 8318836189426445417 ll
      25:	7b 1a 90 ff 00 00 00 00	*(u64 *)(r10 - 112) = r1
      26:	18 01 00 00 61 62 63 64 00 00 00 00 65 66 67 68	r1 = 7523094288207667809 ll
      28:	7b 1a 88 ff 00 00 00 00	*(u64 *)(r10 - 120) = r1
      29:	bf a3 00 00 00 00 00 00	r3 = r10
      30:	07 03 00 00 88 ff ff ff	r3 += -120
      31:	18 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00	r1 = 0 ll
      33:	b7 02 00 00 1a 00 00 00	r2 = 26
      34:	85 00 00 00 06 00 00 00	call 6
      35:	b7 00 00 00 02 00 00 00	r0 = 2
      36:	95 00 00 00 00 00 00 00	exit
```

.rodata section:
```
$ llvm-objdump -s -j .rodata .output/main.bpf.o

.output/main.bpf.o:	file format elf64-bpf
Contents of section .rodata:
 0000 48656c6c 6f20576f 726c6420 66726f6d  Hello World from
 0010 20584450 3a202573 0a00                XDP: %s..
```

### 05 - double const string
```C
SEC("xdp")
int hello_world(struct xdp_md *ctx) {

    bpf_printk("Hello World from XDP: %s\n", "abcdefg");
    return XDP_PASS;
}
```

Gets compiled to:
```
$ llvm-objdump -d .output/main.bpf.o

.output/main.bpf.o:	file format elf64-bpf

Disassembly of section xdp:

0000000000000000 <hello_world>:
       0:	18 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00	r1 = 0 ll
       2:	b7 02 00 00 1a 00 00 00	r2 = 26
       3:	18 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00	r3 = 0 ll
       5:	85 00 00 00 06 00 00 00	call 6
       6:	b7 00 00 00 02 00 00 00	r0 = 2
       7:	95 00 00 00 00 00 00 00	exit
```
.rodata section:
```
$ llvm-objdump -s -j .rodata .output/main.bpf.o

.output/main.bpf.o:	file format elf64-bpf
Contents of section .rodata:
 0000 48656c6c 6f20576f 726c6420 66726f6d  Hello World from
 0010 20584450 3a202573 0a00                XDP: %s..
```

.rodata.str1.1
```
$ llvm-objdump -s -j .rodata.str1.1 .output/main.bpf.o

.output/main.bpf.o:	file format elf64-bpf
Contents of section .rodata.str1.1:
 0000 61626364 65666700                    abcdefg.
```

## Conclusion
