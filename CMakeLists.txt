cmake_minimum_required(VERSION 3.29)

project(ebpf_llvm_jit)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)

add_executable(ebpf_llvm_jit
        main.cpp)

#######################################
 # EXT LIBRARIES
#######################################

find_package(spdlog CONFIG REQUIRED)
find_package(argparse CONFIG REQUIRED)

target_link_libraries(ebpf_llvm_jit PRIVATE spdlog::spdlog)
target_link_libraries(ebpf_llvm_jit PRIVATE argparse::argparse)