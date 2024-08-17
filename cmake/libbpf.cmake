# libbpf.cmake

include(FetchContent)

# Declare the libbpf content to be fetched
FetchContent_Declare(
        libbpf
        GIT_REPOSITORY https://github.com/libbpf/libbpf.git
        GIT_TAG v1.4.5  # Replace with the desired version tag
        GIT_SHALLOW TRUE
)

# Set libbpf build options
set(LIBBPF_BUILD_STATIC_ONLY ON CACHE BOOL "Build only the static library")
set(LIBBPF_ENABLE_SHARED OFF CACHE BOOL "Disable shared library")
set(LIBBPF_FORCE_STATIC_LIB ON CACHE BOOL "Force using static libbpf")

# Download and build libbpf
FetchContent_MakeAvailable(libbpf)

# Provide libbpf variables to the main CMakeLists.txt
set(LIBBPF_INCLUDE_DIRS ${libbpf_SOURCE_DIR}/src)
set(LIBBPF_LIBRARIES ${libbpf_BINARY_DIR}/src/libbpf.a)
