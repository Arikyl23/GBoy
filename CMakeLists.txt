cmake_minimum_required(VERSION 3.28.3)
project(GBoy LANGUAGES C CXX VERSION "0.1.0")

option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
if(ENABLE_ASAN)
    message(STATUS "AddressSanitizer enabled")
    # These flags must be applied to both compilation and linking
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer -g)
    add_link_options(-fsanitize=address)
endif()

# Configuration Header (Global #define statements)
set(CONFIG_HEADER "${CMAKE_SOURCE_DIR}/src/configuration.h")

# Setup bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")

# Add DEFINE that holds the length of the absolute path preceeding the projects relavtive paths
string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
add_definitions(-DPROJECT_SOURCE_DIR_SIZE=${SOURCE_PATH_SIZE})

# Add External Vendored Libraries (Require own compilation)
add_subdirectory(vendored/log.c-master)
add_subdirectory(vendored/SDL-release-3.4.0 EXCLUDE_FROM_ALL)
set(SDLTTF_VENDORED ON)
add_subdirectory(vendored/SDL_ttf-release-3.2.0 EXCLUDE_FROM_ALL)

# ----- Construct GBoy Asset Library -----
include(CMakeRC.cmake)
cmrc_add_resource_library(Assets
    NAMESPACE GBoy
    ALIAS GBoy::Assets
    assets/fonts/JetBrainsMono-Regular.ttf
)

# ----- Construct GBoy Internal INTERFACE Library -----
# This creates a virtual library that does not compile any source files
add_library(GBoy_Internal INTERFACE)
add_library(GBoy::Internal ALIAS GBoy_Internal)
# All source/headers are in /src
target_include_directories(GBoy_Internal INTERFACE "${CMAKE_SOURCE_DIR}/src")
add_subdirectory(src)
# Add the libraries the source depends on
target_link_libraries(GBoy_Internal
    INTERFACE
        Logging
        SDL3::SDL3
        SDL3_ttf::SDL3_ttf
)
# Force Includes the configuration header (so we dont have to manual include in everywhere)
target_compile_options(GBoy_Internal
    INTERFACE
        "$<$<C_COMPILER_ID:GNU,Clang>:-include${CONFIG_HEADER}>"
        "$<$<C_COMPILER_ID:MSVC>:/FI${CONFIG_HEADER}>"
)

# ----- Construct GBoy Internal Production Library -----
# This creates a static library (compiled and linked)
add_library(GBoy_Internal_Prod STATIC)
add_library(GBoy::Internal::Prod ALIAS GBoy_Internal_Prod)
# Brings in all dependencies but hides them from further propagation
# (This is required else INTERFACE library go burr)
target_link_libraries(GBoy_Internal_Prod PRIVATE GBoy::Internal)
# The Logging Library should persist past this target
target_link_libraries(GBoy_Internal_Prod PUBLIC Logging)
# The /src include directory should persist past this target
target_include_directories(GBoy_Internal_Prod PUBLIC "${CMAKE_SOURCE_DIR}/src")

# ----- Construct GBoy Internal Testing Library -----
# This creates a static library (compiled and linked)
add_library(GBoy_Internal_Test STATIC)
add_library(GBoy::Internal::Test ALIAS GBoy_Internal_Test)
# Brings in all dependencies but hides them from further propagation
# (This is required else INTERFACE library go burr)
target_link_libraries(GBoy_Internal_Test PRIVATE GBoy::Internal)
# The Logging Library should persist past this target
target_link_libraries(GBoy_Internal_Test PUBLIC Logging)
# Addes a #define TESTING to this target and all further targets
target_compile_definitions(GBoy_Internal_Test PUBLIC TESTING)
# The /src and /test include directories should persist past this target
target_include_directories(GBoy_Internal_Test
    PUBLIC
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/tests"
)

# Setup GBoy Executable (Simple Wrapper for lib_GBoy)
add_executable(GBoy 
    src/main.c
    src/assets.cpp
)
target_link_libraries(GBoy
PRIVATE
    GBoy::Internal::Prod
    GBoy::Assets
)

# ----- Testing -----
enable_testing()
add_subdirectory(tests)