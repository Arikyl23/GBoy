# Add lower level tests first
add_subdirectory(mbc)

# Add Cartridge Integration Tests
set(code_files int_test_cartridge.c)
set(exec_name "int_test_memory_cartridge")
set(test_name "[INT] Memory/Cartridge")
add_executable(${exec_name} ${code_files})
target_link_libraries(${exec_name} PRIVATE GBoy_Internal_Test SDL3::SDL3)
add_test(
    NAME ${test_name}
    COMMAND ${exec_name}
)
set_tests_properties(${test_name} PROPERTIES LABELS "Integration")