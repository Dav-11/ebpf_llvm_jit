# ebpf_llvm_jit

## Compile
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

### Procedure
To compile the project use cmake:
- run in root folder to generate build files and dependencies
```shell
cmake .
```
- then run to actually build the project
```shell
make
```