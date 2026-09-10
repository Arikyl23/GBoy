/**
 * @file keycodes.h
 * @brief Defines keycodes for keyboards, mouses, and controllers.
 */
#pragma once

enum keycodes {
    KEYCODE_NONE,

    /* Alpha */
    KEYCODE_A,
    KEYCODE_B,
    KEYCODE_C,
    KEYCODE_D,
    KEYCODE_E,
    KEYCODE_F,
    KEYCODE_G,
    KEYCODE_H,
    KEYCODE_I,
    KEYCODE_J,
    KEYCODE_K,
    KEYCODE_L,
    KEYCODE_M,
    KEYCODE_N,
    KEYCODE_O,
    KEYCODE_P,
    KEYCODE_Q,
    KEYCODE_R,
    KEYCODE_S,
    KEYCODE_T,
    KEYCODE_U,
    KEYCODE_V,
    KEYCODE_W,
    KEYCODE_X,
    KEYCODE_Y,
    KEYCODE_Z,

    /* Numeric (Alpha Row)*/
    KEYCODE_0,
    KEYCODE_1,
    KEYCODE_2,
    KEYCODE_3,
    KEYCODE_4,
    KEYCODE_5,
    KEYCODE_6,
    KEYCODE_7,
    KEYCODE_8,
    KEYCODE_9,
    KEYCODE_MINUS,
    KEYCODE_EQUALS,

    /* General */
    KEYCODE_CAPSLOCK,
    KEYCODE_SPACE,
    KEYCODE_TAB,
    KEYCODE_ENTER,
    KEYCODE_ESCAPE,
    KEYCODE_BACKSPACE,

    /* Cursor */
    KEYCODE_SCROLLLOCK,
    KEYCODE_DELETE,
    KEYCODE_INSERT,
    KEYCODE_HOME,
    KEYCODE_END,
    KEYCODE_PAGEUP,
    KEYCODE_PAGEDOWN,
    KEYCODE_UP,
    KEYCODE_DOWN,
    KEYCODE_LEFT,
    KEYCODE_RIGHT,

    /* Numpad */
    KEYCODE_NUMPAD_0,
    KEYCODE_NUMPAD_1,
    KEYCODE_NUMPAD_2,
    KEYCODE_NUMPAD_3,
    KEYCODE_NUMPAD_4,
    KEYCODE_NUMPAD_5,
    KEYCODE_NUMPAD_6,
    KEYCODE_NUMPAD_7,
    KEYCODE_NUMPAD_8,
    KEYCODE_NUMPAD_9,
    KEYCODE_NUMPAD_DELETE,
    KEYCODE_NUMPAD_NUMLOCK,
    KEYCODE_NUMPAD_DIVIDE,
    KEYCODE_NUMPAD_MULTIPLY,
    KEYCODE_NUMPAD_PLUS,
    KEYCODE_NUMPAD_MINUS,
    KEYCODE_NUMPAD_ENTER,

    /* Function */
    KEYCODE_F1,
    KEYCODE_F2,
    KEYCODE_F3,
    KEYCODE_F4,
    KEYCODE_F5,
    KEYCODE_F6,
    KEYCODE_F7,
    KEYCODE_F8,
    KEYCODE_F9,
    KEYCODE_F10,
    KEYCODE_F11,
    KEYCODE_F12,
    KEYCODE_F13,
    KEYCODE_F14,
    KEYCODE_F15,
    KEYCODE_F16,
    KEYCODE_F17,
    KEYCODE_F18,
    KEYCODE_F19,
    KEYCODE_F20,
    KEYCODE_F21,
    KEYCODE_F22,
    KEYCODE_F23,
    KEYCODE_F24,

    /* Modifier */
    KEYCODE_LGUI,
    KEYCODE_RGUI,
    KEYCODE_LCTRL,
    KEYCODE_RCTRL,
    KEYCODE_LSHIFT,
    KEYCODE_RSHIFT,
    KEYCODE_LALT,
    KEYCODE_RALT,

    /* Symbol */
    KEYCODE_GRAVE,
    KEYCODE_LEFTBRACKET,
    KEYCODE_RIGHTBRACKET,
    KEYCODE_BACKSLASH,
    KEYCODE_SEMICOLON,
    KEYCODE_QUOTE,
    KEYCODE_COMMA,
    KEYCODE_PERIOD,
    KEYCODE_FORWARDSLASH,

    /* Other */
    KEYCODE_MENU,
    KEYCODE_PRINTSCREEN,
    KEYCODE_PAUSE
};

enum keycodes_modifier_flags {
    KEYCODE_MODIFER_NONE     = 0,
    KEYCODE_MODIFER_LSHIFT   = 1 << 0,
    KEYCODE_MODIFER_RSHIFT   = 1 << 1,
    KEYCODE_MODIFER_LCONTROL = 1 << 2,
    KEYCODE_MODIFER_RCONTROL = 1 << 3,
    KEYCODE_MODIFER_LALT     = 1 << 4,
    KEYCODE_MODIFER_RALT     = 1 << 5
};

enum keycodes_mouse {
    KEYCODE_MOUSE_NONE = 0,
    KEYCODE_MOUSE_LEFT = 1,
    KEYCODE_MOUSE_RIGHT,
    KEYCODE_MOUSE_MIDDLE,
    KEYCODE_MOUSE_4,
    KEYCODE_MOUSE_5,
    KEYCODE_MOUSE_6,
    KEYCODE_MOUSE_7,
    KEYCODE_MOUSE_8,
    KEYCODE_MOUSE_9,
    KEYCODE_MOUSE_10,
    KEYCODE_MOUSE_11,
    KEYCODE_MOUSE_12
};

enum keycodes_gamepad {
    KEYCODE_GAMEPAD_NONE = 0,
    KEYCODE_GAMEPAD_SOUTH, /**< Bottom face button (e.g. Xbox A button) */
    KEYCODE_GAMEPAD_EAST,  /**< Right face button (e.g. Xbox B button) */
    KEYCODE_GAMEPAD_WEST,  /**< Left face button (e.g. Xbox X button) */
    KEYCODE_GAMEPAD_NORTH, /**< Top face button (e.g. Xbox Y button) */
    KEYCODE_GAMEPAD_BACK,
    KEYCODE_GAMEPAD_GUIDE,
    KEYCODE_GAMEPAD_START,
    KEYCODE_GAMEPAD_LEFT_STICK,
    KEYCODE_GAMEPAD_RIGHT_STICK,
    KEYCODE_GAMEPAD_LEFT_SHOULDER,
    KEYCODE_GAMEPAD_RIGHT_SHOULDER,
    KEYCODE_GAMEPAD_DPAD_UP,
    KEYCODE_GAMEPAD_DPAD_DOWN,
    KEYCODE_GAMEPAD_DPAD_LEFT,
    KEYCODE_GAMEPAD_DPAD_RIGHT,
    KEYCODE_GAMEPAD_MISC1,         /**< Additional button (e.g. Xbox Series X share button, PS5
                                         microphone button, Nintendo Switch Pro capture button, Amazon Luna
                                         microphone button, Google Stadia capture button) */
    KEYCODE_GAMEPAD_RIGHT_PADDLE1, /**< Upper or primary paddle, under your right hand (e.g. Xbox
                                         Elite paddle P1, DualSense Edge RB button, Right Joy-Con SR
                                         button) */
    KEYCODE_GAMEPAD_LEFT_PADDLE1,  /**< Upper or primary paddle, under your left hand (e.g. Xbox
                                         Elite paddle P3, DualSense Edge LB button, Left Joy-Con SL
                                         button) */
    KEYCODE_GAMEPAD_RIGHT_PADDLE2, /**< Lower or secondary paddle, under your right hand (e.g.
                                         Xbox Elite paddle P2, DualSense Edge right Fn button, Right
                                         Joy-Con SL button) */
    KEYCODE_GAMEPAD_LEFT_PADDLE2,  /**< Lower or secondary paddle, under your left hand (e.g. Xbox
                                         Elite paddle P4, DualSense Edge left Fn button, Left Joy-Con
                                         SR button) */
    KEYCODE_GAMEPAD_TOUCHPAD,      /**< PS4/PS5 touchpad button */
    KEYCODE_GAMEPAD_MISC2,         /**< Additional button */
    KEYCODE_GAMEPAD_MISC3, /**< Additional button (e.g. Nintendo GameCube left trigger click) */
    KEYCODE_GAMEPAD_MISC4, /**< Additional button (e.g. Nintendo GameCube right trigger click) */
    KEYCODE_GAMEPAD_MISC5, /**< Additional button */
    KEYCODE_GAMEPAD_MISC6  /**< Additional button */
};
