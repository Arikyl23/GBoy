/**
 * @file renderer.c
 * @brief Implementation for the Renderer.
 */
#include "display/renderer.h"

#include <SDL3/SDL.h>
#include <log.h>

LOG_MODULE_SETUP("renderer", CONFIG_RENDERER_MODULE_LOG_LEVEL);

#define RENDERER_MAX_CALLBACKS 5

struct callback_entry {
    bool        enabled;
    renderer_cb cb;
};

struct {
    bool init;

    struct SDL_Mutex*     data_mutex;
    renderer_cb_handle_t  leader;
    struct callback_entry callback_list[RENDERER_MAX_CALLBACKS];
} m_ctx = {
    .init = false,
};

#define LOG_SDL_ERROR(msg)                                                                         \
    do {                                                                                           \
        log_error("SDL Error: " msg "\n\tReason: %s", SDL_GetError());                             \
        SDL_ClearError();                                                                          \
    } while (false);

#define INIT_CHECK()                                                                               \
    do {                                                                                           \
        if (m_ctx.init == false) {                                                                 \
            log_error("Renderer is not initialized yet.");                                         \
            return false;                                                                          \
        }                                                                                          \
    } while (false);

#define HANDLE_CHECK(handle)                                                                       \
    do {                                                                                           \
        if (handle < 0 || handle >= RENDERER_MAX_CALLBACKS) {                                      \
            log_error("Invalid Handle: %i", handle);                                               \
            return false;                                                                          \
        }                                                                                          \
    } while (false);

static int         render_thread_entry_point(void* data);
static inline bool is_active_entry(const struct callback_entry* entry);

bool renderer_init(void) {
    if (m_ctx.init == true) {
        log_warn("Renderer is already initialized");
        return false;
    }

    log_info("Initializing Renderer...");

    m_ctx.data_mutex = SDL_CreateMutex();
    if (m_ctx.data_mutex == NULL) {
        LOG_SDL_ERROR("Failed to create data mutex.");
        goto cleanup;
    }
    m_ctx.leader = 0;
    for (int i = 0; i < RENDERER_MAX_CALLBACKS; i++) {
        m_ctx.callback_list[i] = (struct callback_entry){
            .enabled = false,
            .cb      = NULL,
        };
    }

    m_ctx.init = true;
    log_info("Renderer Initialized");
    return true;

cleanup:
    if (m_ctx.data_mutex != NULL) { SDL_DestroyMutex(m_ctx.data_mutex); }
    // No need to cleanup thread data as it should always be the last thing created
    return false;
}

renderer_cb_handle_t renderer_register_render_cb(renderer_cb cb) {
    INIT_CHECK();

    if (cb == NULL) {
        log_error("No callback given to register");
        return -1;
    }

    SDL_LockMutex(m_ctx.data_mutex);
    for (int i = 0; i < RENDERER_MAX_CALLBACKS; i++) {
        if (m_ctx.callback_list[i].cb == NULL) {
            m_ctx.callback_list[i] = (struct callback_entry){
                .enabled = false,
                .cb      = cb,
            };
            SDL_UnlockMutex(m_ctx.data_mutex);
            log_debug("Registered callback to slot %i", i);
            return i;
        }
    }
    SDL_UnlockMutex(m_ctx.data_mutex);

    log_warn("Failed to register renderer callback. No avalible slots.");
    return -1;
}

bool renderer_set_leader(const renderer_cb_handle_t handle) {
    INIT_CHECK();
    HANDLE_CHECK(handle);

    //* NOTE: Leader is suppose to the the priority window to vsync to. However the API does not
    //*       support actually enabling and disabling vsync.
    //!       VSYNC SHOULD NEVER BE ACTIVE ON MORE THAN ONE RENDERER
    SDL_LockMutex(m_ctx.data_mutex);
    if (m_ctx.callback_list[handle].cb == NULL) {
        SDL_UnlockMutex(m_ctx.data_mutex);
        log_error("Handle not registered to a callback. Handle: %i", handle);
        return false;
    }
    m_ctx.leader = handle;
    SDL_UnlockMutex(m_ctx.data_mutex);

    return true;
}

bool renderer_enable_cb(const renderer_cb_handle_t handle, const bool enable) {
    INIT_CHECK();
    HANDLE_CHECK(handle);

    SDL_LockMutex(m_ctx.data_mutex);
    if (m_ctx.callback_list[handle].cb == NULL) {
        log_warn("Handle not registered. Handle: %i", handle);
    }
    m_ctx.callback_list[handle].enabled = enable;
    SDL_UnlockMutex(m_ctx.data_mutex);
    log_info("%s handle %i", (enable) ? "Enabled" : "Disabled", handle);
    return true;
}

bool renderer_deregister_render_cb(const renderer_cb_handle_t handle) {
    INIT_CHECK();
    HANDLE_CHECK(handle);

    SDL_LockMutex(m_ctx.data_mutex);
    if (m_ctx.callback_list[handle].cb == NULL) {
        log_warn("Handle is already deregistered. Handle: %i", handle);
    } else {
        log_debug("Deregistered handle %i", handle);
        m_ctx.callback_list[handle].cb = NULL;
    }
    SDL_UnlockMutex(m_ctx.data_mutex);

    return true;
}

void renderer_update(void) {
    // Copy the data early to prevent state drift during renderer loop
    // If a callback/other thread modifies internal state, the logic will still hold fine for
    // one loop
    SDL_LockMutex(m_ctx.data_mutex);
    renderer_cb_handle_t  leader_handle = m_ctx.leader;
    struct callback_entry entries[RENDERER_MAX_CALLBACKS];
    SDL_memcpy(entries, m_ctx.callback_list, sizeof(m_ctx.callback_list));
    SDL_UnlockMutex(m_ctx.data_mutex);

    // Dispatch all registered and enabled callbacks
    for (int i = 0; i < RENDERER_MAX_CALLBACKS; i++) {
        if (i == leader_handle) { continue; }
        if (is_active_entry(&entries[i]) == true) { entries[i].cb(); }
    }
    if (is_active_entry(&entries[leader_handle]) == true) { entries[leader_handle].cb(); }
}

void renderer_deinit(void) { }

static inline bool is_active_entry(const struct callback_entry* entry) {
    return entry->cb != NULL && entry->enabled == true;
}
