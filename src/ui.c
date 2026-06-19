#include "ui.h"

#include <math.h>
#include <stdio.h>

#include "constants.h"
#include "screen.h"

Vector2 ui_slot_b_center(void) {
    return (Vector2){
        scr_w() * 0.72f,
        slot_y() + slot_h() * 0.5f
    };
}

void ui_get_home(HomeUI *ui, int screen_w, int screen_h) {
    float scale = ui_font_scale();
    float btn_w = 220.0f * scale;
    float btn_h = 64.0f * scale;
    ui->play_btn = (Rectangle){
        (screen_w - btn_w) * 0.5f,
        screen_h * 0.55f,
        btn_w,
        btn_h
    };
}

Vector2 ui_get_pointer(void) {
    if (GetTouchPointCount() > 0) return GetTouchPosition(0);
    return GetMousePosition();
}

bool ui_pointer_pressed(void) {
    return IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

bool ui_home_handle_input(const HomeUI *ui, Vector2 pointer, bool pressed) {
    if (!pressed) return false;
    return CheckCollisionPointRec(pointer, ui->play_btn);
}

static void draw_centered_text(Font font, const char *text, float cx, float cy, float size, Color color) {
    Vector2 dims = MeasureTextEx(font, text, size, 2.0f);
    DrawTextEx(font, text, (Vector2){ cx - dims.x * 0.5f, cy - dims.y * 0.5f }, size, 2.0f, color);
}

void ui_draw_home(const HomeUI *ui, Font font, int high_score) {
    int sw = scr_w();
    int sh = scr_h();
    float scale = ui_font_scale();

    ClearBackground(COLOR_BG);

    draw_centered_text(font, "Make 10!", sw * 0.5f, sh * 0.28f, 52.0f * scale, COLOR_TITLE);
    draw_centered_text(font, "Find the number in the pool!", sw * 0.5f, sh * 0.38f, 20.0f * scale, COLOR_SCORE);

    Vector2 pointer = ui_get_pointer();
    bool hover = CheckCollisionPointRec(pointer, ui->play_btn);
    Color btn_color = hover ? COLOR_PLAY_HOVER : COLOR_PLAY_BTN;
    DrawRectangleRounded(ui->play_btn, 0.35f, 12, btn_color);
    DrawRectangleRoundedLines(ui->play_btn, 0.35f, 12, (Color){ 255, 255, 255, 120 });
    draw_centered_text(font, "PLAY", ui->play_btn.x + ui->play_btn.width * 0.5f,
                       ui->play_btn.y + ui->play_btn.height * 0.5f, 32.0f * scale, (Color){ 255, 255, 255, 255 });

    if (high_score > 0) {
        char buf[32];
        snprintf(buf, sizeof(buf), "Best: %d", high_score);
        draw_centered_text(font, buf, sw * 0.5f, sh * 0.72f, 22.0f * scale, COLOR_SCORE);
    }
}

static void draw_slot(float cx, float cy, float w, float h, int value, bool filled, bool is_target,
                      float shake_x, float pop_scale, Font font, const char *label) {
    float scale = pop_scale > 0.0f ? pop_scale : 1.0f;
    float sw = w * scale;
    float sh = h * scale;
    float font_size = h * 0.42f * scale;
    float label_size = 14.0f * ui_font_scale();
    float roundness = 0.28f;

    Rectangle outer = {
        cx - sw * 0.5f + shake_x,
        cy - sh * 0.5f,
        sw,
        sh
    };

    Color face = is_target ? COLOR_SLOT_A : COLOR_SLOT_B;
    Color ring = is_target ? (Color){ 90, 150, 220, 255 } : (Color){ 220, 170, 120, 255 };

    DrawTextEx(font, label, (Vector2){ outer.x + outer.width * 0.5f - MeasureTextEx(font, label, label_size, 1.0f).x * 0.5f,
                                       outer.y - label_size - 6.0f }, label_size, 1.0f, COLOR_SCORE);

    DrawRectangleRounded((Rectangle){ outer.x + 4, outer.y + 5, outer.width, outer.height }, roundness, 16, (Color){ 0, 0, 0, 30 });
    DrawRectangleRounded(outer, roundness, 16, face);
    DrawRectangleRoundedLines(outer, roundness, 16, COLOR_SLOT_BORDER);
    DrawRectangleRoundedLines((Rectangle){ outer.x + 3, outer.y + 3, outer.width - 6, outer.height - 6 },
                              roundness, 12, ring);

    Rectangle inner = {
        outer.x + sw * 0.12f,
        outer.y + sh * 0.18f,
        sw * 0.76f,
        sh * 0.64f
    };
    DrawRectangleRounded(inner, 0.35f, 10, COLOR_SLOT_INNER);
    DrawRectangleRoundedLines(inner, 0.35f, 10, (Color){ 255, 255, 255, 100 });

    float hole_cx = inner.x + inner.width * 0.5f;
    float hole_cy = inner.y + inner.height * 0.55f;
    float hole_r = inner.height * 0.32f;
    DrawCircle((int)hole_cx, (int)hole_cy, hole_r, (Color){ 200, 210, 225, 120 });
    DrawCircleLines((int)hole_cx, (int)hole_cy, hole_r, (Color){ 160, 180, 200, 160 });

    if (filled && value >= 1) {
        char buf[4];
        snprintf(buf, sizeof(buf), "%d", value);
        draw_centered_text(font, buf, hole_cx, hole_cy, font_size, COLOR_TEXT);
    } else {
        draw_centered_text(font, "?", hole_cx, hole_cy, font_size * 0.8f, (Color){ 170, 185, 200, 255 });
    }
}

void ui_draw_slots(int slot_a, int slot_b, float shake_offset, float pop_scale, Font font) {
    int sw = scr_w();
    float cy = slot_y() + slot_h() * 0.5f;
    float left_cx = sw * 0.28f;
    float right_cx = sw * 0.72f;
    float w = slot_w();
    float h = slot_h();
    float mid_y = cy;
    float scale = ui_font_scale();

    draw_slot(left_cx, cy, w, h, slot_a, true, true, 0.0f, pop_scale, font, "Target");
    draw_slot(right_cx, cy, w, h, slot_b, slot_b >= 1, false, shake_offset, pop_scale, font, "Your pick");

    DrawCircle((int)(sw * 0.5f), (int)mid_y, 22.0f * scale, (Color){ 255, 255, 255, 230 });
    DrawCircleLines((int)(sw * 0.5f), (int)mid_y, 22.0f * scale, COLOR_SLOT_BORDER);
    draw_centered_text(font, "+", sw * 0.5f, mid_y, 28.0f * scale, COLOR_SCORE);

    draw_centered_text(font, "= 10", sw * 0.5f, cy + h * 0.5f + 18.0f * scale, 16.0f * scale, (Color){ 120, 150, 180, 255 });
}

void ui_draw_score(int score, Font font) {
    float y = ui_top_margin();
    char buf[32];
    snprintf(buf, sizeof(buf), "Score: %d", score);
    DrawTextEx(font, buf, (Vector2){ 24.0f, y }, 24.0f * ui_font_scale(), 2.0f, COLOR_SCORE);
}

void ui_draw_timer(float time_left, float max_time, Font font) {
    int sw = scr_w();
    float scale = ui_font_scale();
    float bar_w = sw * 0.32f;
    float bar_h = 14.0f * scale;
    float x = sw - bar_w - 24.0f;
    float y = ui_top_margin() + 4.0f;

    float pct = time_left / max_time;
    if (pct < 0.0f) pct = 0.0f;
    if (pct > 1.0f) pct = 1.0f;

    Color fill = pct < 0.25f ? COLOR_TIMER_LOW : COLOR_TIMER;

    DrawRectangleRounded((Rectangle){ x, y, bar_w, bar_h }, 0.5f, 8, (Color){ 220, 230, 240, 255 });
    if (pct > 0.01f) {
        DrawRectangleRounded((Rectangle){ x, y, bar_w * pct, bar_h }, 0.5f, 8, fill);
    }
    DrawRectangleRoundedLines((Rectangle){ x, y, bar_w, bar_h }, 0.5f, 8, COLOR_SLOT_BORDER);

    char buf[16];
    snprintf(buf, sizeof(buf), "%.0fs", time_left < 0.0f ? 0.0f : time_left);
    DrawTextEx(font, buf, (Vector2){ x + bar_w * 0.5f - 18.0f * scale, y - 22.0f * scale }, 18.0f * scale, 2.0f, fill);
}

void ui_draw_flying_bubble(float x, float y, float radius, int value, Color fill, Font font) {
    Color shadow = (Color){ fill.r - 20, fill.g - 20, fill.b - 20, 100 };
    DrawCircle((int)x + 2, (int)y + 3, radius, shadow);
    DrawCircle((int)x, (int)y, radius, fill);
    DrawCircleLines((int)x, (int)y, radius, (Color){ 255, 255, 255, 200 });

    char buf[4];
    snprintf(buf, sizeof(buf), "%d", value);
    draw_centered_text(font, buf, x, y, bubble_font_size(), COLOR_TEXT);
}

void ui_draw_try_again(Font font) {
    draw_centered_text(font, "Try again!", scr_w() * 0.5f, play_top() - 20.0f, 26.0f * ui_font_scale(), (Color){ 220, 80, 80, 255 });
}

void ui_draw_success_flash(float alpha) {
    if (alpha <= 0.0f) return;
    Color c = COLOR_SUCCESS;
    c.a = (unsigned char)(alpha * 255.0f);
    DrawRectangle(0, 0, scr_w(), scr_h(), c);
}

void ui_draw_wrong_flash(float alpha) {
    if (alpha <= 0.0f) return;
    Color c = COLOR_WRONG;
    c.a = (unsigned char)(alpha * 255.0f);
    DrawRectangle(0, 0, scr_w(), scr_h(), c);
}

static float rand_range_local(float lo, float hi) {
    return lo + (float)GetRandomValue(0, 10000) / 10000.0f * (hi - lo);
}

void ui_spawn_burst(Particle particles[], int count, float x, float y, Color base, bool success) {
    for (int i = 0; i < count; i++) particles[i].active = false;

    int n = success ? 16 : 10;
    if (n > count) n = count;

    for (int i = 0; i < n; i++) {
        float angle = (6.2831853f * (float)i / (float)n) + rand_range_local(0.0f, 0.5f);
        float speed = success ? rand_range_local(120.0f, 260.0f) : rand_range_local(80.0f, 180.0f);
        particles[i].x = x;
        particles[i].y = y;
        particles[i].vx = cosf(angle) * speed;
        particles[i].vy = sinf(angle) * speed;
        particles[i].life = success ? 0.7f : 0.5f;
        particles[i].radius = success ? rand_range_local(4.0f, 9.0f) : rand_range_local(3.0f, 7.0f);
        particles[i].color = base;
        particles[i].active = true;
    }
}

void ui_draw_particles(const Particle particles[], int count) {
    for (int i = 0; i < count; i++) {
        if (!particles[i].active) continue;
        Color c = particles[i].color;
        c.a = (unsigned char)(particles[i].life * 255.0f);
        DrawCircle((int)particles[i].x, (int)particles[i].y, particles[i].radius, c);
    }
}

void ui_draw_game_over(int score, int high_score, Font font) {
    int sw = scr_w();
    int sh = scr_h();
    float scale = ui_font_scale();

    DrawRectangle(0, 0, sw, sh, (Color){ 30, 40, 60, 160 });
    draw_centered_text(font, "Time's up!", sw * 0.5f, sh * 0.32f, 48.0f * scale, (Color){ 255, 255, 255, 255 });

    char buf[32];
    snprintf(buf, sizeof(buf), "Score: %d", score);
    draw_centered_text(font, buf, sw * 0.5f, sh * 0.45f, 36.0f * scale, (Color){ 200, 230, 255, 255 });

    if (high_score > 0) {
        snprintf(buf, sizeof(buf), "Best: %d", high_score);
        draw_centered_text(font, buf, sw * 0.5f, sh * 0.54f, 24.0f * scale, COLOR_SCORE);
    }

    draw_centered_text(font, "Click to return home", sw * 0.5f, sh * 0.68f, 20.0f * scale, (Color){ 180, 200, 220, 255 });
}
