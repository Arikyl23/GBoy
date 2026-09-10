#include <log.h>
#include <stdbool.h>

#include "display/renderer.h"
#include "events/event.h"
#include "gboy.h"

LOG_MODULE_SETUP("main", LOG_DEBUG);

#define ROM_FILEPATH "01-special"

static bool m_active = true;

static void application_event_handler(const struct event* evt) {
    switch (evt->type) {
    case EVENT_APPLICATION:
        switch (evt->application.type) {
        case EVENT_APPLICATION_QUIT:
            log_debug("Application Quit Reqested");
            m_active = false;
            return;
        case EVENT_APPLICATION_TERMINATING:
            log_debug("Application Terminating");
            m_active = false;
            return;
        case EVENT_APPLICATION_LOW_MEMORY:
            log_warn("Application has LOW MEMORY");
            return;
        default:
            log_error("Unhandled Application Event");
            return;
        };
    default:
        log_error("Only application events should be thrown here");
        return;
    };
}

int main(void) {
    if (renderer_init() == false) {
        log_error("Failed to initialize the renderer");
        goto err;
    }

    // Setup event handlers
    event_register_application_event_handler(application_event_handler);

    // Enable GBoy
    if (gboy_init() == false) { goto err; }
    if (gboy_load_rom(ROM_FILEPATH) == false) { goto err; }
    if (gboy_poweron(0) == false) { goto err; }

    // Just loop for now
    while (m_active == true) {
        event_poll();
        renderer_update();
    }

    gboy_cleanup();
    return 0;

err:
    log_error("Critical Error occured! Stopping Process.");
    gboy_cleanup();
    return -1;
}
