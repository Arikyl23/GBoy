# Add Register Unit Tests
set(code_files unit_test_registers.c)
set(exec_name unit_test_cpu_registers)
set(test_name "[UNIT] CPU/Registers")
add_executable(${exec_name} ${code_files})
target_link_libraries(${exec_name} PRIVATE GBoy_Internal_Test)
add_test(
    NAME ${test_name}
    COMMAND ${exec_name}
)
set_tests_properties(${test_name} PROPERTIES LABELS "Unit")

# Add ALU Unit Tests
set(code_files unit_test_alu.c)
set(exec_name unit_test_cpu_alu)
set(test_name "[UNIT] CPU/ALU")
add_executable(${exec_name} ${code_files})
target_link_libraries(${exec_name} PRIVATE GBoy_Internal_Test)
add_test(
    NAME ${test_name}
    COMMAND ${exec_name}
)
set_tests_properties(${test_name} PROPERTIES LABELS "Unit")
