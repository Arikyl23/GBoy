cmake_minimum_required(VERSION 3.28.3)
project(GBoy LANGUAGES C VERSION "0.1.0")

# Setup bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>")

# Add DEFINE that holds the length of the absolute path preceeding the projects relavtive paths
string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
add_definitions(-DPROJECT_SOURCE_DIR_SIZE=${SOURCE_PATH_SIZE})

# Add External Libraries (Require own compilation)
add_subdirectory(vendored/log.c-master)
add_subdirectory(vendored/SDL-release-3.4.0 EXCLUDE_FROM_ALL)

# Setup GBoy Executable
add_executable(GBoy src/main.c)

target_sources(GBoy
    PRIVATE
        src/main.c
        src/cpu/registers/registers.c
)


target_include_directories(GBoy PRIVATE src)

target_link_libraries(GBoy 
    PRIVATE 
        log_lib
        SDL3::SDL3
)