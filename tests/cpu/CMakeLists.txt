# Add Register Unit Tests
add_executable(unit_test_cpu_registers registers/unit_test_registers.c)
target_link_libraries(unit_test_cpu_registers PRIVATE GBoy_Internal_Test)
add_test(
    NAME "[UNIT] CPU/Registers"
    COMMAND unit_test_cpu_registers
)

# Add ALU Unit Tests
add_executable(unit_test_cpu_alu alu/unit_test_alu.c)
target_link_libraries(unit_test_cpu_alu PRIVATE GBoy_Internal_Test)
add_test(
    NAME "[UNIT] CPU/ALU"
    COMMAND unit_test_cpu_alu
)
