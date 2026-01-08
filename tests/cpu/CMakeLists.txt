add_executable(test_registers test_registers.c)
target_include_directories(test_registers PRIVATE ${CMAKE_SOURCE_DIR}/tests)
target_link_libraries(test_registers PRIVATE lib_GBoy lib_logging)

add_test(
    NAME "Test Module GBoy/CPU/Registers"
    COMMAND test_registers
)