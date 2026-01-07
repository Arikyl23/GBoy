cmake_minimum_required(VERSION 3.28.3)

add_library(log_lib STATIC src/log.c)

target_include_directories(log_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")