# libbpf.cmake

include(ExternalProject)

message(STATUS "Downloading and building libbpf...")

set(LIBBPF_DIR ${CMAKE_CURRENT_LIST_DIR}/../third_party/libbpf/)

# Declare the libbpf content to be fetched
ExternalProject_Add(
        libbpf
        GIT_REPOSITORY "https://github.com/libbpf/libbpf.git"
        GIT_TAG "v1.4.5"
        GIT_SUBMODULES_RECURSE TRUE

        SOURCE_DIR ${LIBBPF_DIR} # Source directory into which downloaded contents will be unpacked/ cloned

        CONFIGURE_COMMAND "mkdir" "-p" "${CMAKE_CURRENT_BINARY_DIR}/libbpf/libbpf"
        BUILD_COMMAND "cd" "src/" "&&" "INCLUDEDIR=" "LIBDIR=" "UAPIDIR=" "OBJDIR=${CMAKE_CURRENT_BINARY_DIR}/libbpf/libbpf" "DESTDIR=${CMAKE_CURRENT_BINARY_DIR}/libbpf" "make" "CFLAGS=-g -O2 -Werror -Wall -std=gnu89 -fPIC -fvisibility=hidden -DSHARED -DCUSTOM_DEFINE=1" "-j" "install"
        BUILD_IN_SOURCE TRUE
        BUILD_ALWAYS TRUE
        INSTALL_COMMAND ""
        BUILD_BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/libbpf/libbpf.a
)

# Provide libbpf variables to the main CMakeLists.txt
set(LIBBPF_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/libbpf/)
set(LIBBPF_LIBRARIES ${CMAKE_CURRENT_BINARY_DIR}/libbpf/libbpf.a)

message(STATUS "DONE")
