# ebpf_llvm_jit

## Description
The target of this project is to create a compiler that can compile BPF object files to native RISCV code.

This can be considered a fork of [eunomia-bpf/llvmbpf](https://github.com/eunomia-bpf/llvmbpf) as it was used as the starting point.

## Limitations
The compiler is still in its early stage, as such, it can handle only XDP programs with no maps.
Also only the `bpf_printk()` helper function is supported.

## Examples
There are some working usage examples in the [examples](examples/) folder.

## Build
This project can be compiled only on linux systems due to the need of libbpf.

### Requirements
- LLVM 15
- zlib1g-dev
- libelf-dev
- git
- make
- cmake
- g++
- gcc
- pkgconf

> If running on debian/ubuntu there is a script
```shell
.devcontainer/debian.sh -h
```
```
Usage: .devcontainer/debian.sh [OPTION]
Install main dependencies for your project.

Options:
  -a         Install both main and QEMU dependencies.
  -q         Install only QEMU dependencies.
  -h         Display this help message.
```

### Procedure
To compile the project use cmake:
1. Create a `build` folder
```shell
mkdir build
```

2. Enter `build` folder
```shell
cd build
```

3. run cmake to generate build files and dependencies
```shell
cmake ..
```

4. then run make to actually build the project
```shell
make
```
