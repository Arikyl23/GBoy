add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cpu)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/display)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/events)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/memory)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ppu)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/utility)

target_sources(GBoy_Internal
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/gboy.c
        ${CMAKE_CURRENT_SOURCE_DIR}/debugger.c
        ${CMAKE_CURRENT_SOURCE_DIR}/assets.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lcd.c
)
