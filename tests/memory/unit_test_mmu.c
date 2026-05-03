/**
 * @file unit_test_mmu.c
 * @brief Unit Tests for the Memory Management Unit (MMU).
 */
#include <stdbool.h>
#include <string.h>

#include "DEFINES.h"
#include "memory/hw_registers.h"
#include "memory/memory_map.h"
#include "memory/mmu.h"
#include "testing.h"

LOG_MODULE_SETUP_DEFAULT("UNIT TEST MMU");

static const byte m_fill_pattern = 0xCD;

static bool m_mock_read_called  = false;
static bool m_mock_write_called = false;
static word m_mock_last_addr    = 0x0000;
static byte m_mock_last_value   = m_fill_pattern;

bool test_mmu_module(void);

bool test_memory_mapping(void);
bool test_blocking(void);
bool test_cartridge_load_eject(void);

bool test_memory_mapping_rom(void);
bool test_memory_mapping_vram(void);
bool test_memory_mapping_cart_ram(void);
bool test_memory_mapping_wram(void);
bool test_memory_mapping_echo_ram(void);
bool test_memory_mapping_oam(void);
bool test_memory_mapping_prohibited(void);
bool test_memory_mapping_io(void);
bool test_memory_mapping_hram(void);
bool test_memory_mapping_ie(void);

bool test_blocking_vram(void);
bool test_blocking_oam(void);

bool assert_bus_read(byte* p_test_byte, const word test_addr, const byte test_value);
bool assert_bus_no_read(byte* p_test_byte, const word test_addr, const byte test_value);
bool assert_bus_write(byte* p_test_byte, const word test_addr, const byte test_value);
bool assert_bus_no_write(byte* p_test_byte, const word test_addr, const byte test_value);

void reset_mock(void);
byte hook_cart_read(struct cartridge* cart, const word addr);
void hook_cart_write(struct cartridge* cart, const word addr, const byte value);
bool assert_cart_read(const word addr, const byte expected_value);
bool assert_cart_no_read(const word addr, const byte expected_value);
bool assert_cart_write(const word addr, const byte value);
bool assert_cart_no_write(const word addr, const byte value);

void reset_memory();
void set_pattern(byte* array, const size_t size, const byte fill_pattern);

int main(void) { return (test_mmu_module() == true) ? 0 : 1; }

bool test_mmu_module(void) {
    mmu_testing_set_cartridge_hooks(hook_cart_read, hook_cart_write);
    reset_memory();

    ASSERT_TRUE(test_memory_mapping(), "Memory Mapping test(s) failed");
    // ASSERT_TRUE(test_oam_vram_blocking(), "OAM and VRAM Block Test failed");
    // ASSERT_TRUE(test_cartridge_load_eject(), "Cartridge Load and Eject Test failed");
    return true;
}

bool test_memory_mapping(void) {
    ASSERT_TRUE(test_memory_mapping_rom(), "ROM test failed");
    ASSERT_TRUE(test_memory_mapping_vram(), "VRAM test failed");
    ASSERT_TRUE(test_memory_mapping_cart_ram(), "Cart RAM test failed");
    ASSERT_TRUE(test_memory_mapping_wram(), "WRAM test failed");
    ASSERT_TRUE(test_memory_mapping_echo_ram(), "Echo RAM test failed");
    ASSERT_TRUE(test_memory_mapping_oam(), "OAM test failed");
    // ASSERT_TRUE(test_memory_mapping_prohibited(), "Prohibited addresses test failed");
    // ASSERT_TRUE(test_memory_mapping_io(), "IO Registers test failed");
    // ASSERT_TRUE(test_memory_mapping_hram(), "HRAM test failed");
    // ASSERT_TRUE(test_memory_mapping_ie(), "ie test failed");
    return true;
}

bool test_memory_mapping_rom(void) {
    word test_addr  = 0x0000;
    byte test_value = 0xAA;

    // Force Boot ROM to be mapped
    mmu_testing_get_io_registers()[REG_BANK - ADDR_IO_REGISTERS_START] = 0x00;
    // TODO: Test mapped boot ROM
    // for (test_addr = ADDR_BOOT_ROM_START; test_addr <= ADDR_BOOT_ROM_END; test_addr++) {
    //     // TODO: Add boot ROM read testing

    //     // Ensure no cartridge read takes place
    //     ASSERT_FALSE(
    //         m_mock_read_called,
    //         "Read from Cartridge while in boot ROM region (boot ROM mapped)"
    //     );

    //     // TODO: Add boot ROM write testing

    //     // Ensure no cartridge write takes place
    //     ASSERT_FALSE(
    //         m_mock_write_called,
    //         "Wrote to Cartridge while in boot ROM region (boot ROM mapped)"
    //     );
    // }

    // Test boot ROM boundary with cart ROM
    test_addr = ADDR_BOOT_ROM_END + 1;
    ASSERT_TRUE(assert_cart_read(test_addr, m_fill_pattern), "Expected valid cartridge read");
    ASSERT_TRUE(assert_cart_no_write(test_addr, test_value), "Expected dropped write");

    // Force boot ROM to unmap
    mmu_testing_get_io_registers()[REG_BANK - ADDR_IO_REGISTERS_START] = m_fill_pattern;

    // Test unmapped boot ROM
    test_addr = ADDR_BOOT_ROM_END;
    ASSERT_TRUE(assert_cart_read(test_addr, m_fill_pattern), "Expected valid cartridge read");
    ASSERT_TRUE(assert_cart_no_write(test_addr, test_value), "Expected dropped write");

    // Test ROM boundary
    // (inside)
    test_addr = ADDR_CART_ROM_END;
    ASSERT_TRUE(assert_cart_read(test_addr, m_fill_pattern), "Expected valid cartridge read");
    ASSERT_TRUE(assert_cart_no_write(test_addr, test_value), "Expected dropped write");
    // (outside)
    test_addr = ADDR_CART_ROM_END + 1;
    ASSERT_TRUE(assert_cart_no_read(test_addr, m_fill_pattern), "Expected no cartridge read");
    ASSERT_TRUE(assert_cart_no_write(test_addr, test_value), "Expected no cartrdige write");

    return true;
}

bool test_memory_mapping_vram(void) {
    word  test_addr   = 0x0000;
    byte  test_value  = 0xAA;
    byte  result      = 0x00;
    byte* vram        = mmu_testing_get_vram();
    byte* p_test_byte = NULL;

    // Reset Memory (reseeds everything)
    reset_memory();

    // Test lower boundary
    // (inside)
    test_addr   = ADDR_VRAM_START;
    p_test_byte = vram + 0;
    ASSERT_TRUE(
        assert_bus_read(p_test_byte, test_addr, test_value),
        "Lower boundary fail: Expected read"
    );
    ASSERT_TRUE(
        assert_bus_write(p_test_byte, test_addr, test_value),
        "Lower boundary fail: Expected write"
    );
    // (outside)
    test_addr = ADDR_VRAM_START - 1;
    ASSERT_TRUE(
        assert_bus_no_read(p_test_byte, test_addr, test_value),
        "Lower boundary fail: Expected no read"
    );
    ASSERT_TRUE(
        assert_bus_no_write(p_test_byte, test_addr, test_value),
        "Lower boundary fail: Expected no write"
    );

    // Test upper boundary
    // (inside)
    test_addr   = ADDR_VRAM_END;
    p_test_byte = vram + VRAM_BANK_SIZE - 1;
    ASSERT_TRUE(
        assert_bus_read(p_test_byte, test_addr, test_value),
        "Upper boundary fail: Expected read"
    );
    ASSERT_TRUE(
        assert_bus_write(p_test_byte, test_addr, test_value),
        "Upper boundary fail: Expected write"
    );
    // (outside)
    test_addr = ADDR_VRAM_END + 1;
    ASSERT_TRUE(
        assert_bus_no_read(p_test_byte, test_addr, test_value),
        "Upper boundary fail: Expected no read"
    );
    ASSERT_TRUE(
        assert_bus_no_write(p_test_byte, test_addr, test_value),
        "Upper boundary fail: Expected no write"
    );

    return true;
}

bool test_memory_mapping_cart_ram(void) {
    word test_addr  = 0x0000;
    byte test_value = 0xAA;
    byte result     = 0x00;

    // Reset Memory (reseeds everything)
    reset_memory();

    // Test Lower Boundary
    // (inside)
    test_addr = ADDR_CART_RAM_START;
    ASSERT_TRUE(assert_cart_read(test_addr, test_value), "Expected valid cartridge read");
    ASSERT_TRUE(assert_cart_write(test_addr, test_value), "Expected valid cartridge write");
    // (outside)
    test_addr = ADDR_CART_RAM_START - 1;
    ASSERT_TRUE(assert_cart_no_read(test_addr, test_value), "Expected no cartridge read");
    ASSERT_TRUE(assert_cart_no_write(test_addr, test_value), "Expected no cartridge write");

    // Test Upper Boundary
    // (inside)
    test_addr = ADDR_CART_RAM_END;
    ASSERT_TRUE(assert_cart_read(test_addr, test_value), "Expected valid cartridge read");
    ASSERT_TRUE(assert_cart_write(test_addr, test_value), "Expected valid cartridge write");
    // (outside)
    test_addr = ADDR_CART_RAM_END + 1;
    ASSERT_TRUE(assert_cart_no_read(test_addr, test_value), "Expected no cartridge read");
    ASSERT_TRUE(assert_cart_no_write(test_addr, test_value), "Expected no cartridge write");

    return true;
}

bool test_memory_mapping_wram(void) {
    word  test_addr   = 0x0000;
    byte  test_value  = 0xAA;
    byte  result      = 0x00;
    byte* wram        = mmu_testing_get_wram(0);
    byte* p_test_byte = NULL;

    // Reset Memory (reseeds everything)
    reset_memory();
    // Ensure wram bank register is 0x00
    mmu_testing_get_io_registers()[REG_SVBK - ADDR_IO_REGISTERS_START] = 0x00;

    // Test lower boundary
    // (inside)
    test_addr   = ADDR_WRAM_START;
    p_test_byte = wram + 0;
    ASSERT_TRUE(
        assert_bus_read(p_test_byte, test_addr, test_value),
        "Lower boundary fail: Expected read"
    );
    ASSERT_TRUE(
        assert_bus_write(p_test_byte, test_addr, test_value),
        "Lower boundary fail: Expected write"
    );
    // (outside)
    test_addr = ADDR_WRAM_START - 1;
    ASSERT_TRUE(
        assert_bus_no_read(p_test_byte, test_addr, test_value),
        "Lower boundary fail: Expected no read"
    );
    ASSERT_TRUE(
        assert_bus_no_write(p_test_byte, test_addr, test_value),
        "Lower boundary fail: Expected no write"
    );

    // Test upper boundary
    // (inside)
    wram        = mmu_testing_get_wram(1);
    test_addr   = ADDR_WRAM_END;
    p_test_byte = wram + WRAM_BANK_SIZE - 1;
    ASSERT_TRUE(
        assert_bus_read(p_test_byte, test_addr, test_value),
        "Upper boundary fail: Expected read"
    );
    ASSERT_TRUE(
        assert_bus_write(p_test_byte, test_addr, test_value),
        "Upper boundary fail: Expected write"
    );
    // (outside)
    test_addr = ADDR_WRAM_END + 1;
    ASSERT_TRUE(
        assert_bus_no_read(p_test_byte, test_addr, test_value),
        "Upper boundary fail: Expected no read"
    );
    ASSERT_TRUE(
        assert_bus_no_write(p_test_byte, test_addr, test_value),
        "Upper boundary fail: Expected no write"
    );

    return true;
}

bool test_memory_mapping_echo_ram(void) {
    word  test_addr   = 0x0000;
    byte  test_value  = 0xAA;
    byte* p_test_byte = NULL;

    // Reset Memory (reseeds everything)
    reset_memory();
    // Ensure wram bank register is 0x00 (forces upper bank to bank 1)
    mmu_testing_get_io_registers()[REG_SVBK - ADDR_IO_REGISTERS_START] = 0x00;

    // Test lower boundary
    // (inside)
    test_addr   = ADDR_ECHO_RAM_START;
    p_test_byte = mmu_testing_get_wram(0);
    ASSERT_TRUE(
        assert_bus_read(p_test_byte, test_addr, test_value),
        "Lower boundary fail: Expected read"
    );
    ASSERT_TRUE(
        assert_bus_write(p_test_byte, test_addr, test_value),
        "Lower boundary fail: Expected write"
    );
    // (outside)
    test_addr = ADDR_ECHO_RAM_START - 1;
    ASSERT_TRUE(
        assert_bus_no_read(p_test_byte, test_addr, test_value),
        "Lower boundary fail: Expected no read"
    );
    ASSERT_TRUE(
        assert_bus_no_write(p_test_byte, test_addr, test_value),
        "Lower boundary fail: Expected no write"
    );

    // Test upper boundary
    // (inside)
    p_test_byte =
        mmu_testing_get_wram(1) + (ADDR_ECHO_RAM_END - ADDR_ECHO_RAM_START - WRAM_BANK_SIZE);
    test_addr = ADDR_ECHO_RAM_END;
    ASSERT_TRUE(
        assert_bus_read(p_test_byte, test_addr, test_value),
        "Upper boundary fail: Expected read"
    );
    ASSERT_TRUE(
        assert_bus_write(p_test_byte, test_addr, test_value),
        "Upper boundary fail: Expected write"
    );
    // (outside)
    test_addr = ADDR_ECHO_RAM_END + 1;
    ASSERT_TRUE(
        assert_bus_no_read(p_test_byte, test_addr, test_value),
        "Upper boundary fail: Expected no read"
    );
    ASSERT_TRUE(
        assert_bus_no_write(p_test_byte, test_addr, test_value),
        "Upper boundary fail: Expected no write"
    );

    return true;
}

bool test_memory_mapping_oam(void) {
    word  test_addr   = 0x0000;
    byte  test_value  = 0xAA;
    byte* p_test_byte = NULL;

    // Reset Memory (reseeds everything)
    reset_memory();

    // Test lower boundary
    // (inside)
    test_addr   = ADDR_OAM_START;
    p_test_byte = mmu_testing_get_oam();
    ASSERT_TRUE(
        assert_bus_read(p_test_byte, test_addr, test_value),
        "Lower boundary fail: Expected read"
    );
    ASSERT_TRUE(
        assert_bus_write(p_test_byte, test_addr, test_value),
        "Lower boundary fail: Expected write"
    );
    // (outside)
    test_addr = ADDR_OAM_START - 1;
    ASSERT_TRUE(
        assert_bus_no_read(p_test_byte, test_addr, test_value),
        "Lower boundary fail: Expected no read"
    );
    ASSERT_TRUE(
        assert_bus_no_write(p_test_byte, test_addr, test_value),
        "Lower boundary fail: Expected no write"
    );

    // Test upper boundary
    // (inside)
    p_test_byte += OAM_SIZE - 1;
    test_addr    = ADDR_OAM_END;
    ASSERT_TRUE(
        assert_bus_read(p_test_byte, test_addr, test_value),
        "Upper boundary fail: Expected read"
    );
    ASSERT_TRUE(
        assert_bus_write(p_test_byte, test_addr, test_value),
        "Upper boundary fail: Expected write"
    );
    // (outside)
    test_addr = ADDR_OAM_END + 1;
    ASSERT_TRUE(
        assert_bus_no_read(p_test_byte, test_addr, test_value),
        "Upper boundary fail: Expected no read"
    );
    ASSERT_TRUE(
        assert_bus_no_write(p_test_byte, test_addr, test_value),
        "Upper boundary fail: Expected no write"
    );

    return true;
}

bool assert_bus_read(byte* p_test_byte, const word test_addr, const byte test_value) {
    *p_test_byte      = test_value;
    byte result_value = mmu_read(BUS_PPU, test_addr);
    ASSERT_EQ(
        result_value,
        test_value,
        "Expected value of 0x%.2X but got 0x%.2X",
        test_value,
        result_value
    );
    *p_test_byte = m_fill_pattern;
    return true;
}
bool assert_bus_no_read(byte* p_test_byte, const word test_addr, const byte test_value) {
    *p_test_byte      = test_value;
    byte result_value = mmu_read(BUS_PPU, test_addr);
    ASSERT_NQ(result_value, test_value, "Expected no read");
    *p_test_byte = m_fill_pattern;
    return true;
}
bool assert_bus_write(byte* p_test_byte, const word test_addr, const byte test_value) {
    *p_test_byte = m_fill_pattern;
    mmu_write(BUS_PPU, test_addr, test_value);
    ASSERT_EQ(
        *p_test_byte,
        test_value,
        "Expected value of 0x%.2X but got 0x%.2X",
        test_value,
        *p_test_byte
    );
    *p_test_byte = m_fill_pattern;
    return true;
}
bool assert_bus_no_write(byte* p_test_byte, const word test_addr, const byte test_value) {
    *p_test_byte = m_fill_pattern;
    mmu_write(BUS_PPU, test_addr, test_value);
    ASSERT_NQ(*p_test_byte, test_value, "Expected no write");
    *p_test_byte = m_fill_pattern;
    return true;
}

void reset_mock(void) {
    m_mock_read_called  = false;
    m_mock_write_called = false;
    m_mock_last_addr    = 0x0000;
    m_mock_last_value   = m_fill_pattern;
}
byte hook_cart_read(struct cartridge* cart, const word addr) {
    m_mock_read_called = true;
    m_mock_last_addr   = addr;
    return m_mock_last_value;
}
void hook_cart_write(struct cartridge* cart, const word addr, const byte value) {
    m_mock_write_called = true;
    m_mock_last_addr    = addr;
    m_mock_last_value   = value;
}
bool assert_cart_read(const word addr, const byte expected_value) {
    reset_mock();
    m_mock_last_value = expected_value;
    byte result_value = mmu_read(BUS_PPU, addr);
    ASSERT_TRUE(m_mock_read_called, "Failed to read from Cartridge");
    ASSERT_EQ(
        m_mock_last_addr,
        addr,
        "Expected read address of 0x%.4X but got 0x%.4X",
        addr,
        m_mock_last_addr
    );
    ASSERT_EQ(
        result_value,
        expected_value,
        "Expected read value of 0x%.2X but got 0x%.2X",
        expected_value,
        result_value
    );
    return true;
}
bool assert_cart_no_read(const word addr, const byte expected_value) {
    reset_mock();
    m_mock_last_value = expected_value;
    mmu_read(BUS_PPU, addr);
    ASSERT_FALSE(m_mock_read_called, "Expected failed read from Cartridge");
    return true;
}
bool assert_cart_write(const word addr, const byte value) {
    reset_mock();
    mmu_write(BUS_PPU, addr, value);
    ASSERT_TRUE(m_mock_write_called, "Failed to write to Cartridge");
    ASSERT_EQ(
        m_mock_last_addr,
        addr,
        "Expected write address of 0x%.4X but got 0x%.4X",
        addr,
        m_mock_last_addr
    );
    ASSERT_EQ(
        m_mock_last_value,
        value,
        "Expected write value of 0x%.2X but got 0x%.2X",
        value,
        m_mock_last_value
    );
    return true;
}
bool assert_cart_no_write(const word addr, const byte value) {
    reset_mock();
    mmu_write(BUS_PPU, addr, value);
    ASSERT_FALSE(m_mock_write_called, "Expected failed write to Cartridge");
    return true;
}

void reset_memory() {
    reset_mock();
    set_pattern(mmu_testing_get_vram(), VRAM_BANK_SIZE, m_fill_pattern);

    for (int i = 0; i < WRAM_BANK_COUNT; i++) {
        set_pattern(mmu_testing_get_wram(i), WRAM_BANK_SIZE, m_fill_pattern);
    }

    set_pattern(mmu_testing_get_oam(), OAM_SIZE, m_fill_pattern);
    set_pattern(mmu_testing_get_io_registers(), IO_REGISTERS_SIZE, m_fill_pattern);
    set_pattern(mmu_testing_get_hram(), HRAM_SIZE, m_fill_pattern);
    set_pattern(mmu_testing_get_reg_ie(), 1, m_fill_pattern);
}
void set_pattern(byte* array, const size_t size, const byte fill_value) {
    memset(array, fill_value, size);
}
