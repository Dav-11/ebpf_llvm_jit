# argparse.cmake

include(FetchContent)

message(STATUS "Downloading and building argparse...")

# Declare the argparse content to be fetched
FetchContent_Declare(
        argparse
        GIT_REPOSITORY https://github.com/p-ranav/argparse.git
        GIT_TAG v3.0  # Replace with the desired version tag
        GIT_SHALLOW TRUE
)

# Download and make argparse available
FetchContent_MakeAvailable(argparse)

# Set argparse include directories and libraries
set(ARGPARSE_INCLUDE_DIRS ${argparse_SOURCE_DIR}/include)
set(ARGPARSE_LIBRARIES argparse)

message(STATUS "DONE")
