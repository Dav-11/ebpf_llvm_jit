# llvm-15.cmake

option(ENABLE_LLVM_SHARED "Link shared library of LLVM" YES)

if(ENABLE_LLVM_SHARED)

        message(STATUS "Configuring project for shared LLVM...")

        find_package(LLVM REQUIRED CONFIG)

        if (${LLVM_PACKAGE_VERSION} VERSION_LESS 15)
                message(FATAL_ERROR "LLVM version must be >=15")
        endif()

        set(LLVM_LIBRARIES LLVM)
else()

        include(FetchContent)

        message(STATUS "Downloading and building llvm...")

        FetchContent_Declare(
                llvm
                GIT_REPOSITORY https://github.com/llvm/llvm-project.git
                GIT_TAG llvmorg-15.0.0
                GIT_SHALLOW TRUE
        )

        # Set LLVM build options
        set(LLVM_ENABLE_PROJECTS "clang;lld;libcxx;libcxxabi" CACHE STRING "LLVM projects to build")
        set(LLVM_TARGETS_TO_BUILD "X86;RISCV;ARM;AArch64" CACHE STRING "Targets to build")
        set(LLVM_ENABLE_RUNTIMES "all" CACHE STRING "Runtimes to build")
        set(LLVM_ENABLE_RTTI ON CACHE BOOL "Enable RTTI")
        set(LLVM_ENABLE_EH ON CACHE BOOL "Enable Exception Handling")

        # Download LLVM
        FetchContent_MakeAvailable(llvm)

        # Configure and build LLVM
        set(LLVM_BUILD_DIR ${llvm_BINARY_DIR})
        set(LLVM_INSTALL_DIR ${CMAKE_BINARY_DIR}/llvm-install)

        # Create a custom target to build and install LLVM
        add_custom_target(build-llvm
                COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" -S ${llvm_SOURCE_DIR}/llvm -B ${LLVM_BUILD_DIR}
                COMMAND ${CMAKE_COMMAND} --build ${LLVM_BUILD_DIR} --target install
                -D CMAKE_INSTALL_PREFIX=${LLVM_INSTALL_DIR}
                -D LLVM_ENABLE_PROJECTS=${LLVM_ENABLE_PROJECTS}
                -D LLVM_TARGETS_TO_BUILD=${LLVM_TARGETS_TO_BUILD}
                -D LLVM_ENABLE_RTTI=${LLVM_ENABLE_RTTI}
                -D LLVM_ENABLE_EH=${LLVM_ENABLE_EH}
                -D CMAKE_BUILD_TYPE=Release
        )

        # Set LLVM CMake configuration
        set(LLVM_DIR ${llvm_SOURCE_DIR}/llvm/cmake/modules CACHE PATH "Path to LLVM cmake modules")

        # Locate LLVM package
        find_package(LLVM REQUIRED CONFIG)

        # Link LLVM components
        llvm_map_components_to_libnames(LLVM_LIBRARIES
                Core
                OrcJIT
                mcjit
                Support
                nativecodegen
        )

        # # Provide LLVM variables to the main CMakeLists.txt
        # set(LLVM_INCLUDE_DIRS ${LLVM_INCLUDE_DIRS})
        # set(LLVM_LIBRARIES ${LLVM_LIBRARIES})
endif()

message(STATUS "DONE")
