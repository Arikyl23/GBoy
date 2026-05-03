cmake_minimum_required(VERSION 3.28.3)

add_library(Logging STATIC src/log.c)
target_include_directories(Logging
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)