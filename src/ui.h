#ifndef UI_H
#define UI_H

#include "raylib.h"
#include <stdbool.h>

typedef struct {
    Rectangle play_btn;
} HomeUI;

typedef struct {
    float x, y, vx, vy, life, radius;
    Color color;
    bool active;
} Particle;

Vector2 ui_slot_b_center(void);

void ui_get_home(HomeUI *ui, int screen_w, int screen_h);
bool ui_home_handle_input(const HomeUI *ui, Vector2 pointer, bool pressed);
void ui_draw_home(const HomeUI *ui, Font font, int high_score);

void ui_draw_slots(int slot_a, int slot_b, float shake_offset, float pop_scale, Font font);
void ui_draw_score(int score, Font font);
void ui_draw_timer(float time_left, float max_time, Font font);
void ui_draw_flying_bubble(float x, float y, float radius, int value, Color fill, Font font);
void ui_draw_try_again(Font font);
void ui_draw_success_flash(float alpha);
void ui_draw_wrong_flash(float alpha);
void ui_draw_particles(const Particle particles[], int count);
void ui_spawn_burst(Particle particles[], int count, float x, float y, Color base, bool success);
void ui_draw_game_over(int score, int high_score, Font font);

Vector2 ui_get_pointer(void);
bool ui_pointer_pressed(void);

#endif
