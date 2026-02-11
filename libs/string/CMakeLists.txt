cmake_minimum_required(VERSION 3.28.3)
project(String LANGUAGES C VERSION "0.1.0")

# ----- Construct String Internal INTERFACE Library -----
add_library(String_Internal INTERFACE)
target_include_directories(String_Internal
    INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_subdirectory(src)
target_link_libraries(String_Internal
    INTERFACE
        Logging
        SDL3::SDL3
)

# ----- Construct String Production Library -----
add_library(String STATIC)
target_link_libraries(String PRIVATE String_Internal)
target_include_directories(String PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# ----- Construct String Testing Library -----
add_library(String_Internal_Test STATIC)
target_link_libraries(String_Internal_Test PUBLIC String_Internal)

# ----- Testing -----
enable_testing()
add_subdirectory(tests)