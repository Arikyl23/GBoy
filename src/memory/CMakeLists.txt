target_sources(GBoy_Internal
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/mmu.c
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cartridge)
