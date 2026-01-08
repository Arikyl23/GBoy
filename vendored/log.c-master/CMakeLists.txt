cmake_minimum_required(VERSION 3.28.3)

add_library(lib_logging STATIC src/log.c)

target_include_directories(lib_logging PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")