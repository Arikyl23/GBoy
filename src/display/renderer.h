/**
 * @file renderer.h
 * @brief Public API for the Renderer.
 */
#pragma once

#include <stdbool.h>

typedef int  renderer_cb_handle_t;
typedef void (*renderer_cb)(void);

bool                 renderer_init(void);
renderer_cb_handle_t renderer_register_render_cb(renderer_cb cb);
bool                 renderer_set_leader(const renderer_cb_handle_t handle);
bool                 renderer_enable_cb(const renderer_cb_handle_t handle, const bool enable);
bool                 renderer_deregister_render_cb(const renderer_cb_handle_t handle);
void                 renderer_update(void);
void                 renderer_deinit(void);
