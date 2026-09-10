/**
 * @file hw_registers.h
 * @brief Header containing definitions for Hardware Register addresses.
 */
#pragma once

// clang-format off
//*Sourced from: https://gbdev.io/pandocs/Hardware_Reg_List.html
// Address     | Name      | Description                                                       | Readable / Writable | Models
// 0xFF00      | P1/JOYP   | Joypad                                                            | Mixed               | All
// 0xFF01      | SB        | Serial transfer data                                              | R/W                 | All
// 0xFF02      | SC        | Serial transfer control                                           | R/W                 | Mixed
// 0xFF04      | DIV       | Divider register                                                  | R/W                 | All
// 0xFF05      | TIMA      | Timer counter                                                     | R/W                 | All
// 0xFF06      | TMA       | Timer modulo                                                      | R/W                 | All
// 0xFF07      | TAC       | Timer control                                                     | R/W                 | All
// 0xFF0F      | IF        | Interrupt flag                                                    | R/W                 | All
// 0xFF10      | NR10      | Sound channel 1 sweep                                             | R/W                 | All
// 0xFF11      | NR11      | Sound channel 1 length timer & duty cycle                         | Mixed               | All
// 0xFF12      | NR12      | Sound channel 1 volume & envelope                                 | R/W                 | All
// 0xFF13      | NR13      | Sound channel 1 period low                                        | W                   | All
// 0xFF14      | NR14      | Sound channel 1 period high & control                             | Mixed               | All
// 0xFF16      | NR21      | Sound channel 2 length timer & duty cycle                         | Mixed               | All
// 0xFF17      | NR22      | Sound channel 2 volume & envelope                                 | R/W                 | All
// 0xFF18      | NR23      | Sound channel 2 period low                                        | W                   | All
// 0xFF19      | NR24      | Sound channel 2 period high & control                             | Mixed               | All
// 0xFF1A      | NR30      | Sound channel 3 DAC enable                                        | R/W                 | All
// 0xFF1B      | NR31      | Sound channel 3 length timer                                      | W                   | All
// 0xFF1C      | NR32      | Sound channel 3 output level                                      | R/W                 | All
// 0xFF1D      | NR33      | Sound channel 3 period low                                        | W                   | All
// 0xFF1E      | NR34      | Sound channel 3 period high & control                             | Mixed               | All
// 0xFF20      | NR41      | Sound channel 4 length timer                                      | W                   | All
// 0xFF21      | NR42      | Sound channel 4 volume & envelope                                 | R/W                 | All
// 0xFF22      | NR43      | Sound channel 4 frequency & randomness                            | R/W                 | All
// 0xFF23      | NR44      | Sound channel 4 control                                           | Mixed               | All
// 0xFF24      | NR50      | Master volume & VIN panning                                       | R/W                 | All
// 0xFF25      | NR51      | Sound panning                                                     | R/W                 | All
// 0xFF26      | NR52      | Sound on/off                                                      | Mixed               | All
// 0xFF30-FF3F | Wave RAM  | Storage for one of the sound channels’ waveform                   | R/W                 | All
// 0xFF40      | LCDC      | LCD control                                                       | R/W                 | All
// 0xFF41      | STAT      | LCD status                                                        | Mixed               | All
// 0xFF42      | SCY       | Viewport Y position                                               | R/W                 | All
// 0xFF43      | SCX       | Viewport X position                                               | R/W                 | All
// 0xFF44      | LY        | LCD Y coordinate                                                  | R                   | All
// 0xFF45      | LYC       | LY compare                                                        | R/W                 | All
// 0xFF46      | DMA       | OAM DMA source address & start                                    | R/W                 | All
// 0xFF47      | BGP       | BG palette data                                                   | R/W                 | DMG
// 0xFF48      | OBP0      | OBJ palette 0 data                                                | R/W                 | DMG
// 0xFF49      | OBP1      | OBJ palette 1 data                                                | R/W                 | DMG
// 0xFF4A      | WY        | Window Y position                                                 | R/W                 | All
// 0xFF4B      | WX        | Window X position plus 7                                          | R/W                 | All
// 0xFF4C      | KEY0/SYS  | CPU mode select                                                   | Mixed               | CGB
// 0xFF4D      | KEY1/SPD  | Prepare speed switch                                              | Mixed               | CGB
// 0xFF4F      | VBK       | VRAM bank                                                         | R/W                 | CGB
// 0xFF50      | BANK      | Boot ROM mapping control                                          | W                   | All
// 0xFF51      | HDMA1     | VRAM DMA source high                                              | W                   | CGB
// 0xFF52      | HDMA2     | VRAM DMA source low                                               | W                   | CGB
// 0xFF53      | HDMA3     | VRAM DMA destination high                                         | W                   | CGB
// 0xFF54      | HDMA4     | VRAM DMA destination low                                          | W                   | CGB
// 0xFF55      | HDMA5     | VRAM DMA length/mode/start                                        | R/W                 | CGB
// 0xFF56      | RP        | Infrared communications port                                      | Mixed               | CGB
// 0xFF68      | BCPS/BGPI | Background color palette specification / Background palette index | R/W                 | CGB
// 0xFF69      | BCPD/BGPD | Background color palette data / Background palette data           | R/W                 | CGB
// 0xFF6A      | OCPS/OBPI | OBJ color palette specification / OBJ palette index               | R/W                 | CGB
// 0xFF6B      | OCPD/OBPD | OBJ color palette data / OBJ palette data                         | R/W                 | CGB
// 0xFF6C      | OPRI      | Object priority mode                                              | R/W                 | CGB
// 0xFF70      | SVBK/WBK  | WRAM bank                                                         | R/W                 | CGB
// 0xFF76      | PCM12     | Audio digital outputs 1 & 2                                       | R                   | CGB
// 0xFF77      | PCM34     | Audio digital outputs 3 & 4                                       | R                   | CGB
// 0xFFFF      | IE        | Interrupt enable                                                  | R/W                 | All
// clang-format on

#define REG_JOYP 0xFF00
#define REG_SB   0xFF01
#define REG_SC   0xFF02
#define REG_DIV  0xFF04
#define REG_TIMA 0xFF05
#define REG_TMA  0xFF06
#define REG_TAC  0xFF07
#define REG_IF   0xFF0F
#define REG_NR10 0xFF10
#define REG_NR11 0xFF11
#define REG_NR12 0xFF12
#define REG_NR13 0xFF13
#define REG_NR14 0xFF14
#define REG_NR21 0xFF16
#define REG_NR22 0xFF17
#define REG_NR23 0xFF18
#define REG_NR24 0xFF19
#define REG_NR30 0xFF1A
#define REG_NR31 0xFF1B
#define REG_NR32 0xFF1C
#define REG_NR33 0xFF1D
#define REG_NR34 0xFF1E
#define REG_NR41 0xFF20
#define REG_NR42 0xFF21
#define REG_NR43 0xFF22
#define REG_NR44 0xFF23
#define REG_NR50 0xFF24
#define REG_NR51 0xFF25
#define REG_NR52 0xFF26
#define REG_Wave 0xFF30

/** @brief LCD Control Registers. */
#define REG_LCDC                   0xFF40
/** @brief 0b10000000 | (R/W) | LCD & PPU enable. */
#define REG_LCDC_PPU_ENABLE_MASK   0x80
/** @brief 0b01000000 | (R/W) | Window tile map area. */
#define REG_LCDC_WIN_TILE_MAP_MASK 0x40
/** @brief 0b00100000 | (R/W) | Window enable. */
#define REG_LCDC_WIN_ENABLE_MASK   0x20
/** @brief 0b00010000 | (R/W) | BKG & Window tile data area. */
#define REG_LCDC_TILE_DATA_MASK    0x10
/** @brief 0b00001000 | (R/W) | BKG tile map area. */
#define REG_LCDC_BKG_TILE_MAP_MASK 0x08
/** @brief 0b00000100 | (R/W) | Object size. */
#define REG_LCDC_OBJ_SIZE_MASK     0x04
/** @brief 0b00000010 | (R/W) | Object enable. */
#define REG_LCDC_OBJ_ENABLE_MASK   0x02
/** @brief 0b00000001 | (R/W) | BKG & Window enable/priority. */
#define REG_LCDC_PRIORITY_MASK     0x01

/** @brief LCD Status Registers. */
#define REG_STAT                    0xFF41
/** @brief 0b01000000 | (R/W) | LYC int select. */
#define REG_STAT_LYC_INT_SEL_MASK   0x40
/** @brief 0b00100000 | (R/W) | Mode 2 int select. */
#define REG_STAT_MODE2_INT_SEL_MASK 0x20
/** @brief 0b00010000 | (R/W) | Mode 1 int select. */
#define REG_STAT_MODE1_INT_SEL_MASK 0x10
/** @brief 0b00001000 | (R/W) | Mode 0 int select. */
#define REG_STAT_MODE0_INT_SEL_MASK 0x08
/** @brief 0b00000100 | (R)   | LYC == LY condition. */
#define REG_STAT_LYC_LY_EQ_MASK     0x04
/** @brief 0b00000011 | (R)   | PPU Mode. */
#define REG_STAT_MODE_MASK          0x03

#define REG_SCY   0xFF42 /** @brief Viewport Y position. */
#define REG_SCX   0xFF43 /** @brief Viewport X position. */
#define REG_LY    0xFF44 /** @brief LCD Y coordinate. */
#define REG_LYC   0xFF45 /** @brief LY compare. */
#define REG_DMA   0xFF46
#define REG_BGP   0xFF47
#define REG_OBP0  0xFF48
#define REG_OBP1  0xFF49
#define REG_WY    0xFF4A
#define REG_WX    0xFF4B
#define REG_KEY0  0xFF4C
#define REG_KEY1  0xFF4D
#define REG_VBK   0xFF4F
#define REG_BANK  0xFF50
#define REG_HDMA1 0xFF51
#define REG_HDMA2 0xFF52
#define REG_HDMA3 0xFF53
#define REG_HDMA4 0xFF54
#define REG_HDMA5 0xFF55
#define REG_RP    0xFF56
#define REG_BCPS  0xFF68
#define REG_BCPD  0xFF69
#define REG_OCPS  0xFF6A
#define REG_OCPD  0xFF6B
#define REG_OPRI  0xFF6C
#define REG_SVBK  0xFF70
#define REG_PCM12 0xFF76
#define REG_PCM34 0xFF77
#define REG_IE    0xFFFF

static inline const char* hw_reg_to_string(word reg_addr) {
    switch (reg_addr) {
    case REG_JOYP:
        return "JOYP";
    case REG_SB:
        return "SB";
    case REG_SC:
        return "SC";
    case REG_DIV:
        return "DIV";
    case REG_TIMA:
        return "TIMA";
    case REG_TMA:
        return "TMA";
    case REG_TAC:
        return "TAC";
    case REG_IF:
        return "IF";
    case REG_NR10:
        return "NR10";
    case REG_NR11:
        return "NR11";
    case REG_NR12:
        return "NR12";
    case REG_NR13:
        return "NR13";
    case REG_NR14:
        return "NR14";
    case REG_NR21:
        return "NR21";
    case REG_NR22:
        return "NR22";
    case REG_NR23:
        return "NR23";
    case REG_NR24:
        return "NR24";
    case REG_NR30:
        return "NR30";
    case REG_NR31:
        return "NR31";
    case REG_NR32:
        return "NR32";
    case REG_NR33:
        return "NR33";
    case REG_NR34:
        return "NR34";
    case REG_NR41:
        return "NR41";
    case REG_NR42:
        return "NR42";
    case REG_NR43:
        return "NR43";
    case REG_NR44:
        return "NR44";
    case REG_NR50:
        return "NR50";
    case REG_NR51:
        return "NR51";
    case REG_NR52:
        return "NR52";
    case REG_Wave:
        return "Wave";
    case REG_LCDC:
        return "LCDC";
    case REG_STAT:
        return "STAT";
    case REG_SCY:
        return "SCY";
    case REG_SCX:
        return "SCX";
    case REG_LY:
        return "LY";
    case REG_LYC:
        return "LYC";
    case REG_DMA:
        return "DMA";
    case REG_BGP:
        return "BGP";
    case REG_OBP0:
        return "OBP0";
    case REG_OBP1:
        return "OBP1";
    case REG_WY:
        return "WY";
    case REG_WX:
        return "WX";
    case REG_KEY0:
        return "KEY0";
    case REG_KEY1:
        return "KEY1";
    case REG_VBK:
        return "VBK";
    case REG_BANK:
        return "BANK";
    case REG_HDMA1:
        return "HDMA1";
    case REG_HDMA2:
        return "HDMA2";
    case REG_HDMA3:
        return "HDMA3";
    case REG_HDMA4:
        return "HDMA4";
    case REG_HDMA5:
        return "HDMA5";
    case REG_RP:
        return "RP";
    case REG_BCPS:
        return "BCPS";
    case REG_BCPD:
        return "BCPD";
    case REG_OCPS:
        return "OCPS";
    case REG_OCPD:
        return "OCPD";
    case REG_OPRI:
        return "OPRI";
    case REG_SVBK:
        return "SVBK";
    case REG_PCM12:
        return "PCM12";
    case REG_PCM34:
        return "PCM34";
    case REG_IE:
        return "IE";
    default:
        return "unknown";
    }
}
