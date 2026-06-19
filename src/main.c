#include "raylib.h"

#include "constants.h"
#include "game.h"
#include "save.h"
#include "ui.h"

typedef enum {
    STATE_HOME,
    STATE_PLAYING
} AppState;

int main(void) {
    int monitor = GetCurrentMonitor();
    int win_w = GetMonitorWidth(monitor);
    int win_h = GetMonitorHeight(monitor);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(win_w, win_h, "Make 10");
    SetTargetFPS(TARGET_FPS);

    Font font = GetFontDefault();

    AppState app = STATE_HOME;
    HomeUI home_ui;
    GameState game;

    int high_score = save_read_high_score();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        Vector2 pointer = ui_get_pointer();
        bool pressed = ui_pointer_pressed();
        int sw = GetScreenWidth();
        int sh = GetScreenHeight();

        if (app == STATE_HOME) {
            ui_get_home(&home_ui, sw, sh);
            if (ui_home_handle_input(&home_ui, pointer, pressed)) {
                game_init(&game);
                app = STATE_PLAYING;
            }
        } else {
            if (game_wants_home(&game, pointer, pressed)) {
                app = STATE_HOME;
            } else {
                game_handle_input(&game, pointer, pressed);
                game_update(&game, dt);
            }
            high_score = game.high_score;
        }

        BeginDrawing();
        if (app == STATE_HOME) {
            ui_draw_home(&home_ui, font, high_score);
        } else {
            game_draw(&game, font);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
