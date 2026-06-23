/**
 * @file macros.h
 * @brief Header containing useful preprocessor macros
 */
#pragma once

// ----- Concatenation -----
#define GB_HIDDEN_CONCAT(a, b)                          a##b
#define GB_CONCAT(a, b)                                 GB_HIDDEN_CONCAT(a, b)
#define GB_CONCAT_WITH_SPACER(a, b)                     GB_CONCAT(a, GB_CONCAT(_, b))
#define GB_UNIQUE_ID(basename)                          GB_CONCAT_WITH_SPACER(basename, __LINE__)
#define GB_UNIQUE_OBJ(basename, obj_name)               GB_UNIQUE_ID(GB_CONCAT_WITH_SPACER(basename, obj_name))
#define GB_HIDDEN_QUALIFIED_DECL(qualifier, type, name) qualifier type name
// -----

// ----- BUFFERS -----
#define GB_BUFFER_DECL(name, size)          GB_HIDDEN_QUALIFIED_DECL(, char, name)[size]
#define GB_STATIC_BUFFER_DECL(name, size)   GB_HIDDEN_QUALIFIED_DECL(static, char, name)[size]
#define GB_BUFFER_DEFINE(name, size)        GB_BUFFER_DECL(name, size) = {0}
#define GB_STATIC_BUFFER_DEFINE(name, size) GB_STATIC_BUFFER_DECL(name, size) = {0}
// -----

#define GB_ARRAY_SIZEOF(arr) (sizeof(arr) / sizeof(arr[0]))
