add_subdirectory(cpu)
add_subdirectory(ppu)
add_subdirectory(memory)
add_subdirectory(display)
add_subdirectory(events)

target_sources(GBoy_Internal
    INTERFACE
        gboy.c
        debugger.c
		assets.cpp
)