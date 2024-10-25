# spdlog.cmake

include(FetchContent)

message(STATUS "Downloading and building spdlog...")

# Declare the spdlog content to be fetched
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.11.0  # Replace with the desired version tag
        GIT_SHALLOW TRUE
)

# Download and make spdlog available
FetchContent_MakeAvailable(spdlog)

# Set spdlog include directories and libraries
set(SPDLOG_INCLUDE_DIRS ${spdlog_SOURCE_DIR}/include)
set(SPDLOG_LIBRARIES spdlog)

message(STATUS "DONE")
