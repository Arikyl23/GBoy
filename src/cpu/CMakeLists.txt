target_sources(GBoy_Internal
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/registers.c
        ${CMAKE_CURRENT_SOURCE_DIR}/alu.c
        ${CMAKE_CURRENT_SOURCE_DIR}/cpu.c
)
