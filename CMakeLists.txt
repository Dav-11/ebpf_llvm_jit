cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 17)

project(
        ebpf_llvm_jit
        VERSION "0.1.0"
)


include(FetchContent)

#######################################
 # INCLUDE OTHER .CMAKE
#######################################
include(cmake/libbpf.cmake)
include(cmake/llvm-15.cmake)
#include(cmake/llvm-os.cmake)
include(cmake/argparse.cmake)
include(cmake/spdlog.cmake)


#######################################
 # INCLUDE HEADERS
#######################################
include_directories(${LIBBPF_INCLUDE_DIRS})
include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${ARGPARSE_INCLUDE_DIRS})
include_directories(${SPDLOG_INCLUDE_DIRS})


#######################################
 # ACTUAL BUILD
#######################################
add_executable(ebpf_llvm_jit
        main.cpp
        src/ebpf_inst.h
        src/jit/bpftime_llvm_jit_vm.cpp
        src/jit/bpftime_llvm_jit_vm.h
        src/jit/llvm_bpf_jit_context.cpp
        src/jit/llvm_bpf_jit_context.h
        src/jit/external_function.cpp
        src/jit/external_function.h
        src/utils/bo.h
        src/jit/compiler.cpp
        src/jit/code_gen.cpp
        src/jit/code_gen.h)

#######################################
 # EXT LIBRARIES LINKING
#######################################
#target_link_libraries(ebpf_llvm_jit PRIVATE ${LIBBPF_LIBRARIES})
#target_link_libraries(ebpf_llvm_jit PRIVATE ${LLVM_LIBRARIES})
#target_link_libraries(ebpf_llvm_jit PRIVATE ${ARGPARSE_LIBRARIES})
#target_link_libraries(ebpf_llvm_jit PRIVATE ${SPDLOG_LIBRARIES})

target_link_libraries(ebpf_llvm_jit PUBLIC ${LLVM_LIBRARIES} PRIVATE spdlog::spdlog argparse ${LIBBPF_LIBRARIES} elf z)


# IDK WHAT THIS DOES
add_dependencies(ebpf_llvm_jit libbpf spdlog argparse)
