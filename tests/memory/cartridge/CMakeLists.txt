# Add DLD Unit Tests
add_executable(int_test_memory_cartridge int_test_cartridge.c)
target_link_libraries(int_test_memory_cartridge PRIVATE GBoy_Internal_Test SDL3::SDL3)
add_test(
    NAME "[INT] Memory/Cartridge"
    COMMAND int_test_memory_cartridge
)

add_subdirectory(mbc)