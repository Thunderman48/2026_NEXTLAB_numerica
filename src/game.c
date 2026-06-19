#include "game.h"

#include <math.h>

#include "constants.h"
#include "save.h"
#include "screen.h"

static float ease_out_cubic(float t) {
    float inv = 1.0f - t;
    return 1.0f - inv * inv * inv;
}

static void add_time(GameState *gs, float delta) {
    gs->time_left += delta;
    if (gs->time_left > GAME_START_TIME) gs->time_left = GAME_START_TIME;
}

static int pick_slot_a(int previous) {
    int n;
    do {
        n = GetRandomValue(1, 9);
    } while (n == previous);
    return n;
}

static void release_bubble(GameState *gs) {
    if (gs->selected_bubble >= 0) {
        gs->bubbles[gs->selected_bubble].picked = false;
        gs->bubbles[gs->selected_bubble].flying = false;
        gs->selected_bubble = -1;
    }
}

void game_init(GameState *gs) {
    gs->score = 0;
    gs->high_score = save_read_high_score();
    gs->phase = GAME_PHASE_PLAYING;
    gs->time_left = GAME_START_TIME;
    gs->max_time = GAME_START_TIME;
    gs->spawn_timer = 0.5f;
    gs->shake_timer = 0.0f;
    gs->shake_offset = 0.0f;
    gs->effect_timer = 0.0f;
    gs->pop_scale = 1.0f;
    gs->wrong_flash = 0.0f;
    gs->fly_t = 0.0f;
    gs->input_locked = false;
    gs->selected_bubble = -1;
    gs->slot_b = -1;
    gs->pool_count = MAX_BUBBLES;
    gs->slot_a = GetRandomValue(1, 9);

    for (int i = 0; i < MAX_PARTICLES; i++) gs->particles[i].active = false;

    bubbles_init_pool(gs->bubbles, gs->slot_a, (int)play_top(), scr_w(), scr_h());
}

void game_new_target(GameState *gs) {
    release_bubble(gs);
    gs->slot_a = pick_slot_a(gs->slot_a);
    gs->slot_b = -1;
    bubbles_ensure_answer(gs->bubbles, gs->pool_count, gs->slot_a);
}

static void start_fly_to_slot(GameState *gs, int idx) {
    Bubble *b = &gs->bubbles[idx];
    Vector2 target = ui_slot_b_center();

    gs->selected_bubble = idx;
    gs->fly_value = b->value;
    gs->fly_radius = b->radius;
    gs->fly_color = BUBBLE_COLORS[idx % BUBBLE_COLOR_COUNT];
    gs->fly_from_x = b->x;
    gs->fly_from_y = b->y;
    gs->fly_home_x = b->x;
    gs->fly_home_y = b->y;
    gs->fly_to_x = target.x;
    gs->fly_to_y = target.y;
    gs->fly_t = 0.0f;

    b->picked = true;
    b->flying = true;
    gs->phase = GAME_PHASE_FLYING_TO_SLOT;
    gs->input_locked = true;
}

static void start_fly_back(GameState *gs) {
    if (gs->selected_bubble < 0) return;

    gs->fly_from_x = ui_slot_b_center().x;
    gs->fly_from_y = ui_slot_b_center().y;
    gs->fly_to_x = gs->fly_home_x;
    gs->fly_to_y = gs->fly_home_y;
    gs->fly_t = 0.0f;
    gs->slot_b = -1;
    gs->phase = GAME_PHASE_FLYING_BACK;
}

static void on_fly_arrived(GameState *gs) {
    gs->slot_b = gs->fly_value;
    gs->pop_scale = 1.0f;

    if (gs->slot_a + gs->slot_b == 10) {
        gs->score += 10;
        if (gs->score > gs->high_score) {
            gs->high_score = gs->score;
            save_write_high_score(gs->high_score);
        }
        add_time(gs, TIME_BONUS_CORRECT);
        gs->effect_timer = CORRECT_EFFECT_DURATION;
        gs->pop_scale = 1.15f;
        gs->phase = GAME_PHASE_CORRECT;
        ui_spawn_burst(gs->particles, MAX_PARTICLES, ui_slot_b_center().x, ui_slot_b_center().y,
                       (Color){ 120, 220, 150, 255 }, true);
        if (gs->selected_bubble >= 0) {
            gs->bubbles[gs->selected_bubble].flying = false;
            gs->bubbles[gs->selected_bubble].picked = false;
            gs->selected_bubble = -1;
        }
        gs->slot_b = -1;
    } else {
        add_time(gs, -TIME_PENALTY_WRONG);
        gs->slot_b = gs->fly_value;
        gs->wrong_flash = 1.0f;
        gs->shake_timer = 0.45f;
        ui_spawn_burst(gs->particles, MAX_PARTICLES, ui_slot_b_center().x, ui_slot_b_center().y,
                       (Color){ 240, 120, 120, 255 }, false);
        start_fly_back(gs);
    }
}

static void on_wrong_done(GameState *gs) {
    if (gs->selected_bubble >= 0) {
        Bubble *b = &gs->bubbles[gs->selected_bubble];
        b->x = gs->fly_home_x;
        b->y = gs->fly_home_y;
        b->picked = false;
        b->flying = false;
        gs->selected_bubble = -1;
    }
    gs->slot_b = -1;
    gs->phase = GAME_PHASE_PLAYING;
    gs->input_locked = false;
}

static void on_correct_done(GameState *gs) {
    game_new_target(gs);
    gs->phase = GAME_PHASE_PLAYING;
    gs->input_locked = false;
}

static void update_particles(GameState *gs, float dt) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!gs->particles[i].active) continue;
        gs->particles[i].x += gs->particles[i].vx * dt;
        gs->particles[i].y += gs->particles[i].vy * dt;
        gs->particles[i].vy += 180.0f * dt;
        gs->particles[i].life -= dt;
        if (gs->particles[i].life <= 0.0f) gs->particles[i].active = false;
    }
}

static float fly_pos(float from, float to, float t) {
    return from + (to - from) * ease_out_cubic(t);
}

void game_handle_input(GameState *gs, Vector2 pointer, bool pressed) {
    if (gs->phase == GAME_PHASE_OVER) return;
    if (!pressed || gs->input_locked || gs->phase != GAME_PHASE_PLAYING) return;
    if (gs->slot_b >= 1) return;

    int idx = bubbles_hit(gs->bubbles, gs->pool_count, pointer);
    if (idx < 0) return;

    start_fly_to_slot(gs, idx);
}

void game_update(GameState *gs, float dt) {
    if (gs->phase == GAME_PHASE_OVER) return;

    if (gs->phase == GAME_PHASE_PLAYING || gs->phase == GAME_PHASE_FLYING_TO_SLOT ||
        gs->phase == GAME_PHASE_FLYING_BACK || gs->phase == GAME_PHASE_CORRECT) {
        gs->time_left -= dt;
        if (gs->time_left <= 0.0f) {
            gs->time_left = 0.0f;
            gs->phase = GAME_PHASE_OVER;
            return;
        }
    }

    bubbles_update_spawning(gs->bubbles, gs->pool_count, gs->slot_a, &gs->spawn_timer, dt,
                            (int)play_top(), scr_w(), scr_h());
    bubbles_update(gs->bubbles, gs->pool_count, dt, (int)play_top(), scr_w(), scr_h());
    bubbles_ensure_answer(gs->bubbles, gs->pool_count, gs->slot_a);
    update_particles(gs, dt);

    if (gs->phase == GAME_PHASE_FLYING_TO_SLOT || gs->phase == GAME_PHASE_FLYING_BACK) {
        gs->fly_t += dt / FLY_DURATION;
        if (gs->fly_t >= 1.0f) {
            gs->fly_t = 1.0f;
            if (gs->phase == GAME_PHASE_FLYING_TO_SLOT) {
                on_fly_arrived(gs);
            } else {
                on_wrong_done(gs);
            }
        }
    }

    if (gs->shake_timer > 0.0f) {
        gs->shake_timer -= dt;
        gs->shake_offset = sinf(gs->shake_timer * 50.0f) * 10.0f;
        if (gs->shake_timer <= 0.0f) gs->shake_offset = 0.0f;
    }

    if (gs->wrong_flash > 0.0f) {
        gs->wrong_flash -= dt * 2.5f;
        if (gs->wrong_flash < 0.0f) gs->wrong_flash = 0.0f;
    }

    if (gs->phase == GAME_PHASE_CORRECT) {
        gs->effect_timer -= dt;
        if (gs->pop_scale > 1.0f) {
            gs->pop_scale -= dt * 1.2f;
            if (gs->pop_scale < 1.0f) gs->pop_scale = 1.0f;
        }
        if (gs->effect_timer <= 0.0f) on_correct_done(gs);
    }
}

bool game_is_over(const GameState *gs) {
    return gs->phase == GAME_PHASE_OVER;
}

bool game_wants_home(const GameState *gs, Vector2 pointer, bool pressed) {
    if (gs->phase != GAME_PHASE_OVER || !pressed) return false;
    return true;
}

void game_draw(const GameState *gs, Font font) {
    ClearBackground(COLOR_BG);

    ui_draw_score(gs->score, font);
    ui_draw_timer(gs->time_left, gs->max_time, font);
    ui_draw_slots(gs->slot_a, gs->slot_b, gs->shake_offset, gs->pop_scale, font);

    bubbles_draw(gs->bubbles, gs->pool_count, font);

    if (gs->phase == GAME_PHASE_FLYING_TO_SLOT || gs->phase == GAME_PHASE_FLYING_BACK) {
        float x = fly_pos(gs->fly_from_x, gs->fly_to_x, gs->fly_t);
        float y = fly_pos(gs->fly_from_y, gs->fly_to_y, gs->fly_t);
        float r = gs->fly_radius * (1.0f + sinf(gs->fly_t * 3.14159f) * 0.08f);
        ui_draw_flying_bubble(x, y, r, gs->fly_value, gs->fly_color, font);
    }

    ui_draw_particles(gs->particles, MAX_PARTICLES);

    if (gs->phase == GAME_PHASE_FLYING_BACK) {
        ui_draw_try_again(font);
    }

    if (gs->phase == GAME_PHASE_CORRECT) {
        float alpha = gs->effect_timer / CORRECT_EFFECT_DURATION;
        ui_draw_success_flash(alpha);
    }

    if (gs->wrong_flash > 0.0f) {
        ui_draw_wrong_flash(gs->wrong_flash);
    }

    if (gs->phase == GAME_PHASE_OVER) {
        ui_draw_game_over(gs->score, gs->high_score, font);
    }
}
