cmake_minimum_required(VERSION 3.28.3)
project(GBoy)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>")

add_subdirectory(vendored/SDL-release-3.4.0 EXCLUDE_FROM_ALL)

add_executable(GBoy)

target_sources(GBoy
    PRIVATE
        src/main.c
        src/cpu/registers/registers.c
)

target_include_directories(GBoy PRIVATE src)

target_link_libraries(GBoy PRIVATE SDL3::SDL3)