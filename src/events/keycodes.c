#include "events/keycodes.h"

#include <SDL3/SDL.h>

#include "events/keycodes_internal.h"

uint32_t keycode_from_sdl_scancode(const SDL_Keycode code) {
    switch (code) {
    case SDLK_UNKNOWN:
        return KEYCODE_NONE;
    case SDL_SCANCODE_A:
        return KEYCODE_A;
    case SDL_SCANCODE_B:
        return KEYCODE_B;
    case SDL_SCANCODE_C:
        return KEYCODE_C;
    case SDL_SCANCODE_D:
        return KEYCODE_D;
    case SDL_SCANCODE_E:
        return KEYCODE_E;
    case SDL_SCANCODE_F:
        return KEYCODE_F;
    case SDL_SCANCODE_G:
        return KEYCODE_G;
    case SDL_SCANCODE_H:
        return KEYCODE_H;
    case SDL_SCANCODE_I:
        return KEYCODE_I;
    case SDL_SCANCODE_J:
        return KEYCODE_J;
    case SDL_SCANCODE_K:
        return KEYCODE_K;
    case SDL_SCANCODE_L:
        return KEYCODE_L;
    case SDL_SCANCODE_M:
        return KEYCODE_M;
    case SDL_SCANCODE_N:
        return KEYCODE_N;
    case SDL_SCANCODE_O:
        return KEYCODE_O;
    case SDL_SCANCODE_P:
        return KEYCODE_P;
    case SDL_SCANCODE_Q:
        return KEYCODE_Q;
    case SDL_SCANCODE_R:
        return KEYCODE_R;
    case SDL_SCANCODE_S:
        return KEYCODE_S;
    case SDL_SCANCODE_T:
        return KEYCODE_T;
    case SDL_SCANCODE_U:
        return KEYCODE_U;
    case SDL_SCANCODE_V:
        return KEYCODE_V;
    case SDL_SCANCODE_W:
        return KEYCODE_W;
    case SDL_SCANCODE_X:
        return KEYCODE_X;
    case SDL_SCANCODE_Y:
        return KEYCODE_Y;
    case SDL_SCANCODE_Z:
        return KEYCODE_Z;

    case SDL_SCANCODE_0:
        return KEYCODE_0;
    case SDL_SCANCODE_1:
        return KEYCODE_1;
    case SDL_SCANCODE_2:
        return KEYCODE_2;
    case SDL_SCANCODE_3:
        return KEYCODE_3;
    case SDL_SCANCODE_4:
        return KEYCODE_4;
    case SDL_SCANCODE_5:
        return KEYCODE_5;
    case SDL_SCANCODE_6:
        return KEYCODE_6;
    case SDL_SCANCODE_7:
        return KEYCODE_7;
    case SDL_SCANCODE_8:
        return KEYCODE_8;
    case SDL_SCANCODE_9:
        return KEYCODE_9;
    case SDL_SCANCODE_MINUS:
        return KEYCODE_MINUS;
    case SDL_SCANCODE_EQUALS:
        return KEYCODE_EQUALS;

    case SDL_SCANCODE_CAPSLOCK:
        return KEYCODE_CAPSLOCK;
    case SDL_SCANCODE_SPACE:
        return KEYCODE_SPACE;
    case SDL_SCANCODE_TAB:
        return KEYCODE_TAB;
    case SDL_SCANCODE_RETURN:
        return KEYCODE_ENTER;
    case SDL_SCANCODE_ESCAPE:
        return KEYCODE_ESCAPE;
    case SDL_SCANCODE_BACKSPACE:
        return KEYCODE_BACKSPACE;

    case SDL_SCANCODE_SCROLLLOCK:
        return KEYCODE_SCROLLLOCK;
    case SDL_SCANCODE_DELETE:
        return KEYCODE_DELETE;
    case SDL_SCANCODE_INSERT:
        return KEYCODE_INSERT;
    case SDL_SCANCODE_HOME:
        return KEYCODE_HOME;
    case SDL_SCANCODE_END:
        return KEYCODE_END;
    case SDL_SCANCODE_PAGEUP:
        return KEYCODE_PAGEUP;
    case SDL_SCANCODE_PAGEDOWN:
        return KEYCODE_PAGEDOWN;
    case SDL_SCANCODE_UP:
        return KEYCODE_UP;
    case SDL_SCANCODE_DOWN:
        return KEYCODE_DOWN;
    case SDL_SCANCODE_LEFT:
        return KEYCODE_LEFT;
    case SDL_SCANCODE_RIGHT:
        return KEYCODE_RIGHT;

    case SDL_SCANCODE_KP_0:
        return KEYCODE_NUMPAD_0;
    case SDL_SCANCODE_KP_1:
        return KEYCODE_NUMPAD_1;
    case SDL_SCANCODE_KP_2:
        return KEYCODE_NUMPAD_2;
    case SDL_SCANCODE_KP_3:
        return KEYCODE_NUMPAD_3;
    case SDL_SCANCODE_KP_4:
        return KEYCODE_NUMPAD_4;
    case SDL_SCANCODE_KP_5:
        return KEYCODE_NUMPAD_5;
    case SDL_SCANCODE_KP_6:
        return KEYCODE_NUMPAD_6;
    case SDL_SCANCODE_KP_7:
        return KEYCODE_NUMPAD_7;
    case SDL_SCANCODE_KP_8:
        return KEYCODE_NUMPAD_8;
    case SDL_SCANCODE_KP_9:
        return KEYCODE_NUMPAD_9;
    // NO NUMPAD_DELETE
    case SDL_SCANCODE_NUMLOCKCLEAR:
        return KEYCODE_NUMPAD_NUMLOCK;
    case SDL_SCANCODE_KP_DIVIDE:
        return KEYCODE_NUMPAD_DIVIDE;
    case SDL_SCANCODE_KP_MULTIPLY:
        return KEYCODE_NUMPAD_MULTIPLY;
    case SDL_SCANCODE_KP_PLUS:
        return KEYCODE_NUMPAD_PLUS;
    case SDL_SCANCODE_KP_MINUS:
        return KEYCODE_NUMPAD_MINUS;
    case SDL_SCANCODE_KP_ENTER:
        return KEYCODE_NUMPAD_ENTER;

    case SDL_SCANCODE_F1:
        return KEYCODE_F1;
    case SDL_SCANCODE_F2:
        return KEYCODE_F2;
    case SDL_SCANCODE_F3:
        return KEYCODE_F3;
    case SDL_SCANCODE_F4:
        return KEYCODE_F4;
    case SDL_SCANCODE_F5:
        return KEYCODE_F5;
    case SDL_SCANCODE_F6:
        return KEYCODE_F6;
    case SDL_SCANCODE_F7:
        return KEYCODE_F7;
    case SDL_SCANCODE_F8:
        return KEYCODE_F8;
    case SDL_SCANCODE_F9:
        return KEYCODE_F9;
    case SDL_SCANCODE_F10:
        return KEYCODE_F10;
    case SDL_SCANCODE_F11:
        return KEYCODE_F11;
    case SDL_SCANCODE_F12:
        return KEYCODE_F12;
    case SDL_SCANCODE_F13:
        return KEYCODE_F13;
    case SDL_SCANCODE_F14:
        return KEYCODE_F14;
    case SDL_SCANCODE_F15:
        return KEYCODE_F15;
    case SDL_SCANCODE_F16:
        return KEYCODE_F16;
    case SDL_SCANCODE_F17:
        return KEYCODE_F17;
    case SDL_SCANCODE_F18:
        return KEYCODE_F18;
    case SDL_SCANCODE_F19:
        return KEYCODE_F19;
    case SDL_SCANCODE_F20:
        return KEYCODE_F20;
    case SDL_SCANCODE_F21:
        return KEYCODE_F21;
    case SDL_SCANCODE_F22:
        return KEYCODE_F22;
    case SDL_SCANCODE_F23:
        return KEYCODE_F23;
    case SDL_SCANCODE_F24:
        return KEYCODE_F24;

    case SDL_SCANCODE_LGUI:
        return KEYCODE_LGUI;
    case SDL_SCANCODE_RGUI:
        return KEYCODE_RGUI;
    case SDL_SCANCODE_LCTRL:
        return KEYCODE_LCTRL;
    case SDL_SCANCODE_RCTRL:
        return KEYCODE_RCTRL;
    case SDL_SCANCODE_LSHIFT:
        return KEYCODE_LSHIFT;
    case SDL_SCANCODE_RSHIFT:
        return KEYCODE_RSHIFT;
    case SDL_SCANCODE_LALT:
        return KEYCODE_LALT;
    case SDL_SCANCODE_RALT:
        return KEYCODE_RALT;

    case SDL_SCANCODE_GRAVE:
        return KEYCODE_GRAVE;
    case SDL_SCANCODE_LEFTBRACKET:
        return KEYCODE_LEFTBRACKET;
    case SDL_SCANCODE_RIGHTBRACKET:
        return KEYCODE_RIGHTBRACKET;
    case SDL_SCANCODE_BACKSLASH:
        return KEYCODE_BACKSLASH;
    case SDL_SCANCODE_SEMICOLON:
        return KEYCODE_SEMICOLON;
    case SDL_SCANCODE_APOSTROPHE:
        return KEYCODE_QUOTE;
    case SDL_SCANCODE_COMMA:
        return KEYCODE_COMMA;
    case SDL_SCANCODE_PERIOD:
        return KEYCODE_PERIOD;
    case SDL_SCANCODE_SLASH:
        return KEYCODE_FORWARDSLASH;

    case SDL_SCANCODE_MENU:
        return KEYCODE_MENU;
    case SDL_SCANCODE_PRINTSCREEN:
        return KEYCODE_PRINTSCREEN;
    case SDL_SCANCODE_PAUSE:
        return KEYCODE_PAUSE;

    default:
        return KEYCODE_NONE;
    }
}

uint32_t keycode_from_sdl_mouse_button(const SDL_MouseButtonFlags code) {
    switch (code) {
    case 0:
        return KEYCODE_MOUSE_NONE;
    case SDL_BUTTON_LMASK:
        return KEYCODE_MOUSE_LEFT;
    case SDL_BUTTON_MMASK:
        return KEYCODE_MOUSE_MIDDLE;
    case SDL_BUTTON_RMASK:
        return KEYCODE_MOUSE_RIGHT;
    case SDL_BUTTON_X1MASK:
        return KEYCODE_MOUSE_4;
    case SDL_BUTTON_X2MASK:
        return KEYCODE_MOUSE_5;

    default:
        return KEYCODE_MOUSE_NONE;
    }
};

uint32_t keycode_from_sdl_gamepad(const SDL_GamepadButton code) {
    switch (code) {
    case SDL_GAMEPAD_BUTTON_INVALID:
        return KEYCODE_GAMEPAD_NONE;
    case SDL_GAMEPAD_BUTTON_SOUTH:
        return KEYCODE_GAMEPAD_SOUTH;
    case SDL_GAMEPAD_BUTTON_EAST:
        return KEYCODE_GAMEPAD_EAST;
    case SDL_GAMEPAD_BUTTON_WEST:
        return KEYCODE_GAMEPAD_WEST;
    case SDL_GAMEPAD_BUTTON_NORTH:
        return KEYCODE_GAMEPAD_NORTH;
    case SDL_GAMEPAD_BUTTON_BACK:
        return KEYCODE_GAMEPAD_BACK;
    case SDL_GAMEPAD_BUTTON_GUIDE:
        return KEYCODE_GAMEPAD_GUIDE;
    case SDL_GAMEPAD_BUTTON_START:
        return KEYCODE_GAMEPAD_START;
    case SDL_GAMEPAD_BUTTON_LEFT_STICK:
        return KEYCODE_GAMEPAD_LEFT_STICK;
    case SDL_GAMEPAD_BUTTON_RIGHT_STICK:
        return KEYCODE_GAMEPAD_RIGHT_STICK;
    case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER:
        return KEYCODE_GAMEPAD_LEFT_SHOULDER;
    case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER:
        return KEYCODE_GAMEPAD_RIGHT_SHOULDER;
    case SDL_GAMEPAD_BUTTON_DPAD_UP:
        return KEYCODE_GAMEPAD_DPAD_UP;
    case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
        return KEYCODE_GAMEPAD_DPAD_DOWN;
    case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
        return KEYCODE_GAMEPAD_DPAD_LEFT;
    case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
        return KEYCODE_GAMEPAD_DPAD_RIGHT;
    case SDL_GAMEPAD_BUTTON_MISC1:
        return KEYCODE_GAMEPAD_MISC1;
    case SDL_GAMEPAD_BUTTON_RIGHT_PADDLE1:
        return KEYCODE_GAMEPAD_RIGHT_PADDLE1;
    case SDL_GAMEPAD_BUTTON_LEFT_PADDLE1:
        return KEYCODE_GAMEPAD_LEFT_PADDLE1;
    case SDL_GAMEPAD_BUTTON_RIGHT_PADDLE2:
        return KEYCODE_GAMEPAD_RIGHT_PADDLE2;
    case SDL_GAMEPAD_BUTTON_LEFT_PADDLE2:
        return KEYCODE_GAMEPAD_LEFT_PADDLE2;
    case SDL_GAMEPAD_BUTTON_TOUCHPAD:
        return KEYCODE_GAMEPAD_TOUCHPAD;
    case SDL_GAMEPAD_BUTTON_MISC2:
        return KEYCODE_GAMEPAD_MISC2;
    case SDL_GAMEPAD_BUTTON_MISC3:
        return KEYCODE_GAMEPAD_MISC3;
    case SDL_GAMEPAD_BUTTON_MISC4:
        return KEYCODE_GAMEPAD_MISC4;
    case SDL_GAMEPAD_BUTTON_MISC5:
        return KEYCODE_GAMEPAD_MISC5;
    case SDL_GAMEPAD_BUTTON_MISC6:
        return KEYCODE_GAMEPAD_MISC6;

    default:
        return KEYCODE_GAMEPAD_NONE;
    }
}

enum keycodes_modifier_flags keycode_modifier_from_sdl(const SDL_Keymod mod) {
    enum keycodes_modifier_flags rtn = KEYCODE_MODIFER_NONE;
    if (mod == SDL_KMOD_NONE) { return rtn; }
    rtn |= (mod & SDL_KMOD_LSHIFT) ? KEYCODE_MODIFER_LSHIFT : KEYCODE_MODIFER_NONE;
    rtn |= (mod & SDL_KMOD_RSHIFT) ? KEYCODE_MODIFER_RSHIFT : KEYCODE_MODIFER_NONE;
    rtn |= (mod & SDL_KMOD_LCTRL) ? KEYCODE_MODIFER_LCONTROL : KEYCODE_MODIFER_NONE;
    rtn |= (mod & SDL_KMOD_RCTRL) ? KEYCODE_MODIFER_RCONTROL : KEYCODE_MODIFER_NONE;
    rtn |= (mod & SDL_KMOD_LALT) ? KEYCODE_MODIFER_LALT : KEYCODE_MODIFER_NONE;
    rtn |= (mod & SDL_KMOD_RALT) ? KEYCODE_MODIFER_RALT : KEYCODE_MODIFER_NONE;
    return rtn;
}
