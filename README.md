# ebpf_llvm_jit

This repository has three parts:

## [Compiler](compiler/README.md)
A XCompiler from BPF to native RISCV64 (this can be considered a fork of [eunomia-bpf/llvmbpf](https://github.com/eunomia-bpf/llvmbpf) as it was used as the starting point)
   
## [Runtime](rv64_baremetal_runtime)
A basic baremetal runtime for `qemu-system-riscv64`

## [Examples](examples)
There are some working usage of XDP BPF programs running on baremetal qemu.







