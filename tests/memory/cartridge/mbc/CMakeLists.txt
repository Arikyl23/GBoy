# Add DLD Unit Tests
add_executable(unit_test_memory_cartridge_mbc_dld unit_test_dld.c)
target_include_directories(unit_test_memory_cartridge_mbc_dld PRIVATE ${CMAKE_SOURCE_DIR}/tests)
target_link_libraries(unit_test_memory_cartridge_mbc_dld PRIVATE GBoy_Internal lib_logging)
add_test(
    NAME "[UNIT] Memory/Cartridge/MBC/DLD"
    COMMAND unit_test_memory_cartridge_mbc_dld
)

# Add MBC1 Unit Tests
add_executable(unit_test_memory_cartridge_mbc_mbc1 unit_test_mbc1.c)
target_include_directories(unit_test_memory_cartridge_mbc_mbc1 PRIVATE ${CMAKE_SOURCE_DIR}/tests)
target_link_libraries(unit_test_memory_cartridge_mbc_mbc1 PRIVATE GBoy_Internal lib_logging)
add_test(
    NAME "[UNIT] Memory/Cartridge/MBC/MBC1"
    COMMAND unit_test_memory_cartridge_mbc_mbc1
)