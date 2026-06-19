#ifndef BUBBLE_H
#define BUBBLE_H

#include "raylib.h"
#include <stdbool.h>

#include "constants.h"

typedef struct {
    float x, y;
    float vx, vy;
    float radius;
    int value;
    bool active;
    bool picked;
    bool flying;
} Bubble;

void bubbles_init_pool(Bubble bubbles[MAX_BUBBLES], int slot_a, int play_top, int screen_w, int screen_h);
void bubbles_ensure_answer(Bubble bubbles[MAX_BUBBLES], int pool_count, int slot_a);
int bubbles_active_count(const Bubble bubbles[MAX_BUBBLES], int pool_count);
int bubbles_pick_spawn_value(const Bubble bubbles[MAX_BUBBLES], int pool_count, int slot_a);
bool bubbles_has_answer(const Bubble bubbles[MAX_BUBBLES], int pool_count, int slot_a);
int bubbles_find_free_slot(const Bubble bubbles[MAX_BUBBLES], int pool_count);
bool bubbles_spawn_one(Bubble bubbles[MAX_BUBBLES], int pool_count, int index, int value, int play_top, int screen_w, int screen_h);
void bubbles_update_spawning(Bubble bubbles[MAX_BUBBLES], int pool_count, int slot_a, float *spawn_timer, float dt, int play_top, int screen_w, int screen_h);
void bubbles_update(Bubble bubbles[MAX_BUBBLES], int pool_count, float dt, int play_top, int screen_w, int screen_h);
int bubbles_hit(Bubble bubbles[MAX_BUBBLES], int pool_count, Vector2 point);
void bubbles_draw(const Bubble bubbles[MAX_BUBBLES], int pool_count, Font font);

#endif
