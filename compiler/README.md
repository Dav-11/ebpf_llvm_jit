# Compiler
A XCompiler from BPF to native RISCV64 baremetal.

> This is based on [eunomia-bpf/llvmbpf](https://github.com/eunomia-bpf/llvmbpf), which was used as a starting point.

## Limitations
The compiler is still in its early stage, as such, it can handle only XDP programs with no maps.
Also only the `bpf_printk()` helper function is supported.

## Requirements
- LLVM 15
- zlib1g-dev
- libelf-dev
- git
- make
- cmake
- g++
- gcc
- pkgconf

### Script
There is an install script for debian/ubuntu [debian.sh](.devcontainer/debian.sh)
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

## Build
This project can be compiled only on linux systems due to the need of libbpf.

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