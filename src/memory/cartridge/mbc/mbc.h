/**
 * @file mbc.h
 * @brief Contains the definition of the Memory Bank Controller.
 */
#pragma once

#include <stddef.h>

#include "DEFINES.h"
#include "memory/cartridge/cartridge_type.h"
#include "memory/cartridge/ram_size.h"
#include "memory/cartridge/rom_size.h"

struct mbc;

/**
 * @brief Creates a new MBC.
 * @param cart_code Specifies the type of MBC being created. Should be sourced from ROM header.
 * @param rom_size Specifies the ROM size. Should be sourced from ROM header.
 * @param ram_size Specifies the RAM size. Should be sourced from ROM header.
 * @returns A pointer to the allocated MBC. If allocation fails, pointer will be NULL.
 * @note This function allocates the necessary heap space needed for the MBC.
 */
struct mbc* mbc_create(
    const enum cartridge_type cart_type,
    const enum rom_size       rom_size,
    const enum ram_size       ram_size
);
/**
 * @brief Loads the given data into the MBCs ROM.
 * @param mbc MBC to load data into. Must not be `NULL`.
 * @param data Data to load. Should be a byte array. Must not be `NULL`.
 * @param size Size of data array.
 * @returns An integer indicating success:
 * @returns - 0: Success
 * @returns - 1: Failure
 * @warning This function will overwrite anything currently in the ROM. If the data to load is
 * longer than the size of the ROM, only what fits will be loaded truncating the rest.
 */
int  mbc_load_rom(struct mbc* mbc, const byte* data, const size_t size);
/**
 * @brief Loads the given data into the MBCs RAM.
 * @param mbc MBC to load data into. Must not be `NULL`.
 * @param data Data to load. Should be a byte array. Must not be `NULL`.
 * @param size Size of data array.
 * @returns An integer indicating success:
 * @returns - 0: Success
 * @warning This function will overwrite anything currently in the RAM. If the data to load is
 * longer than the size of the RAM, only what fits will be loaded truncating the rest..
 */
int  mbc_load_ram(struct mbc* mbc, const byte* data, const size_t size);
/**
 * @brief Dumps the MBC's ROM into the provided buffer.
 * @param mbc MBC to dump data from. Must not be `NULL`.
 * @param data [OUT] Buffer the dumped data should be placed into. Must not be `NULL`.
 * @param size Size of data array. The size must be equal to or larger than the MBC's ROM size.
 * @returns An integer indicating success:
 * @returns - 0: Success
 * @returns - 1: Failure
 */
int  mbc_dump_rom(struct mbc* mbc, byte* data, const size_t size);
/**
 * @brief Dumps the MBC's RAM into the provided buffer.
 * @param mbc MBC to dump data from. Must not be `NULL`.
 * @param data [OUT] Buffer the dumped data should be placed into. Must not be `NULL`.
 * @param size Size of data array. The size must be equal to or larger than the MBC's RAM size.
 * @returns An integer indicating success:
 * @returns - 0: Success
 * @returns - 1: Failure
 */
int  mbc_dump_ram(struct mbc* mbc, byte* data, const size_t size);
/**
 * @brief Reads data inside an MBC at the specified address.
 * @param mbc MBC to read from. Must not be `NULL`.
 * @param addr Address to read at.
 * @returns The value at given address inside the MBC.
 * @note If the read is undefined, a value of 0xFF is returned instead.
 */
byte mbc_read(struct mbc* mbc, const word addr);
/**
 * @brief Writes data inside an MBC at the specified address.
 * @param mbc MBC to write to. Must not be `NULL`
 * @param addr Address to write to.
 * @param value Value to write.
 * @note If the write is undefined, the write is ignored.
 */
void mbc_write(struct mbc* mbc, const word addr, const byte value);
/**
 * @brief Cleans up the resources allocated by the MBC.
 * @param p_mbc MBC to cleanup.
 * @note Guranteed to not fail.
 */
void mbc_cleanup(struct mbc** p_mbc);
