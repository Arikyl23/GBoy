/**
 * @file assets.h
 * @brief API for handling external asset files like Images/Fonts/etc.
 */
#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif
    struct asset {
        const char* data;
        size_t      size;
    };

    struct asset assets_get_file(const char* path);

#ifdef __cplusplus
}
#endif
