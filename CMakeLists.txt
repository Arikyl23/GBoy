cmake_minimum_required(VERSION 3.28.3)
project(GBoy LANGUAGES C VERSION "0.1.0")

# Setup bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")

# Add DEFINE that holds the length of the absolute path preceeding the projects relavtive paths
string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
add_definitions(-DPROJECT_SOURCE_DIR_SIZE=${SOURCE_PATH_SIZE})

# Add External Libraries (Require own compilation)
add_subdirectory(vendored/log.c-master)
add_subdirectory(vendored/SDL-release-3.4.0 EXCLUDE_FROM_ALL)

# Construct GBoy library (Contains the logic)
add_library(lib_GBoy STATIC)
add_subdirectory(src)
target_include_directories(lib_GBoy PUBLIC src)
target_link_libraries(lib_GBoy
    PRIVATE
        lib_logging
        SDL3::SDL3
)

# Setup GBoy Executable (Simple Wrapper for lib_GBoy)
add_executable(GBoy)
target_sources(GBoy PRIVATE src/main.c)
target_link_libraries(GBoy PRIVATE lib_GBoy)

enable_testing()
add_subdirectory(tests)