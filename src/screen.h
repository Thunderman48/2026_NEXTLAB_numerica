#ifndef SCREEN_H
#define SCREEN_H

#include "raylib.h"

#include "constants.h"

static inline int scr_w(void) { return GetScreenWidth(); }
static inline int scr_h(void) { return GetScreenHeight(); }

static inline float ui_top_margin(void) {
    float m = (float)(scr_h() < scr_w() ? scr_h() : scr_w());
    return m * 0.055f;
}

static inline float slot_y(void) { return ui_top_margin() + (float)scr_h() * 0.06f; }
static inline float slot_w(void) { return (float)scr_w() * 0.18f; }
static inline float slot_h(void) { return (float)scr_h() * 0.11f; }
static inline float play_top(void) { return slot_y() + slot_h() + (float)scr_h() * 0.04f; }

static inline float ui_font_scale(void) {
    float m = (float)(scr_w() < scr_h() ? scr_w() : scr_h());
    return m / 800.0f;
}

static inline int bubble_pool_count(void) {
    int area = scr_w() * scr_h();
    int count;
    if (area >= 1500000) count = 30;
    else if (area >= 1000000) count = 24;
    else if (area >= 700000) count = 18;
    else count = 14;
    if (count > MAX_BUBBLES) count = MAX_BUBBLES;
    return count;
}

static inline float bubble_radius(void) {
    float m = (float)(scr_w() < scr_h() ? scr_w() : scr_h());
    int target = bubble_pool_count();
    float shrink = 1.0f;
    if (target > 24) shrink = 0.78f;
    else if (target > 18) shrink = 0.88f;
    return m * 0.028f * shrink;
}

static inline float bubble_font_size(void) { return bubble_radius() * 0.82f; }

#endif
