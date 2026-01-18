# Add DLD Unit Tests
add_executable(unit_test_memory_cartridge_mbc_dld unit_test_dld.c)
target_link_libraries(unit_test_memory_cartridge_mbc_dld PRIVATE GBoy_Internal_Test)
add_test(
    NAME "[UNIT] Memory/Cartridge/MBC/DLD"
    COMMAND unit_test_memory_cartridge_mbc_dld
)

# Add MBC1 Unit Tests
add_executable(unit_test_memory_cartridge_mbc_mbc1 unit_test_mbc1.c)
target_link_libraries(unit_test_memory_cartridge_mbc_mbc1 PRIVATE GBoy_Internal_Test)
add_test(
    NAME "[UNIT] Memory/Cartridge/MBC/MBC1"
    COMMAND unit_test_memory_cartridge_mbc_mbc1
)

# Add MBC Integration Tests
add_executable(int_test_memory_cartridge_mbc int_test_mbc.c)
target_link_libraries(int_test_memory_cartridge_mbc PRIVATE GBoy_Internal_Test)
add_test(
    NAME "[INT] Memory/Cartridge/MBC"
    COMMAND int_test_memory_cartridge_mbc
)