#include "bubble.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "screen.h"

static float rand_range(float lo, float hi) {
    return lo + (float)GetRandomValue(0, 10000) / 10000.0f * (hi - lo);
}

static int clamp_answer(int slot_a) {
    int answer = 10 - slot_a;
    if (answer < 1) return 1;
    if (answer > 9) return 9;
    return answer;
}

static bool overlaps_existing(const Bubble bubbles[MAX_BUBBLES], int pool_count, float x, float y, float radius, int skip) {
    for (int i = 0; i < pool_count; i++) {
        if (i == skip || !bubbles[i].active) continue;
        float dx = x - bubbles[i].x;
        float dy = y - bubbles[i].y;
        float min_dist = radius + bubbles[i].radius + 16.0f;
        if (dx * dx + dy * dy < min_dist * min_dist) return true;
    }
    return false;
}

static void place_bubble(Bubble *b, const Bubble bubbles[MAX_BUBBLES], int pool_count, int index, int play_top, int screen_w, int screen_h) {
    float margin = b->radius + 10.0f;
    float min_x = margin;
    float max_x = (float)screen_w - margin;
    float min_y = (float)play_top + margin;
    float max_y = (float)screen_h - margin;

    for (int attempt = 0; attempt < 60; attempt++) {
        float x = rand_range(min_x, max_x);
        float y = rand_range(min_y, max_y);
        if (!overlaps_existing(bubbles, pool_count, x, y, b->radius, index)) {
            b->x = x;
            b->y = y;
            return;
        }
    }
    b->x = rand_range(min_x, max_x);
    b->y = rand_range(min_y, max_y);
}

static void give_velocity(Bubble *b) {
    float speed = rand_range(BUBBLE_MIN_SPEED, BUBBLE_MAX_SPEED);
    float angle = rand_range(0.0f, 6.2831853f);
    b->vx = cosf(angle) * speed;
    b->vy = sinf(angle) * speed;
}

bool bubbles_has_answer(const Bubble bubbles[MAX_BUBBLES], int pool_count, int slot_a) {
    int answer = clamp_answer(slot_a);
    for (int i = 0; i < pool_count; i++) {
        if (bubbles[i].active && !bubbles[i].flying && bubbles[i].value == answer) return true;
    }
    return false;
}

int bubbles_pick_spawn_value(const Bubble bubbles[MAX_BUBBLES], int pool_count, int slot_a) {
    if (!bubbles_has_answer(bubbles, pool_count, slot_a)) return clamp_answer(slot_a);
    int answer = clamp_answer(slot_a);
    int v;
    do {
        v = GetRandomValue(1, 9);
    } while (v == answer);
    return v;
}

void bubbles_ensure_answer(Bubble bubbles[MAX_BUBBLES], int pool_count, int slot_a) {
    if (bubbles_has_answer(bubbles, pool_count, slot_a)) return;

    int candidates[MAX_BUBBLES];
    int n = 0;
    for (int i = 0; i < pool_count; i++) {
        if (bubbles[i].active && !bubbles[i].flying && !bubbles[i].picked) candidates[n++] = i;
    }
    if (n == 0) return;

    int idx = candidates[GetRandomValue(0, n - 1)];
    bubbles[idx].value = clamp_answer(slot_a);
}

int bubbles_active_count(const Bubble bubbles[MAX_BUBBLES], int pool_count) {
    int count = 0;
    for (int i = 0; i < pool_count; i++) {
        if (bubbles[i].active) count++;
    }
    return count;
}

int bubbles_find_free_slot(const Bubble bubbles[MAX_BUBBLES], int pool_count) {
    for (int i = 0; i < pool_count; i++) {
        if (!bubbles[i].active) return i;
    }
    return -1;
}

bool bubbles_spawn_one(Bubble bubbles[MAX_BUBBLES], int pool_count, int index, int value, int play_top, int screen_w, int screen_h) {
    if (index < 0 || index >= pool_count) return false;

    Bubble *b = &bubbles[index];
    b->radius = bubble_radius();
    b->value = value;
    b->picked = false;
    b->flying = false;
    b->active = true;
    place_bubble(b, bubbles, pool_count, index, play_top, screen_w, screen_h);
    give_velocity(b);
    return true;
}

void bubbles_update_spawning(Bubble bubbles[MAX_BUBBLES], int pool_count, int slot_a, float *spawn_timer, float dt, int play_top, int screen_w, int screen_h) {
    int target = bubble_pool_count();
    int active = bubbles_active_count(bubbles, pool_count);

    *spawn_timer -= dt;
    if (*spawn_timer > 0.0f || active >= target) return;

    int slot = bubbles_find_free_slot(bubbles, pool_count);
    if (slot < 0) return;

    int value = bubbles_pick_spawn_value(bubbles, pool_count, slot_a);
    bubbles_spawn_one(bubbles, pool_count, slot, value, play_top, screen_w, screen_h);
    *spawn_timer = SPAWN_INTERVAL;
}

void bubbles_init_pool(Bubble bubbles[MAX_BUBBLES], int slot_a, int play_top, int screen_w, int screen_h) {
    for (int i = 0; i < MAX_BUBBLES; i++) {
        bubbles[i].active = false;
        bubbles[i].picked = false;
        bubbles[i].flying = false;
    }

    int pool_count = bubble_pool_count();
    int start = MIN_BUBBLES_START;
    if (start > pool_count) start = pool_count;

    for (int i = 0; i < start; i++) {
        int value = bubbles_pick_spawn_value(bubbles, pool_count, slot_a);
        bubbles_spawn_one(bubbles, pool_count, i, value, play_top, screen_w, screen_h);
    }
    bubbles_ensure_answer(bubbles, pool_count, slot_a);
}

static void resolve_collision(Bubble *a, Bubble *b) {
    float dx = b->x - a->x;
    float dy = b->y - a->y;
    float dist_sq = dx * dx + dy * dy;
    float min_dist = a->radius + b->radius;

    if (dist_sq >= min_dist * min_dist || dist_sq < 0.0001f) return;

    float dist = sqrtf(dist_sq);
    float nx = dx / dist;
    float ny = dy / dist;

    float overlap = min_dist - dist;
    a->x -= nx * overlap * 0.5f;
    a->y -= ny * overlap * 0.5f;
    b->x += nx * overlap * 0.5f;
    b->y += ny * overlap * 0.5f;

    float dvx = b->vx - a->vx;
    float dvy = b->vy - a->vy;
    float vel_n = dvx * nx + dvy * ny;
    if (vel_n >= 0.0f) return;

    a->vx += vel_n * nx;
    a->vy += vel_n * ny;
    b->vx -= vel_n * nx;
    b->vy -= vel_n * ny;
}

void bubbles_update(Bubble bubbles[MAX_BUBBLES], int pool_count, float dt, int play_top, int screen_w, int screen_h) {
    float top = (float)play_top;
    float bottom = (float)screen_h;
    float left = 0.0f;
    float right = (float)screen_w;

    for (int i = 0; i < pool_count; i++) {
        if (!bubbles[i].active || bubbles[i].picked || bubbles[i].flying) continue;

        Bubble *b = &bubbles[i];
        b->x += b->vx * dt;
        b->y += b->vy * dt;

        if (b->x - b->radius < left) {
            b->x = left + b->radius;
            b->vx = fabsf(b->vx);
        } else if (b->x + b->radius > right) {
            b->x = right - b->radius;
            b->vx = -fabsf(b->vx);
        }

        if (b->y - b->radius < top) {
            b->y = top + b->radius;
            b->vy = fabsf(b->vy);
        } else if (b->y + b->radius > bottom) {
            b->y = bottom - b->radius;
            b->vy = -fabsf(b->vy);
        }
    }

    for (int i = 0; i < pool_count; i++) {
        if (!bubbles[i].active || bubbles[i].picked || bubbles[i].flying) continue;
        for (int j = i + 1; j < pool_count; j++) {
            if (!bubbles[j].active || bubbles[j].picked || bubbles[j].flying) continue;
            resolve_collision(&bubbles[i], &bubbles[j]);
        }
    }
}

int bubbles_hit(Bubble bubbles[MAX_BUBBLES], int pool_count, Vector2 point) {
    for (int i = 0; i < pool_count; i++) {
        if (!bubbles[i].active || bubbles[i].picked || bubbles[i].flying) continue;
        float dx = point.x - bubbles[i].x;
        float dy = point.y - bubbles[i].y;
        float r = bubbles[i].radius + 8.0f;
        if (dx * dx + dy * dy <= r * r) return i;
    }
    return -1;
}

static void draw_centered_text(Font font, const char *text, float cx, float cy, float size, Color color) {
    Vector2 dims = MeasureTextEx(font, text, size, 2.0f);
    DrawTextEx(font, text, (Vector2){ cx - dims.x * 0.5f, cy - dims.y * 0.5f }, size, 2.0f, color);
}

void bubbles_draw(const Bubble bubbles[MAX_BUBBLES], int pool_count, Font font) {
    float font_size = bubble_font_size();

    for (int i = 0; i < pool_count; i++) {
        if (!bubbles[i].active || bubbles[i].flying) continue;

        Color fill = BUBBLE_COLORS[i % BUBBLE_COLOR_COUNT];
        if (bubbles[i].picked) fill.a = 100;

        Color shadow = (Color){ fill.r - 20, fill.g - 20, fill.b - 20, 80 };
        DrawCircle((int)bubbles[i].x + 3, (int)bubbles[i].y + 4, bubbles[i].radius, shadow);
        DrawCircle((int)bubbles[i].x, (int)bubbles[i].y, bubbles[i].radius, fill);
        DrawCircleLines((int)bubbles[i].x, (int)bubbles[i].y, bubbles[i].radius,
                        bubbles[i].picked ? (Color){ 255, 200, 80, 180 } : (Color){ 255, 255, 255, 140 });

        char buf[4];
        snprintf(buf, sizeof(buf), "%d", bubbles[i].value);
        draw_centered_text(font, buf, bubbles[i].x, bubbles[i].y, font_size, COLOR_TEXT);
    }
}
