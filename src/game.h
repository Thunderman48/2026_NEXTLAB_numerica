#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include <stdbool.h>

#include "bubble.h"
#include "ui.h"

typedef enum {
    GAME_PHASE_PLAYING,
    GAME_PHASE_FLYING_TO_SLOT,
    GAME_PHASE_FLYING_BACK,
    GAME_PHASE_CORRECT,
    GAME_PHASE_WRONG,
    GAME_PHASE_OVER
} GamePhase;

typedef struct {
    int slot_a;
    int slot_b;
    int score;
    int high_score;
    int pool_count;
    int selected_bubble;
    Bubble bubbles[MAX_BUBBLES];
    Particle particles[MAX_PARTICLES];
    GamePhase phase;
    float time_left;
    float max_time;
    float spawn_timer;
    float shake_timer;
    float shake_offset;
    float effect_timer;
    float pop_scale;
    float wrong_flash;
    float fly_t;
    float fly_from_x, fly_from_y, fly_to_x, fly_to_y;
    float fly_home_x, fly_home_y;
    float fly_radius;
    int fly_value;
    Color fly_color;
    bool input_locked;
} GameState;

void game_init(GameState *gs);
void game_new_target(GameState *gs);
void game_update(GameState *gs, float dt);
void game_draw(const GameState *gs, Font font);
void game_handle_input(GameState *gs, Vector2 pointer, bool pressed);
bool game_is_over(const GameState *gs);
bool game_wants_home(const GameState *gs, Vector2 pointer, bool pressed);

#endif
