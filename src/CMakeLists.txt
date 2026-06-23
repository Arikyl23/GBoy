add_subdirectory(cpu)
add_subdirectory(display)
add_subdirectory(events)
add_subdirectory(memory)
add_subdirectory(ppu)
add_subdirectory(utility)

target_sources(GBoy_Internal
    INTERFACE
        gboy.c
        debugger.c
		assets.cpp
        lcd.c
)