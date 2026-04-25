# Add DLD Unit Tests
set(code_files unit_test_dld.c)
set(exec_name unit_test_memory_cartridge_mbc_dld)
set(test_name "[UNIT] Memory/Cartridge/MBC/DLD")
add_executable(${exec_name} ${code_files})
target_link_libraries(${exec_name} PRIVATE GBoy_Internal_Test)
add_test(
    NAME ${test_name}
    COMMAND ${exec_name}
)
set_tests_properties(${test_name} PROPERTIES LABELS "Unit")

# Add MBC1 Unit Tests
set(code_files unit_test_mbc1.c)
set(exec_name "unit_test_memory_cartridge_mbc_mbc1")
set(test_name "[UNIT] Memory/Cartridge/MBC/MBC1")
add_executable(${exec_name} ${code_files})
target_link_libraries(${exec_name} PRIVATE GBoy_Internal_Test)
add_test(
    NAME ${test_name}
    COMMAND ${exec_name}
)
set_tests_properties(${test_name} PROPERTIES LABELS "Unit")

# Add MBC Integration Tests
set(code_files int_test_mbc.c)
set(exec_name "int_test_memory_cartridge_mbc")
set(test_name "[INT] Memory/Cartridge/MBC")
add_executable(${exec_name} ${code_files})
target_link_libraries(${exec_name} PRIVATE GBoy_Internal_Test)
add_test(
    NAME ${test_name}
    COMMAND ${exec_name}
)
set_tests_properties(${test_name} PROPERTIES LABELS "Integration")