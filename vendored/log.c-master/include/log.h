/**
 * Copyright (c) 2020 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `log.c` for details.
 */

#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#define LOG_VERSION "0.1.0"

typedef struct {
    va_list     ap;
    const char* fmt;
    const char* file;
    const char* module; // Added: Track the module name
    struct tm*  time;
    void*       udata;
    int         line;
    int         level;
} log_Event;

typedef void (*log_LogFn)(log_Event* ev);
typedef void (*log_LockFn)(bool lock, void* udata);

enum {
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
};

#define LOG_MODULE_SETUP_DEFAULT(name)                                                             \
    static const char* log_module_ptr = name;                                                      \
    static int         log_module_lvl = LOG_INFO;

#define LOG_MODULE_SETUP(name, level)                                                              \
    static const char* log_module_ptr = name;                                                      \
    static int         log_module_lvl = level;

#define GET_MODULE_NAME  (log_module_ptr == NULL ? "Global" : log_module_ptr)
#define GET_MODULE_LEVEL log_module_lvl

// NOT ORIGINAL CODE
// =================
// Added the __REL_FILE__ define that is a truncated version of __FILE__.
//  It is truncated to the first directory after the root GBoy.
//
// Each Macro below now takes __REL_FILE__ instead of the full __FILE__.

#define __REL_FILE__ (&__FILE__[PROJECT_SOURCE_DIR_SIZE])

#define log_trace(...)                                                                             \
    do {                                                                                           \
        if (LOG_TRACE >= GET_MODULE_LEVEL)                                                         \
            log_log(LOG_TRACE, __REL_FILE__, GET_MODULE_NAME, __LINE__, __VA_ARGS__);              \
    } while (0)

#define log_debug(...)                                                                             \
    do {                                                                                           \
        if (LOG_DEBUG >= GET_MODULE_LEVEL)                                                         \
            log_log(LOG_DEBUG, __REL_FILE__, GET_MODULE_NAME, __LINE__, __VA_ARGS__);              \
    } while (0)

#define log_info(...)                                                                              \
    do {                                                                                           \
        if (LOG_INFO >= GET_MODULE_LEVEL)                                                          \
            log_log(LOG_INFO, __REL_FILE__, GET_MODULE_NAME, __LINE__, __VA_ARGS__);               \
    } while (0)

#define log_warn(...)                                                                              \
    do {                                                                                           \
        if (LOG_WARN >= GET_MODULE_LEVEL)                                                          \
            log_log(LOG_WARN, __REL_FILE__, GET_MODULE_NAME, __LINE__, __VA_ARGS__);               \
    } while (0)

#define log_error(...)                                                                             \
    do {                                                                                           \
        if (LOG_ERROR >= GET_MODULE_LEVEL)                                                         \
            log_log(LOG_ERROR, __REL_FILE__, GET_MODULE_NAME, __LINE__, __VA_ARGS__);              \
    } while (0)

#define log_fatal(...)                                                                             \
    do {                                                                                           \
        if (LOG_FATAL >= GET_MODULE_LEVEL)                                                         \
            log_log(LOG_FATAL, __REL_FILE__, GET_MODULE_NAME, __LINE__, __VA_ARGS__);              \
    } while (0)

// =================
// NOT ORIGINAL CODE

const char* log_level_string(int level);
void        log_set_lock(log_LockFn fn, void* udata);
void        log_set_level(int level);
void        log_set_quiet(bool enable);
int         log_add_callback(log_LogFn fn, void* udata, int level);
int         log_add_fp(FILE* fp, int level);

void log_log(int level, const char* file, const char* module, int line, const char* fmt, ...);

#endif
