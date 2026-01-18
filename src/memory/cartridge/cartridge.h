/**
 * @file cartridge.h
 */
#pragma once

#include "memory/cartridge/cartridge_type.h"
#include "memory/cartridge/mbc/mbc.h"
#include "memory/cartridge/old_licensee_code.h"
#include "memory/cartridge/ram_size.h"
#include "memory/cartridge/rom_size.h"

struct header {
    byte                   logo[16 * 3];
    char                   title[17];
    byte                   manufacturer_code[4];
    byte                   cbg_flag;
    char                   new_licensee_code[2];
    byte                   sgb_flag;
    enum cartridge_type    cartridge_type;
    enum rom_size          rom_size;
    enum ram_size          ram_size;
    byte                   destination_code;
    enum old_licensee_code old_licensee_code;
    byte                   mask_rom_version;
    byte                   header_checksum;
    byte                   global_checksum[2];
};

struct cartridge {
    struct header header;
    struct mbc*   mbc;
};
