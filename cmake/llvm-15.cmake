# llvm-15.cmake

include(FetchContent)

FetchContent_Declare(
        llvm
        GIT_REPOSITORY https://github.com/llvm/llvm-project.git
        GIT_TAG llvmorg-15.0.0
        GIT_SHALLOW TRUE
)

# Set LLVM build options
set(LLVM_ENABLE_PROJECTS "clang;lld;libcxx;libcxxabi" CACHE STRING "LLVM projects to build")
set(LLVM_TARGETS_TO_BUILD "X86" CACHE STRING "Targets to build")
set(LLVM_ENABLE_RTTI ON CACHE BOOL "Enable RTTI")
set(LLVM_ENABLE_EH ON CACHE BOOL "Enable Exception Handling")

# Download and build LLVM
FetchContent_MakeAvailable(llvm)

# Set LLVM CMake configuration
set(LLVM_DIR ${llvm_SOURCE_DIR}/llvm/cmake/modules CACHE PATH "Path to LLVM cmake modules")

# Locate LLVM package
find_package(LLVM REQUIRED CONFIG)

# Link LLVM components
llvm_map_components_to_libnames(LLVM_LIBRARIES
        OrcJIT
        Core
        ExecutionEngine
        MCJIT
        Support
)

# Provide LLVM variables to the main CMakeLists.txt
set(LLVM_INCLUDE_DIRS ${LLVM_INCLUDE_DIRS} PARENT_SCOPE)
set(LLVM_LIBRARIES ${LLVM_LIBRARIES} PARENT_SCOPE)
