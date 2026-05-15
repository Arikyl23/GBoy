/**
 * @file assest.cpp
 * @brief Implementation for assets. Provides a CPP->C bridge for the CMake Resource Compiler.
 */
#include "assets.h"

#include <cmrc/cmrc.hpp>

CMRC_DECLARE(GBoy);

extern "C"
{
    struct asset assets_get_file(const char* path) {
        auto fs = cmrc::GBoy::get_filesystem();
        try {
            auto file = fs.open(path);
            return (struct asset){
                .data = file.begin(),
                .size = file.size(),
            };
        } catch (...) {
            return (struct asset){
                .data = nullptr,
                .size = 0,
            };
        }
    }
}
