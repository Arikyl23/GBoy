#pragma once

// ---- CONSTANT DEFINES -----
// These do not change due to compiler settings

#define ORG_NAME "ARI"

// -----

// ----- TESTING DEFINES -----
// These are the flags that change if testing is enabled
#ifdef TESTING
#    define APP_NAME                      "GBoy_TEST"
#    define CONFIG_DEFAULT_CORE_LOG_LEVEL LOG_FATAL
#endif
// -----

// ----- DEFAULT DEFINES ----
// If a define can be changed due to compiler settings, this is where the default values should be
// set

#ifndef APP_NAME
#    define APP_NAME "GBoy"
#endif

#ifndef CONFIG_DEFAULT_CORE_LOG_LEVEL
#    define CONFIG_DEFAULT_CORE_LOG_LEVEL LOG_INFO
#endif

// -----

// ----- CALCULATED DEFINES -----
// If not set explicitly, these are calculated from above defines.

#ifndef CONFIG_MBC_MODULE_LOG_LEVEL
#    define CONFIG_MBC_MODULE_LOG_LEVEL CONFIG_DEFAULT_CORE_LOG_LEVEL
#endif

#ifndef CONFIG_DLD_MODULE_LOG_LEVEL
#    define CONFIG_DLD_MODULE_LOG_LEVEL CONFIG_DEFAULT_CORE_LOG_LEVEL
#endif

#ifndef CONFIG_MBC1_MODULE_LOG_LEVEL
#    define CONFIG_MBC1_MODULE_LOG_LEVEL CONFIG_DEFAULT_CORE_LOG_LEVEL
#endif

#ifndef CONFIG_CARTRIDGE_MODULE_LOG_LEVEL
#    define CONFIG_CARTRIDGE_MODULE_LOG_LEVEL CONFIG_DEFAULT_CORE_LOG_LEVEL
#endif

// -----
