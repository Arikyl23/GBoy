/**
 *
 */
#pragma once

// clang-format off
// Code | Licensee
// 00   | None
// 01   | Nintendo
// 08   | Capcom
// 09   | HOT-B
// 0A   | Jaleco
// 0B   | Coconuts Japan
// 0C   | Elite Systems
// 13   | EA (Electronic Arts)
// 18   | Hudson Soft
// 19   | ITC Entertainment
// 1A   | Yanoman
// 1D   | Japan Clary
// 1F   | Virgin Games Ltd.3
// 24   | PCM Complete
// 25   | San-X
// 28   | Kemco
// 29   | SETA Corporation
// 30   | Infogrames5
// 31   | Nintendo
// 32   | Bandai
// 33   | Indicates that the New licensee code should be used instead.
// 34   | Konami
// 35   | HectorSoft
// 38   | Capcom
// 39   | Banpresto
// 3C   | Entertainment Interactive (stub)
// 3E   | Gremlin
// 41   | Ubi Soft1
// 42   | Atlus
// 44   | Malibu Interactive
// 46   | Angel
// 47   | Spectrum HoloByte
// 49   | Irem
// 4A   | Virgin Games Ltd.3
// 4D   | Malibu Interactive
// 4F   | U.S. Gold
// 50   | Absolute
// 51   | Acclaim Entertainment
// 52   | Activision
// 53   | Sammy USA Corporation
// 54   | GameTek
// 55   | Park Place15
// 56   | LJN
// 57   | Matchbox
// 59   | Milton Bradley Company
// 5A   | Mindscape
// 5B   | Romstar
// 5C   | Naxat Soft16
// 5D   | Tradewest
// 60   | Titus Interactive
// 61   | Virgin Games Ltd.3
// 67   | Ocean Software
// 69   | EA (Electronic Arts)
// 6E   | Elite Systems
// 6F   | Electro Brain
// 70   | Infogrames5
// 71   | Interplay Entertainment
// 72   | Broderbund
// 73   | Sculptured Software6
// 75   | The Sales Curve Limited7
// 78   | THQ
// 79   | Accolade8
// 7A   | Triffix Entertainment
// 7C   | MicroProse
// 7F   | Kemco
// 80   | Misawa Entertainment
// 83   | LOZC G.
// 86   | Tokuma Shoten
// 8B   | Bullet-Proof Software2
// 8C   | Vic Tokai Corp.17
// 8E   | Ape Inc.18
// 8F   | I’Max19
// 91   | Chunsoft Co.9
// 92   | Video System
// 93   | Tsubaraya Productions
// 95   | Varie
// 96   | Yonezawa10/S’Pal
// 97   | Kemco
// 99   | Arc
// 9A   | Nihon Bussan
// 9B   | Tecmo
// 9C   | Imagineer
// 9D   | Banpresto
// 9F   | Nova
// A1   | Hori Electric
// A2   | Bandai
// A4   | Konami
// A6   | Kawada
// A7   | Takara
// A9   | Technos Japan
// AA   | Broderbund
// AC   | Toei Animation
// AD   | Toho
// AF   | Namco
// B0   | Acclaim Entertainment
// B1   | ASCII Corporation or Nexsoft
// B2   | Bandai
// B4   | Square Enix
// B6   | HAL Laboratory
// B7   | SNK
// B9   | Pony Canyon
// BA   | Culture Brain
// BB   | Sunsoft
// BD   | Sony Imagesoft
// BF   | Sammy Corporation
// C0   | Taito
// C2   | Kemco
// C3   | Square
// C4   | Tokuma Shoten
// C5   | Data East
// C6   | Tonkin House
// C8   | Koei
// C9   | UFL
// CA   | Ultra Games
// CB   | VAP, Inc.
// CC   | Use Corporation
// CD   | Meldac
// CE   | Pony Canyon
// CF   | Angel
// D0   | Taito
// D1   | SOFEL (Software Engineering Lab)
// D2   | Quest
// D3   | Sigma Enterprises
// D4   | ASK Kodansha Co.
// D6   | Naxat Soft16
// D7   | Copya System
// D9   | Banpresto
// DA   | Tomy
// DB   | LJN
// DD   | Nippon Computer Systems
// DE   | Human Ent.
// DF   | Altron
// E0   | Jaleco
// E1   | Towa Chiki
// E2   | Yutaka # Needs more info
// E3   | Varie
// E5   | Epoch
// E7   | Athena
// E8   | Asmik Ace Entertainment
// E9   | Natsume
// EA   | King Records
// EB   | Atlus
// EC   | Epic/Sony Records
// EE   | IGS
// F0   | A Wave
// F3   | Extreme Entertainment
// FF   | LJN
// clang-format on

enum old_licensee_code {
    OLD_LI_CODE_NONE                      = 0x00,
    OLD_LI_CODE_NINTENDO_1                = 0x01,
    OLD_LI_CODE_CAPCOM_1                  = 0x08,
    OLD_LI_CODE_HOT_B                     = 0x09,
    OLD_LI_CODE_JALECO_1                  = 0x0A,
    OLD_LI_CODE_COCONUTS_JAPAN            = 0x0B,
    OLD_LI_CODE_ELITE_SYSTEMS_1           = 0x0C,
    OLD_LI_CODE_EA_2                      = 0x13,
    OLD_LI_CODE_HUDSON_SOFT               = 0x18,
    OLD_LI_CODE_ITC_ENTERTAINMENT         = 0x19,
    OLD_LI_CODE_YANOMAN                   = 0x1A,
    OLD_LI_CODE_JAPAN_CLARY               = 0x1D,
    OLD_LI_CODE_VIRGIN_GAMES_LTD_1        = 0x1F,
    OLD_LI_CODE_PCM_COMPLETE              = 0x24,
    OLD_LI_CODE_SAN_X                     = 0x25,
    OLD_LI_CODE_KEMCO_1                   = 0x28,
    OLD_LI_CODE_SETA_CORPORATION          = 0x29,
    OLD_LI_CODE_INFOGRAMES_1              = 0x30,
    OLD_LI_CODE_NINTENDO_2                = 0x31,
    OLD_LI_CODE_BANDAI_1                  = 0x32,
    OLD_LI_CODE_NEW_LI_CODE               = 0x33,
    OLD_LI_CODE_KONAMI_1                  = 0x34,
    OLD_LI_CODE_HECTORSOFT                = 0x35,
    OLD_LI_CODE_CAPCOM_2                  = 0x38,
    OLD_LI_CODE_BANPRESTO_1               = 0x39,
    OLD_LI_CODE_ENTERTAINMENT_INTERACTIVE = 0x3C,
    OLD_LI_CODE_GREMLIN                   = 0x3E,
    OLD_LI_CODE_UBISOFT                   = 0x41,
    OLD_LI_CODE_ATLUS_1                   = 0x42,
    OLD_LI_CODE_MALIBU_INTERACTIVE_1      = 0x44,
    OLD_LI_CODE_ANGEL_1                   = 0x46,
    OLD_LI_CODE_SPECTRUM_HOLOBYTE         = 0x47,
    OLD_LI_CODE_IREM                      = 0x49,
    OLD_LI_CODE_VIRGIN_GAMES_LTD_2        = 0x4A,
    OLD_LI_CODE_MALIBU_INTERACTIVE_2      = 0x4D,
    OLD_LI_CODE_US_GOLD                   = 0x4F,
    OLD_LI_CODE_ABSOLUTE                  = 0x50,
    OLD_LI_CODE_ACCLAIM_ENTERTAINMENT_1   = 0x51,
    OLD_LI_CODE_ACTIVISION                = 0x52,
    OLD_LI_CODE_SAMMY_USA_CORPORATION     = 0x53,
    OLD_LI_CODE_GAMETEK                   = 0x54,
    OLD_LI_CODE_PARK_PLACE                = 0x55,
    OLD_LI_CODE_LJN_1                     = 0x56,
    OLD_LI_CODE_MATCHBOX                  = 0x57,
    OLD_LI_CODE_MILTON_BRADLEY_COMPANY    = 0x59,
    OLD_LI_CODE_MINDSCAPE                 = 0x5A,
    OLD_LI_CODE_ROMSTAR                   = 0x5B,
    OLD_LI_CODE_NAXAT_SOFT_1              = 0x5C,
    OLD_LI_CODE_TRADEWEST                 = 0x5D,
    OLD_LI_CODE_TITUS_INTERACTIVE         = 0x60,
    OLD_LI_CODE_VIRGIN_GAMES_LTD_3        = 0x61,
    OLD_LI_CODE_OCEAN_SOFTWARE            = 0x67,
    OLD_LI_CODE_EA_1                      = 0x69,
    OLD_LI_CODE_ELITE_SYSTEMS_2           = 0x6E,
    OLD_LI_CODE_ELECTRO_BRAIN             = 0x6F,
    OLD_LI_CODE_INFOGRAMES_2              = 0x70,
    OLD_LI_CODE_INTERPLAY_ENTERTAINMENT   = 0x71,
    OLD_LI_CODE_BRODERBUND_1              = 0x72,
    OLD_LI_CODE_SCULPTURED_SOFTWARE       = 0x73,
    OLD_LI_CODE_THE_SALES_CURVE_LIMITED   = 0x75,
    OLD_LI_CODE_THQ                       = 0x78,
    OLD_LI_CODE_ACCOLADE                  = 0x79,
    OLD_LI_CODE_TRIFFIX_ENTERTAINMENT     = 0x7A,
    OLD_LI_CODE_MICROPROSE                = 0x7C,
    OLD_LI_CODE_KEMCO_2                   = 0x7F,
    OLD_LI_CODE_MISAWA_ENTERTAINMENT      = 0x80,
    OLD_LI_CODE_LOZC_G                    = 0x83,
    OLD_LI_CODE_TOKUMA_SHOTEN_1           = 0x86,
    OLD_LI_CODE_BULLET_PROOF_SOFTWARE     = 0x8B,
    OLD_LI_CODE_VIC_TOKAI_CORP            = 0x8C,
    OLD_LI_CODE_APE_INC                   = 0x8E,
    OLD_LI_CODE_I_MAX                     = 0x8F,
    OLD_LI_CODE_CHUNSOFT_CO               = 0x91,
    OLD_LI_CODE_VIDEO_SYSTEM              = 0x92,
    OLD_LI_CODE_TSUBARAYA_PRODUCTIONS     = 0x93,
    OLD_LI_CODE_VARIE_1                   = 0x95,
    OLD_LI_CODE_YONEZAWA                  = 0x96,
    OLD_LI_CODE_KEMCO_3                   = 0x97,
    OLD_LI_CODE_ARC                       = 0x99,
    OLD_LI_CODE_NIHON_BUSSAN              = 0x9A,
    OLD_LI_CODE_TECMO                     = 0x9B,
    OLD_LI_CODE_IMAGINEER                 = 0x9C,
    OLD_LI_CODE_BANPRESTO_2               = 0x9D,
    OLD_LI_CODE_NOVA                      = 0x9F,
    OLD_LI_CODE_HORI_ELECTRIC             = 0xA1,
    OLD_LI_CODE_BANDAI_2                  = 0xA2,
    OLD_LI_CODE_KONAMI_2                  = 0xA4,
    OLD_LI_CODE_KAWADA                    = 0xA6,
    OLD_LI_CODE_TAKARA                    = 0xA7,
    OLD_LI_CODE_TECHNOS_JAPAN             = 0xA9,
    OLD_LI_CODE_BRODERBUND_2              = 0xAA,
    OLD_LI_CODE_TOEI_ANIMATION            = 0xAC,
    OLD_LI_CODE_TOHO                      = 0xAD,
    OLD_LI_CODE_NAMCO                     = 0xAF,
    OLD_LI_CODE_ACCLAIM_ENTERTAINMENT_2   = 0xB0,
    OLD_LI_CODE_ASCII_CORPORATION         = 0xB1,
    OLD_LI_CODE_BANDAI_3                  = 0xB2,
    OLD_LI_CODE_SQUARE_ENIX               = 0xB4,
    OLD_LI_CODE_HAL_LABORATORY            = 0xB6,
    OLD_LI_CODE_SNK                       = 0xB7,
    OLD_LI_CODE_PONY_CANYON_1             = 0xB9,
    OLD_LI_CODE_CULTURE_BRAIN             = 0xBA,
    OLD_LI_CODE_SUNSOFT                   = 0xBB,
    OLD_LI_CODE_SONY_IMAGESOFT            = 0xBD,
    OLD_LI_CODE_SAMMY_CORPORATION         = 0xBF,
    OLD_LI_CODE_TAITO_1                   = 0xC0,
    OLD_LI_CODE_KEMCO_4                   = 0xC2,
    OLD_LI_CODE_SQUARE                    = 0xC3,
    OLD_LI_CODE_TOKUMA_SHOTEN_2           = 0xC4,
    OLD_LI_CODE_DATA_EAST                 = 0xC5,
    OLD_LI_CODE_TONKIN_HOUSE              = 0xC6,
    OLD_LI_CODE_KOEI                      = 0xC8,
    OLD_LI_CODE_UFL                       = 0xC9,
    OLD_LI_CODE_ULTRA_GAMES               = 0xCA,
    OLD_LI_CODE_VAP_INC                   = 0xCB,
    OLD_LI_CODE_USE_CORPORATION           = 0xCC,
    OLD_LI_CODE_MELDAC                    = 0xCD,
    OLD_LI_CODE_PONY_CANYON_2             = 0xCE,
    OLD_LI_CODE_ANGEL_2                   = 0xCF,
    OLD_LI_CODE_TAITO_2                   = 0xD0,
    OLD_LI_CODE_SOFEL                     = 0xD1,
    OLD_LI_CODE_QUEST                     = 0xD2,
    OLD_LI_CODE_SIGMA_ENTERPRISES         = 0xD3,
    OLD_LI_CODE_ASK_KODANSHA_CO           = 0xD4,
    OLD_LI_CODE_NAXAT_SOFT_2              = 0xD6,
    OLD_LI_CODE_COPYA_SYSTEM              = 0xD7,
    OLD_LI_CODE_BANPRESTO_3               = 0xD9,
    OLD_LI_CODE_TOMY                      = 0xDA,
    OLD_LI_CODE_LJN_2                     = 0xDB,
    OLD_LI_CODE_NIPPON_COMPUTER_SYSTEMS   = 0xDD,
    OLD_LI_CODE_HUMAN_ENT                 = 0xDE,
    OLD_LI_CODE_ALTRON                    = 0xDF,
    OLD_LI_CODE_JALECO_2                  = 0xE0,
    OLD_LI_CODE_TOWA_CHIKI                = 0xE1,
    OLD_LI_CODE_YUTAKA                    = 0xE2,
    OLD_LI_CODE_VARIE_2                   = 0xE3,
    OLD_LI_CODE_EPOCH                     = 0xE5,
    OLD_LI_CODE_ATHENA                    = 0xE7,
    OLD_LI_CODE_ASMIK_ACE_ENTERTAINMENT   = 0xE8,
    OLD_LI_CODE_NATSUME                   = 0xE9,
    OLD_LI_CODE_KING_RECORDS              = 0xEA,
    OLD_LI_CODE_ATLUS_2                   = 0xEB,
    OLD_LI_CODE_SONY_RECORDS              = 0xEC,
    OLD_LI_CODE_IGS                       = 0xEE,
    OLD_LI_CODE_A_WAVE                    = 0xF0,
    OLD_LI_CODE_EXTREME_ENTERTAINMENT     = 0xF3,
    OLD_LI_CODE_LJN_3                     = 0xFF,
};

static inline const char* old_licensee_code_to_string(const enum old_licensee_code code) {
    switch (code) {
    case OLD_LI_CODE_NONE:
        return "NONE";
    case OLD_LI_CODE_NINTENDO_1:
    case OLD_LI_CODE_NINTENDO_2:
        return "Nintendo";
    case OLD_LI_CODE_CAPCOM_1:
    case OLD_LI_CODE_CAPCOM_2:
        return "Capcom";
    case OLD_LI_CODE_HOT_B:
        return "HOT-B";
    case OLD_LI_CODE_JALECO_1:
    case OLD_LI_CODE_JALECO_2:
        return "Jaleco";
    case OLD_LI_CODE_COCONUTS_JAPAN:
        return "Coconuts Japan";
    case OLD_LI_CODE_ELITE_SYSTEMS_1:
    case OLD_LI_CODE_ELITE_SYSTEMS_2:
        return "Elite Systems";
    case OLD_LI_CODE_EA_1:
    case OLD_LI_CODE_EA_2:
        return "EA (Electronic Arts)";
    case OLD_LI_CODE_HUDSON_SOFT:
        return "Hudson Soft";
    case OLD_LI_CODE_ITC_ENTERTAINMENT:
        return "ITC Entertainment";
    case OLD_LI_CODE_YANOMAN:
        return "Yanoman";
    case OLD_LI_CODE_JAPAN_CLARY:
        return "Japan Clary";
    case OLD_LI_CODE_VIRGIN_GAMES_LTD_1:
    case OLD_LI_CODE_VIRGIN_GAMES_LTD_2:
    case OLD_LI_CODE_VIRGIN_GAMES_LTD_3:
        return "Virgin Games Ltd.";
    case OLD_LI_CODE_PCM_COMPLETE:
        return "PCM Complete";
    case OLD_LI_CODE_SAN_X:
        return "San-X";
    case OLD_LI_CODE_KEMCO_1:
    case OLD_LI_CODE_KEMCO_2:
    case OLD_LI_CODE_KEMCO_3:
    case OLD_LI_CODE_KEMCO_4:
        return "Kemco";
    case OLD_LI_CODE_SETA_CORPORATION:
        return "SETA Corporation";
    case OLD_LI_CODE_INFOGRAMES_1:
    case OLD_LI_CODE_INFOGRAMES_2:
        return "Infogrames";
    case OLD_LI_CODE_BANDAI_1:
    case OLD_LI_CODE_BANDAI_2:
    case OLD_LI_CODE_BANDAI_3:
        return "Bandai";
    case OLD_LI_CODE_NEW_LI_CODE:
        return "NEW CODE";
    case OLD_LI_CODE_KONAMI_1:
    case OLD_LI_CODE_KONAMI_2:
        return "Konami";
    case OLD_LI_CODE_HECTORSOFT:
        return "HectorSoft";
    case OLD_LI_CODE_BANPRESTO_1:
    case OLD_LI_CODE_BANPRESTO_2:
    case OLD_LI_CODE_BANPRESTO_3:
        return "Banpresto";
    case OLD_LI_CODE_ENTERTAINMENT_INTERACTIVE:
        return "Entertainment Interactive";
    case OLD_LI_CODE_GREMLIN:
        return "Gremlin";
    case OLD_LI_CODE_UBISOFT:
        return "Ubi Soft";
    case OLD_LI_CODE_ATLUS_1:
    case OLD_LI_CODE_ATLUS_2:
        return "Atlus";
    case OLD_LI_CODE_MALIBU_INTERACTIVE_1:
    case OLD_LI_CODE_MALIBU_INTERACTIVE_2:
        return "Malibu Interactive";
    case OLD_LI_CODE_ANGEL_1:
    case OLD_LI_CODE_ANGEL_2:
        return "Angel";
    case OLD_LI_CODE_SPECTRUM_HOLOBYTE:
        return "Spectrum HoloByte";
    case OLD_LI_CODE_IREM:
        return "Irem";
    case OLD_LI_CODE_US_GOLD:
        return "U.S. Gold";
    case OLD_LI_CODE_ABSOLUTE:
        return "Absolute";
    case OLD_LI_CODE_ACCLAIM_ENTERTAINMENT_1:
    case OLD_LI_CODE_ACCLAIM_ENTERTAINMENT_2:
        return "Acclaim Entertainment";
    case OLD_LI_CODE_ACTIVISION:
        return "Activision";
    case OLD_LI_CODE_SAMMY_USA_CORPORATION:
        return "Sammy USA Corporation";
    case OLD_LI_CODE_GAMETEK:
        return "GameTek";
    case OLD_LI_CODE_PARK_PLACE:
        return "Park Place";
    case OLD_LI_CODE_LJN_1:
    case OLD_LI_CODE_LJN_2:
    case OLD_LI_CODE_LJN_3:
        return "LJN";
    case OLD_LI_CODE_MATCHBOX:
        return "Matchbox";
    case OLD_LI_CODE_MILTON_BRADLEY_COMPANY:
        return "Milton Bradley Company";
    case OLD_LI_CODE_MINDSCAPE:
        return "Mindscape";
    case OLD_LI_CODE_ROMSTAR:
        return "Romstar";
    case OLD_LI_CODE_NAXAT_SOFT_1:
    case OLD_LI_CODE_NAXAT_SOFT_2:
        return "Naxat Soft";
    case OLD_LI_CODE_TRADEWEST:
        return "Tradewest";
    case OLD_LI_CODE_TITUS_INTERACTIVE:
        return "Titus Interactive";
    case OLD_LI_CODE_OCEAN_SOFTWARE:
        return "Ocean Software";
    case OLD_LI_CODE_ELECTRO_BRAIN:
        return "Electro Brain";
    case OLD_LI_CODE_INTERPLAY_ENTERTAINMENT:
        return "Interplay Entertainment";
    case OLD_LI_CODE_BRODERBUND_1:
    case OLD_LI_CODE_BRODERBUND_2:
        return "Broderbund";
    case OLD_LI_CODE_SCULPTURED_SOFTWARE:
        return "Sculptured Software";
    case OLD_LI_CODE_THE_SALES_CURVE_LIMITED:
        return "The Sales Curve Limited";
    case OLD_LI_CODE_THQ:
        return "THQ";
    case OLD_LI_CODE_ACCOLADE:
        return "Accolade";
    case OLD_LI_CODE_TRIFFIX_ENTERTAINMENT:
        return "Triffix Entertainment";
    case OLD_LI_CODE_MICROPROSE:
        return "MicroProse";
    case OLD_LI_CODE_MISAWA_ENTERTAINMENT:
        return "Misawa Entertainment";
    case OLD_LI_CODE_LOZC_G:
        return "LOZC G.";
    case OLD_LI_CODE_TOKUMA_SHOTEN_1:
    case OLD_LI_CODE_TOKUMA_SHOTEN_2:
        return "Tokuma Shoten";
    case OLD_LI_CODE_BULLET_PROOF_SOFTWARE:
        return "Bullet-Proof Software";
    case OLD_LI_CODE_VIC_TOKAI_CORP:
        return "Vic Tokai Corp.";
    case OLD_LI_CODE_APE_INC:
        return "Ape Inc.";
    case OLD_LI_CODE_I_MAX:
        return "I'Max";
    case OLD_LI_CODE_CHUNSOFT_CO:
        return "Chunsoft Co.";
    case OLD_LI_CODE_VIDEO_SYSTEM:
        return "Video System";
    case OLD_LI_CODE_TSUBARAYA_PRODUCTIONS:
        return "Tsubaraya Productions";
    case OLD_LI_CODE_VARIE_1:
    case OLD_LI_CODE_VARIE_2:
        return "Varie";
    case OLD_LI_CODE_YONEZAWA:
        return "Yonezawa/S'Pal";
    case OLD_LI_CODE_ARC:
        return "Arc";
    case OLD_LI_CODE_NIHON_BUSSAN:
        return "Nihon Bussan";
    case OLD_LI_CODE_TECMO:
        return "Tecmo";
    case OLD_LI_CODE_IMAGINEER:
        return "Imagineer";
    case OLD_LI_CODE_NOVA:
        return "Nova";
    case OLD_LI_CODE_HORI_ELECTRIC:
        return "Hori Electric";
    case OLD_LI_CODE_KAWADA:
        return "Kawada";
    case OLD_LI_CODE_TAKARA:
        return "Takara";
    case OLD_LI_CODE_TECHNOS_JAPAN:
        return "Technos Japan";
    case OLD_LI_CODE_TOEI_ANIMATION:
        return "Toei Animation";
    case OLD_LI_CODE_TOHO:
        return "Toho";
    case OLD_LI_CODE_NAMCO:
        return "Namco";
    case OLD_LI_CODE_ASCII_CORPORATION:
        return "ASCII Corporation or Nexsoft";
    case OLD_LI_CODE_SQUARE_ENIX:
        return "Square Enix";
    case OLD_LI_CODE_HAL_LABORATORY:
        return "HAL Laboratory";
    case OLD_LI_CODE_SNK:
        return "SNK";
    case OLD_LI_CODE_PONY_CANYON_1:
    case OLD_LI_CODE_PONY_CANYON_2:
        return "Pony Canyon";
    case OLD_LI_CODE_CULTURE_BRAIN:
        return "Culture Brain";
    case OLD_LI_CODE_SUNSOFT:
        return "Sunsoft";
    case OLD_LI_CODE_SONY_IMAGESOFT:
        return "Sony Imagesoft";
    case OLD_LI_CODE_SAMMY_CORPORATION:
        return "Sammy Corporation";
    case OLD_LI_CODE_TAITO_1:
    case OLD_LI_CODE_TAITO_2:
        return "Taito";
    case OLD_LI_CODE_SQUARE:
        return "Square";
    case OLD_LI_CODE_DATA_EAST:
        return "Data East";
    case OLD_LI_CODE_TONKIN_HOUSE:
        return "Tonkin House";
    case OLD_LI_CODE_KOEI:
        return "Koei";
    case OLD_LI_CODE_UFL:
        return "UFL";
    case OLD_LI_CODE_ULTRA_GAMES:
        return "Ultra Games";
    case OLD_LI_CODE_VAP_INC:
        return "VAP, Inc.";
    case OLD_LI_CODE_USE_CORPORATION:
        return "Use Corporation";
    case OLD_LI_CODE_MELDAC:
        return "Meldac";
    case OLD_LI_CODE_SOFEL:
        return "SOFEL (Software Engineering Lab)";
    case OLD_LI_CODE_QUEST:
        return "Quest";
    case OLD_LI_CODE_SIGMA_ENTERPRISES:
        return "Sigma Enterprises";
    case OLD_LI_CODE_ASK_KODANSHA_CO:
        return "ASK Kodansha Co.";
    case OLD_LI_CODE_COPYA_SYSTEM:
        return "Copya System";
    case OLD_LI_CODE_TOMY:
        return "Tomy";
    case OLD_LI_CODE_NIPPON_COMPUTER_SYSTEMS:
        return "Nippon Computer Systems";
    case OLD_LI_CODE_HUMAN_ENT:
        return "Human Ent.";
    case OLD_LI_CODE_ALTRON:
        return "Altron";
    case OLD_LI_CODE_TOWA_CHIKI:
        return "Towa Chiki";
    case OLD_LI_CODE_YUTAKA:
        return "Yutaka";
    case OLD_LI_CODE_EPOCH:
        return "Epoch";
    case OLD_LI_CODE_ATHENA:
        return "Athena";
    case OLD_LI_CODE_ASMIK_ACE_ENTERTAINMENT:
        return "Asmik Ace Entertainment";
    case OLD_LI_CODE_NATSUME:
        return "Natsume";
    case OLD_LI_CODE_KING_RECORDS:
        return "King Records";
    case OLD_LI_CODE_SONY_RECORDS:
        return "Epic/Sony Records";
    case OLD_LI_CODE_IGS:
        return "IGS";
    case OLD_LI_CODE_A_WAVE:
        return "A Wave";
    case OLD_LI_CODE_EXTREME_ENTERTAINMENT:
        return "Extreme Entertainment";
    }
}
