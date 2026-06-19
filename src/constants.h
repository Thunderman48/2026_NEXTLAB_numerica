#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "raylib.h"

#define TARGET_FPS 30

#define MAX_BUBBLES 30
#define MAX_PARTICLES 32

#define BUBBLE_MIN_SPEED 45.0f
#define BUBBLE_MAX_SPEED 95.0f

#define GAME_START_TIME 90.0f
#define TIME_BONUS_CORRECT 2.0f
#define TIME_PENALTY_WRONG 4.0f
#define CORRECT_EFFECT_DURATION 0.28f
#define FLY_DURATION 0.32f
#define SPAWN_INTERVAL 1.2f
#define MIN_BUBBLES_START 8

#define COLOR_BG        (Color){ 232, 244, 252, 255 }
#define COLOR_SLOT_A    (Color){ 235, 245, 255, 255 }
#define COLOR_SLOT_B    (Color){ 255, 248, 240, 255 }
#define COLOR_SLOT_INNER (Color){ 218, 228, 240, 255 }
#define COLOR_SLOT_BORDER (Color){ 140, 170, 210, 255 }
#define COLOR_SLOT_RING (Color){ 100, 140, 190, 180 }
#define COLOR_TEXT      (Color){  60,  70,  90, 255 }
#define COLOR_TITLE     (Color){  80, 120, 180, 255 }
#define COLOR_PLAY_BTN  (Color){ 120, 200, 160, 255 }
#define COLOR_PLAY_HOVER (Color){ 100, 180, 140, 255 }
#define COLOR_SUCCESS   (Color){ 160, 230, 170, 120 }
#define COLOR_WRONG     (Color){ 240, 140, 140, 130 }
#define COLOR_SCORE     (Color){  90, 110, 140, 255 }
#define COLOR_TIMER     (Color){ 100, 160, 220, 255 }
#define COLOR_TIMER_LOW (Color){ 230, 100, 100, 255 }

static const Color BUBBLE_COLORS[] = {
    { 180, 230, 210, 255 },
    { 255, 210, 190, 255 },
    { 210, 195, 240, 255 },
    { 255, 235, 170, 255 },
    { 190, 220, 255, 255 },
    { 255, 195, 210, 255 },
    { 220, 240, 190, 255 },
    { 240, 200, 230, 255 },
    { 200, 230, 255, 255 },
    { 255, 220, 200, 255 },
};
#define BUBBLE_COLOR_COUNT 10

#endif
