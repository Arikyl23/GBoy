# Add lower level tests first
add_subdirectory(cartridge)

# Add MMU Unit Tests
set(code_files unit_test_mmu.c)
set(exec_name unit_test_mmu)
set(test_name "[UNIT] MMU")
add_executable(${exec_name} ${code_files})
target_link_libraries(${exec_name} PRIVATE GBoy_Internal_Test)
add_test(
    NAME ${test_name}
    COMMAND ${exec_name}
)
set_tests_properties(${test_name} PROPERTIES LABELS "Unit")

# Add MMU Integration Tests
set(code_files int_test_mmu.c)
set(exec_name int_test_mmu)
set(test_name "[INT] MMU")
add_executable(${exec_name} ${code_files})
target_link_libraries(${exec_name} PRIVATE GBoy_Internal_Test)
add_test(
    NAME ${test_name}
    COMMAND ${exec_name}
)
set_tests_properties(${test_name} PROPERTIES LABELS "Integration")