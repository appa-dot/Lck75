#pragma once

/* key matrix size */
#define MATRIX_ROWS 6
#define MATRIX_COLS 16

#define MATRIX_ROW_PINS \
    { C2, C3, C7, C4, C6, C5 }
#define MATRIX_COL_PINS \
    { A0, B0, A1, B1, A2, B2, A3, B3, A4, B4, A5, A6, A7, D7, D6, D5 }

#define ENCODERS_PAD_B \
    { D1 }
#define ENCODERS_PAD_A \
    { D0 }
#define ENCODER_RESOLUTION 2 // default/suggested

/* COL2ROW, ROW2COL*/
#define DIODE_DIRECTION COL2ROW

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
#define DEBOUNCE 10

#define WPM_LAUNCH_CONTROL
#define PERMISSIVE_HOLD
#define OLED_TIMEOUT 60000

/* Bootmagic Lite key configuration */
#define BOOTMAGIC_LITE_ROW 0
#define BOOTMAGIC_LITE_COLUMN 0

#define NO_ACTION_ONESHOT
#define NO_MUSIC_MODE

#undef LOCKING_SUPPORT_ENABLE
#undef LOCKING_RESYNC_ENABLE
#undef RGBLIGHT_ANIMATIONS
#undef RGBLIGHT_EFFECT_BREATHING
#undef RGBLIGHT_EFFECT_RAINBOW_MOOD
#undef RGBLIGHT_EFFECT_RAINBOW_SWIRL
#undef RGBLIGHT_EFFECT_SNAKE
#undef RGBLIGHT_EFFECT_KNIGHT
#undef RGBLIGHT_EFFECT_CHRISTMAS
#undef RGBLIGHT_EFFECT_STATIC_GRADIENT
#undef RGBLIGHT_EFFECT_RGB_TEST
#undef RGBLIGHT_EFFECT_ALTERNATING
#undef RGBLIGHT_EFFECT_TWINKLE