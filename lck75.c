#include "quantum.h"

/** Fimware size chart
* default without oled           : 10528/28672
* default without oled code      : 12260/28672
* default with oled (-3 frames)  : 21228/28672
* default with oled              : 21228/28672
* vial without oled              : 17800/28672
* vial without oled code         : 19536/28672
* vial with oled (-3 frames)     : 28462/28672
*/

#ifdef OLED_ENABLE

// definitions
#define WAIT_MULTIPLIER 5
#define TAP_SPEED 40
#define ANIM_SIZE 512
#define ANIM_AMOUNT 2
#define OLED_TIMEOUT 60000

oled_rotation_t oled_init_kb(oled_rotation_t rotation) {
    return OLED_ROTATION_180;
}

/**
 * Declarations of static variables for animations
 */
static uint32_t anim_timer = 0;
static uint32_t anim_sleep = 0;
static uint8_t  curr_idle  = 0;
static uint8_t  curr_prep  = 0;
static uint8_t  curr_tap   = 0;
static bool     prepped    = false;
static bool     prep_wait  = false;

// struct with all needed information about a single animation
typedef struct {
    uint8_t idle;
    uint8_t prep;
    uint8_t tap;
    uint8_t frame_time;
} animation_details;

static const char PROGMEM appa_idle[][ANIM_SIZE] = {{0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,   0,   0,   0,  0,  0,   0,   0,   0,   0,   0,   0,  0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  128, 192, 192, 224, 96, 112, 112, 56, 56, 24, 28,  156, 156, 220, 124, 60,  12, 236, 252, 252, 48,  208, 248, 120, 14, 7, 127, 252, 198, 131, 1, 1, 0,   0,   0,   0,  0,  6,  15, 15, 27, 25,  25,  48, 48, 96,  96,  224, 192, 192, 128, 129, 3, 3, 131, 195, 231, 118, 60, 56, 112, 224, 192, 192, 240, 126, 7,   254, 0,   0,   0,   0,  0,  0,  0,  0,  0,  0, 0, 0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,   0,   0,   0,   0,   0,   0,  0,  0,  0,   0,   0,   0,   0,  0,  0,  128, 192, 224, 96, 120, 124, 126, 94, 87, 211,
                                                     177, 161, 160, 160, 160, 176, 152, 204, 78,  103, 51,  17,  24,  14,  7,  255, 255, 255, 0,  0,  3,   7,   14,  12,  28,  24,  28, 15, 135, 128, 128, 128, 128, 128, 128, 128, 128, 192, 64, 64, 64, 64, 64,  64,  64,  64,  64, 64,  97,  97, 99, 67, 71,  67,  67,  64,  192, 192, 64, 64,  193, 255, 253, 124, 6,   7,   3,  0, 0,   0,   0,   0,   0, 0, 0,   0,   0,   0,  0,  0,  0,  0,  0,  0,   0,   0,  0,  0,   0,   0,   0,   0,   0,   0,   0, 0, 0,   0,   0,   0,   0,  0,  0,   128, 128, 192, 192, 224, 96,  112, 48,  56,  24,  28, 12, 14, 6,  7,  3,  1, 0, 0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 192, 255, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 15, 255, 247, 231, 192, 224, 112, 48, 24, 24, 26,  95,  223, 223, 28, 24, 24, 24,  25,  89,  88, 24,  24,  60,  60, 52, 54,
                                                     54,  54,  54,  54,  118, 102, 102, 198, 198, 198, 198, 102, 236, 188, 60, 92,  28,  6,   13, 24, 240, 224, 128, 128, 128, 128, 0,  0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,   0,   0,   0,   0,  0,   0,   0,  0,  0,  128, 192, 224, 48,  24,  12,  6,  3,   1,   1,   0,   0,   0,   0,   0,  0, 0,   0,   0,   0,   0, 0, 128, 224, 120, 24, 24, 24, 28, 14, 6,  254, 240, 0,  0,  254, 255, 0,   0,   0,   0,   0,   0, 0, 0,   0,   0,   0,   0,  0,  3,   243, 255, 223, 0,   224, 240, 120, 124, 124, 119, 63, 27, 31, 30, 30, 12, 0, 1, 129, 192, 192, 128, 0, 0, 0, 0, 0, 0, 0,   1,   5, 5, 5, 1, 1, 1, 0, 0, 0, 0, 0, 0,  0,   128, 64,  32,  1,   0,   0,  7,  15, 248, 112, 0,   0,   0,  0,  0,  0,   0,   0,   0,  0,   0,   0,   0,  0},
                                                    {0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,   0,   0,   0,  0,   0,   0,   0,   0,  0,  0,  0,  0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  128, 192, 192, 224, 96, 112, 112, 56, 56, 24, 28,  156, 156, 220, 124, 60,  12, 236, 252, 252, 48,  208, 248, 120, 14, 7, 127, 252, 198, 131, 1, 1, 0,   0,   0,   0,  0,  6,  15, 15, 27, 25,  25,  48, 48, 96,  96,  224, 192, 192, 128, 129, 3, 3, 131, 195, 231, 118, 60, 56, 112, 224, 192, 192, 240, 126, 7,  254, 0,   0,   0,   0,   0,   0,   0,  0,   0,  0,  0, 0, 0, 0,   0,   0,   0, 0, 0, 0, 0, 0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,   0,   0,   0,   0,   0,  0,  0,   0,   0,   0,  0,  0,  0,  0,  128, 192, 224, 96, 120, 124, 126, 94, 87,  211,
                                                     177, 161, 160, 160, 160, 176, 152, 204, 78,  103, 51,  17,  24,  14, 7,  255, 255, 255, 0,  0,   3,   7,   14,  12, 28, 24, 28, 15, 135, 128, 128, 128, 128, 128, 128, 128, 128, 192, 64, 64, 64, 64, 64,  64,  64,  64,  64, 64,  97,  97, 99, 67, 71,  67,  67,  64,  192, 192, 64, 64,  193, 255, 253, 124, 6,   7,   3,  0, 0,   0,   0,   0,   0, 0, 0,   0,   0,   0,  0,  0,  0,  0,  0,  0,   0,   0,  0,  0,   0,   0,   0,   0,   0,   0,   0, 0, 0,   0,   0,   0,   0,  0,  0,   128, 128, 192, 192, 224, 96, 112, 48,  56,  24,  28,  12,  14,  6,  7,   3,  1,  0, 0, 0, 0,   0,   0,   0, 0, 0, 0, 0, 192, 255, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 15, 63, 215, 71,  224, 192, 240, 48, 24, 24,  154, 223, 63, 31, 28, 24, 24, 88,  89,  25,  24, 24,  24,  28,  60, 180, 182,
                                                     182, 54,  118, 118, 118, 102, 198, 198, 198, 198, 198, 102, 108, 60, 60, 60,  28,  6,   13, 120, 240, 192, 128, 0,  0,  0,  0,  0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,   0,   0,   0,   0,  0,   0,   0,  0,  0,  128, 192, 224, 48,  24,  12,  6,  3,   1,   1,   0,   0,   0,   0,   0,  0, 0,   0,   0,   0,   0, 0, 128, 224, 120, 24, 24, 24, 28, 14, 6,  254, 240, 0,  0,  254, 255, 0,   0,   0,   0,   0,   0, 0, 0,   0,   0,   0,   0,  0,  0,   225, 243, 255, 15,  7,   7,  6,   205, 248, 248, 221, 239, 247, 27, 155, 11, 11, 5, 1, 0, 128, 128, 128, 0, 2, 0, 0, 0, 0,   0,   1, 1, 5, 1, 1, 0, 0, 0, 0, 0, 0, 0,  0,  0,   128, 0,   1,   1,   7,  6,  124, 248, 0,   0,  0,  0,  0,  0,  0,   0,   0,   0,  0,   0,   0,   0,  0},
                                                    {0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,   0,   0,   0,  0,   0,   0,   0,   0,   0,  0,  0,  0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  128, 192, 192, 224, 96, 112, 112, 56, 56, 24, 28,  156, 156, 220, 124, 60,  12, 236, 252, 252, 48,  208, 248, 120, 14, 7, 127, 252, 198, 131, 1, 1, 0,   0,   0,   0,  0,  6,  15, 15, 27, 25,  25,  48, 48, 96,  96,  224, 192, 192, 128, 129, 3, 3, 131, 195, 231, 118, 60, 56, 112, 224, 192, 192, 240, 126, 7,  254, 0,   0,   0,   0,   0,   0,   0,   0,  0,  0, 0, 0, 0,   0,   0,   0,   0, 0, 0, 0, 0, 0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,   0,  0,   0,   0,   0,   0,  0,  0,  0,   0,   0,  0,  0,  0,  0,  128, 192, 224, 96, 120, 124, 126, 94, 87, 211,
                                                     177, 161, 160, 160, 160, 176, 152, 204, 78,  103, 51,  17,  24,  14, 7,  255, 255, 255, 0,  0,   3,   7,   14,  12,  28, 24, 28, 15, 135, 128, 128, 128, 128, 128, 128, 128, 128, 192, 64, 64, 64, 64, 64,  64,  64,  64,  64, 64,  97,  97, 99, 67, 71,  67,  67,  64,  192, 192, 64, 64,  193, 255, 253, 124, 6,   7,   3,  0, 0,   0,   0,   0,   0, 0, 0,   0,   0,   0,  0,  0,  0,  0,  0,  0,   0,   0,  0,  0,   0,   0,   0,   0,   0,   0,   0, 0, 0,   0,   0,   0,   0,  0,  0,   128, 128, 192, 192, 224, 96, 112, 48,  56,  24,  28,  12,  14,  6,   7,  3,  1, 0, 0, 0,   0,   0,   0,   0, 0, 0, 0, 0, 192, 255, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 15, 255, 23, 199, 160, 192, 112, 48, 24, 24, 154, 223, 63, 31, 28, 24, 24, 24,  25,  89,  24, 24,  24,  28,  60, 52, 54,
                                                     54,  54,  54,  54,  118, 102, 198, 198, 198, 198, 198, 102, 108, 60, 60, 60,  28,  6,   13, 120, 240, 192, 128, 128, 0,  0,  0,  0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,   0,   0,   0,   0,  0,   0,   0,  0,  0,  128, 192, 224, 48,  24,  12,  6,  3,   1,   1,   0,   0,   0,   0,   0,  0, 0,   0,   0,   0,   0, 0, 128, 224, 120, 24, 24, 24, 28, 14, 6,  254, 240, 0,  0,  254, 255, 0,   0,   0,   0,   0,   0, 0, 0,   0,   0,   0,   0,  0,  0,   225, 254, 255, 47,  6,   4,  232, 248, 185, 204, 254, 247, 251, 255, 31, 14, 1, 1, 0, 128, 192, 192, 128, 0, 0, 0, 0, 0, 0,   0,   0, 5, 5, 5, 1, 0, 0, 0, 0, 0, 0, 0,  0,   0,  0,   0,   1,   1,   3,  7,  60, 240, 0,   0,  0,  0,  0,  0,  0,   0,   0,   0,  0,   0,   0,   0,  0}};

static const char PROGMEM appa_prep[][ANIM_SIZE] = {{0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   128, 192, 192, 224, 96,  112, 112, 56,  56,  24,  28,  28,  156, 220, 252, 124, 188, 252, 252, 252, 28, 12, 255, 255, 248, 140, 6, 2, 3, 3, 1, 0, 4,   14,  14,  26, 26, 50, 51, 115, 96, 192, 192, 128, 128, 1,   3,   3, 2, 6, 134, 198, 244, 60, 9, 3, 3, 14, 28, 248, 224, 128, 224, 60,  15,  252, 0,  0,   0,   0,   0,   0,   0,   0,  0, 0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0,   0, 0, 0, 0, 0,   0,   0, 0, 0,   0, 0, 0,   0,   0,   0,   0,  0,   0,   0,  0, 0,  0,  0,   0,   0,  0,  0,  0,  128, 192, 224, 96, 120, 124, 126, 94,  87,  211,
                                                     177, 161, 160, 160, 160, 176, 152, 204, 78,  102, 51, 49, 24, 14, 127, 255, 224, 135, 15,  12,  28, 28, 28, 15, 143, 131, 128, 128, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 193, 193, 195, 195, 198, 198, 198, 199, 195, 193, 192, 192, 192, 192, 192, 192, 128, 15,  217, 240, 248, 12, 7,  1,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0,   0,   0,   0,  0,  0,  0,  0,   0,  0,   0,   0,   0,   0,   0,   0, 0, 0, 0,   0,   0,   0,  0, 0, 0, 0,  0,  0,   0,   128, 128, 192, 192, 224, 96, 112, 48,  56,  24,  28,  12,  14, 6, 7,   3,   1,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 192, 255, 3, 0, 0, 0, 0,   0,   0, 0, 0,   0, 1, 193, 195, 227, 247, 63, 31,  31,  79, 7, 15, 15, 143, 141, 12, 12, 24, 24, 24,  25,  57,  56, 252, 108, 100, 100, 100, 198,
                                                     198, 134, 134, 134, 198, 196, 196, 228, 124, 56,  57, 25, 8,  8,  13,  15,  15,  31,  243, 224, 0,  0,  0,  0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   128, 192, 224, 48,  24,  12,  6,   3,   1,   1,   0,  0,  0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 128, 224, 120, 24, 24, 24, 28, 14,  6,  254, 240, 0,   0,   254, 255, 0, 0, 0, 0,   0,   0,   0,  0, 0, 0, 0,  0,  3,   231, 255, 255, 221, 246, 255, 29, 206, 247, 251, 207, 135, 128, 0,  0, 128, 192, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,   1,   0, 0, 0, 0, 128, 128, 0, 0, 128, 0, 0, 0,   0,   1,   3,   6,  252, 248, 0,  0, 0,  0,  0,   0,   0,  0,  0,  0,  0,   0,   0,   0,  0,   0,   0,   0,   0},
                                                    {0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  128, 128, 192, 224, 96,  112, 48,  56,  56,  24,  24,  24,  152, 248, 248, 56,  28, 6,   63,  124, 230, 195, 129, 1, 0, 0, 0, 0, 2, 6, 14, 26, 50, 114, 98,  195, 195, 128, 0,  0,  0, 0,   0,   3, 7, 134, 198, 230, 102, 54, 30, 0, 0, 1, 3, 7, 14, 156, 248, 240, 96, 56,  4,   252, 0,   0,   0,   0,  0,  0,  0,   0,  0,   0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0,   0,   0,   0,   0,   0, 0, 0, 0, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0,  0,  0,  0,  0,   0,   0,   128, 192, 224, 112, 124, 94,  95,  87,  211, 177,
                                                     161, 160, 160, 160, 160, 240, 120, 124, 62, 31, 15, 7, 59, 255, 142, 252, 152, 152, 136, 135, 135, 128, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 193, 65, 67, 70, 78, 76, 76, 78,  71,  195, 193, 192, 192, 192, 192, 192, 192, 192, 192, 192, 207, 223, 153, 16, 216, 252, 62,  3,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,   0,   0,   0,   0,   0,  0,  0, 0,   0,   0, 0, 0,   0,   0,   0,   0,  0,  0, 0, 0, 0, 0, 0,  0,   0,   0,   0,  0,   128, 128, 192, 192, 192, 96, 96, 48, 48,  56, 24,  28, 12, 14, 7, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 224, 255, 7,   0,   0,   0,   0, 0, 0, 1, 32, 203, 203, 207, 6,   196, 196, 135, 135, 140, 136, 8, 8, 25, 25, 24, 56, 120, 100, 100, 100, 228, 196, 196, 198, 198, 198, 198, 196, 196,
                                                     228, 100, 100, 100, 56,  56,  25,  9,   8,  4,  4,  2, 15, 7,   253, 255, 31,  15,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   192, 224, 112, 24,  12, 6,   3,   3,   1,   1,   0,   0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  128, 224, 120, 24,  24,  16, 60, 0, 252, 224, 0, 0, 0,   252, 255, 1,   0,  0,  0, 0, 0, 0, 0, 0,  0,   65,  243, 31, 238, 14,  247, 251, 255, 247, 99, 0,  0,  192, 0,  128, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0,   128, 128, 128, 128, 0, 0, 0, 0, 0,  7,   15,  248, 248, 0,   0,   0,   0,   0,   0,   0, 0, 0,  0,  0,  0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
                                                    {0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,   0,   0,   0,   0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   128, 128, 192, 192, 96,  112, 48,  56,  24,  24,  252, 252, 62,  3,   31,  124, 248, 136, 140, 6, 3, 1, 0, 0, 0, 12, 30, 54, 102, 198, 131, 3, 0, 0,   0,   0,  0,  0,  0,  131, 195, 226, 102, 52, 60, 12, 4,   0,   0, 0, 0, 1, 1, 3, 134, 206, 252, 248, 28,  12,  252, 0,  0,   0,   0,   0,   0,   0,  0,  0,  0,   0,  0,  0,  0,  0, 0, 0, 0, 0, 0,   0,   0,   0,   0, 0, 0,   0,   0,  0, 0,   0,   0, 0, 0, 0, 0, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0,  0, 0, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   128, 224, 112, 120, 92,  94,  87,  83,  211, 177,
                                                     160, 160, 160, 160, 240, 248, 124, 63, 11, 15, 134, 252, 140, 140, 12, 207, 135, 128, 128, 128, 192, 192, 192, 192, 192, 192, 192, 192, 193, 195, 199, 198, 204, 204, 198, 199, 195, 193, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 199, 159, 25,  16,  16,  120, 252, 207, 0,   0, 0, 0, 0, 0, 0, 0,  0,  0,  0,   0,   0,   0, 0, 0,   0,   0,  0,  0,  0,  0,   0,   0,   0,   0,  0,  0,  0,   0,   0, 0, 0, 0, 0, 0, 0,   0,   0,   0,   0,   0,   0,   0,  128, 128, 128, 192, 192, 96, 96, 48, 56,  24, 28, 12, 14, 6, 3, 1, 0, 0, 0,   0,   0,   0,   0, 0, 0,   1,   1,  1, 240, 255, 3, 0, 0, 0, 0, 1,  1,   247, 223, 165, 231, 199, 199, 199, 13, 8, 8, 24, 121, 121, 228, 196, 196, 196, 198, 134, 134, 134, 134, 134, 198, 198, 196, 228, 100, 104, 120, 56,
                                                     56,  9,   9,   9,   12,  4,   7,   7,  7,  7,  29,  248, 240, 121, 31, 15,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   128, 224, 112, 24,  12,  6,   3,   1, 1, 0, 0, 0, 0, 0,  0,  0,  0,   0,   0,   0, 0, 128, 224, 56, 24, 24, 16, 24,  248, 240, 0,   0,  0,  0,  254, 255, 1, 0, 0, 0, 0, 0, 248, 238, 115, 226, 195, 253, 254, 71, 115, 32,  192, 192, 0,   0,  0,  0,  128, 0,  0,  0,  1,  1, 1, 1, 1, 0, 128, 128, 128, 128, 0, 0, 128, 192, 64, 0, 0,   0,   0, 0, 0, 0, 4, 15, 255, 241, 0,   0,   0,   0,   0,   0,   0,  0, 0, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
                                                    {0,  0,  0,  0,  0,   0,   0,  0,  0,   0,   0,   0,   0,   0,   0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   128, 192, 192, 96,  112, 48,  112, 248, 254, 3,  62, 120, 200, 136, 12,  12,  4,   6, 3, 1, 28, 60, 102, 198, 135, 3, 3, 1, 1, 0, 0, 128, 128, 192, 192, 97,  99, 50, 54, 30, 28,  12,  0, 0, 0, 0, 0,   0,   0,   0,   128, 129, 131, 134, 204, 124, 6,   30, 240, 0,  0,   0,  0,   0,   0,   0,   0,   0,  0,   0,  0,  0,  0,  0,   0,   0,  0,  0, 0, 0, 0,   0,  0,  0,  0, 0, 0, 0, 0, 0, 0,   0,   0,   0,   0,   0,  0,   0,   0,  0,   0,   0,   0, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   128, 192, 96,  120, 92, 86, 215, 147, 177, 161,
                                                     96, 64, 64, 96, 240, 255, 31, 12, 12,  63,  231, 195, 192, 192, 64, 192, 192, 192, 192, 192, 192, 193, 131, 131, 134, 134, 130, 131, 131, 129, 129, 128, 128, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 128, 128, 143, 27,  17, 48, 24,  24,  28,  12,  255, 126, 0, 0, 0, 0,  0,  0,   0,   0,   0, 0, 0, 0, 0, 0, 0,   0,   0,   0,   0,   0,  0,  0,  0,  0,   0,   0, 0, 0, 0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,   0,  0,   0,  128, 128, 192, 192, 224, 96, 112, 48, 24, 24, 12, 12,  6,   6,  3,  1, 0, 0, 0,   0,  0,  0,  0, 0, 1, 1, 1, 1, 249, 255, 3,   128, 192, 96, 188, 111, 39, 179, 230, 100, 8, 56, 121, 105, 108, 196, 196, 196, 132, 132, 132, 132, 132, 132, 196, 196, 196, 196, 196, 108, 120, 48,  17, 17, 25,  24,  8,   8,
                                                     13, 13, 7,  7,  7,   15,  26, 56, 224, 224, 224, 248, 127, 31,  6,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,   128, 224, 112, 56,  12,  6, 3, 3, 1,  0,  0,   0,   0,   0, 0, 0, 0, 0, 0, 0,   0,   128, 224, 120, 24, 24, 16, 24, 248, 224, 0, 0, 0, 0, 254, 159, 100, 254, 251, 252, 247, 220, 190, 247, 227, 64, 32,  96, 192, 96, 0,   64,  128, 128, 128, 1,  1,   1,  1,  1,  1,  128, 192, 64, 64, 0, 0, 0, 128, 64, 64, 64, 0, 0, 0, 0, 0, 0, 0,   0,   156, 254, 254, 7,  1,   1,   0,  0,   0,   0,   0, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,   0,   0}};

static const char PROGMEM appa_tap[][ANIM_SIZE] = {{0,  0,   0,   0,   0,   0,   0,   0,  0,  0,   0,   0,   0,   0,   0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   128, 192, 192, 96,  112, 48,  112, 248, 254, 3,  62, 120, 200, 136, 12,  12,  4,   6, 3, 1, 28, 60, 102, 198, 135, 3, 3, 1, 1, 0, 0, 128, 128, 192, 192, 97,  99, 50, 54, 30, 28,  12,  0, 0, 0, 0, 0,   0,   0,   0,   128, 129, 131, 134, 204, 124, 6,  30, 240, 0,   0,  0,  0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,   0, 0, 0, 0, 0, 0, 0,   0,   0,   0,   0,   0,  0,   0,   0,   0,   0,   0, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   128, 192, 96,  120, 92, 86, 215, 147, 177, 161,
                                                    96, 64,  64,  96,  240, 255, 31,  12, 12, 63,  231, 195, 192, 192, 64, 192, 192, 192, 192, 192, 192, 193, 131, 131, 134, 134, 130, 131, 131, 129, 129, 128, 128, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 128, 128, 143, 27,  17, 48, 24,  24,  28,  12,  255, 126, 0, 0, 0, 0,  0,  0,   0,   0,   0, 0, 0, 0, 0, 0, 0,   0,   0,   0,   0,   0,  0,  0,  0,  0,   0,   0, 0, 0, 0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,   0,   0,  0,  128, 128, 192, 192, 224, 96,  112, 48, 24, 24, 12, 12, 6, 6, 3, 1, 0, 0, 0, 0, 0,  0,  0,   0, 1, 1, 1, 1, 1, 255, 3,   128, 128, 248, 28, 135, 119, 115, 214, 196, 8, 56, 121, 105, 108, 196, 196, 196, 132, 132, 132, 132, 132, 132, 196, 196, 196, 196, 196, 108, 120, 48,  17, 17, 25,  24,  8,   8,
                                                    77, 173, 167, 135, 71,  143, 138, 24, 24, 240, 240, 248, 63,  31,  6,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,   128, 224, 112, 56,  12,  6, 3, 3, 1,  0,  0,   0,   0,   0, 0, 0, 0, 0, 0, 0,   0,   128, 224, 120, 24, 24, 16, 24, 248, 224, 0, 0, 0, 0, 252, 255, 207, 193, 65,  202, 153, 253, 255, 14,  23, 3,  225, 160, 64, 64, 128, 128, 192, 224, 192, 129, 1,   1,  1,  1,  1,  0,  0, 0, 0, 0, 0, 0, 0, 0, 64, 64, 224, 0, 0, 0, 0, 0, 0, 3,   186, 253, 239, 199, 3,  1,   0,   0,   0,   0,   0, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,   0,   0},
                                                   {0,   0,   0,  0,   0,   0,   0,  0,  0,   0,   0,   0,   0,   0,   0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   128, 192, 192, 96,  112, 48,  112, 248, 254, 3,  62, 120, 200, 136, 12,  12,  4,   6, 3, 1, 28, 60, 102, 198, 135, 3, 3, 1, 1, 0, 0, 128, 128, 192, 192, 97,  99, 50, 54, 30, 28,  12,  0, 0, 0, 0, 0,   0,   0,   0,   128, 129, 131, 134, 204, 124, 6,  30, 240, 0,  0,  0, 0,   0,   0,   0,   0,   0,  0,   0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   128, 192, 96,  120, 92, 86, 215, 147, 177, 161,
                                                    96,  64,  64, 96,  240, 255, 31, 12, 12,  63,  231, 195, 192, 192, 64, 192, 192, 192, 192, 192, 192, 193, 131, 131, 134, 134, 130, 131, 131, 129, 129, 128, 128, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 128, 128, 143, 27,  17, 48, 24,  24,  28,  12,  255, 126, 0, 0, 0, 0,  0,  0,   0,   0,   0, 0, 0, 0, 0, 0, 0,   0,   0,   0,   0,   0,  0,  0,  0,  0,   0,   0, 0, 0, 0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,   0,  0,  0, 128, 128, 192, 192, 224, 96, 112, 48, 24, 24, 12, 12, 6, 6, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 255, 195, 224, 112, 248, 236, 103, 119, 243, 246, 212, 8, 56, 121, 105, 108, 196, 196, 196, 132, 132, 132, 132, 132, 132, 196, 196, 196, 196, 196, 108, 120, 48,  17, 17, 25,  24,  8,   8,
                                                    109, 173, 47, 135, 71,  143, 26, 24, 112, 224, 248, 248, 63,  31,  6,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,   128, 224, 112, 56,  12,  6, 3, 3, 1,  0,  0,   0,   0,   0, 0, 0, 0, 0, 0, 0,   0,   128, 224, 120, 24, 24, 16, 24, 248, 224, 0, 0, 0, 0, 252, 255, 199, 129, 242, 193, 28,  254, 254, 44,  63, 55, 224, 96, 64, 0, 128, 128, 192, 224, 192, 1,  1,   1,  1,  1,  1,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,   2,   189, 254, 231, 3,   1,   0,   0,   0,   0,   0, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,   0,   0}};

/**
 * Custom Functions
 */
uint32_t set_timer(void) {
    anim_timer = timer_read32();
    return anim_timer;
}

void print_frame(uint8_t *curr_frame, const char frames[][ANIM_SIZE], const uint8_t *framenum, bool reverse) {
    *curr_frame = *curr_frame % *framenum;
    oled_write_raw_P(frames[(reverse ? abs((*framenum - 1) - *curr_frame) : *curr_frame)], ANIM_SIZE);
    *curr_frame += 1;
}

/**
 * Declaration of animation after functions and parameters are all initialized
 */
static const animation_details a_animation = {3, 4, 2, 150};

// animations
void animate_dynamic(void) {
    set_timer();
    if (get_current_wpm() < TAP_SPEED) {
        if (prepped) {
            print_frame(&curr_prep, appa_prep, &a_animation.prep, true);
            prepped = curr_prep != a_animation.prep;
        } else {
            print_frame(&curr_idle, appa_idle, &a_animation.idle, false);
        }
        curr_tap = 0;
    } else {
        if ((!prepped)) {
            print_frame(&curr_prep, appa_prep, &a_animation.prep, false);
            prep_wait = prepped = curr_prep == a_animation.prep;
        } else {
            print_frame(&curr_tap, appa_tap, &a_animation.tap, false);
        }
        curr_idle = 0;
    }
}

/**
 * Extended core functions
 */
void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!is_oled_on()) {
        oled_on();
    }
}

bool oled_task_kb(void) {
    if (timer_elapsed32(anim_sleep) > OLED_TIMEOUT) {
        oled_off();
        return false;
    }

    if (get_current_wpm() == 0 && timer_elapsed32(anim_timer) > a_animation.frame_time) {
        animate_dynamic();
    } else if (get_current_wpm() > 0 && timer_elapsed32(anim_timer) > (((prepped && prep_wait) ? WAIT_MULTIPLIER : 1) * a_animation.frame_time)) {
        prep_wait = false;
        animate_dynamic();
    }

    if (host_keyboard_led_state().caps_lock) {
        oled_write_P(PSTR("Caps"), false);
    }

    return false;
}
#endif