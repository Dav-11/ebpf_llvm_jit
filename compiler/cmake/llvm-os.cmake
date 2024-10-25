
find_package(LLVM REQUIRED CONFIG)

if (${LLVM_PACKAGE_VERSION} VERSION_LESS 15)
    message(FATAL_ERROR "LLVM version must be >=15")
endif()

option(ENABLE_LLVM_SHARED "Link shared library of LLVM" YES)

if(ENABLE_LLVM_SHARED)
    set(LLVM_LIBRARIES LLVM)
else()
    llvm_map_components_to_libnames(LLVM_LIBRARIES
            Core
            OrcJIT
            mcjit
            Support
            nativecodegen
    )
endif()

message(STATUS "LLVM_LIBRARIES=${LLVM_LIBRARIES}")