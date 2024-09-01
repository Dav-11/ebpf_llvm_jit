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
        src/jit/vm.cpp
        src/jit/vm.h
        src/jit/context.cpp
        src/jit/context.h
        src/jit/external_function.cpp
        src/jit/external_function.h
        src/jit/compiler.cpp
        src/jit/code_gen.cpp
        src/jit/code_gen.h
        src/helpers/helper.cpp
        src/helpers/helper.h
        src/include/helpers_impl.c
        src/include/helpers_impl.h
)

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
