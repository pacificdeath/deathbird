#define TEXTURE_PATH "./textures"
#define ATLAS_PNG_PATH "./atlas/atlas.png"
#define ATLAS_DATA_GENERATED_H_PATH "./atlas/atlas_data.generated.h"

#ifdef DEBUG
#define DEATHBIRD_LOG_LEVEL LOG_ALL
#else
#define DEATHBIRD_LOG_LEVEL LOG_WARNING
#endif

#ifdef ATLAS_GEN

#include "atlas_generator.h"

int main(void) {
    SetTraceLogLevel(DEATHBIRD_LOG_LEVEL);
    generate_atlas();
    return 0;
}

#else

#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

#include "raylib.h"

#include "../atlas/atlas_data.generated.h"

#ifdef DEBUG
    #include <stdio.h>
    #define ASSERT(condition)\
        do {\
            if (!(condition)) {\
                printf("You are a horrible person\n -> %s:%i\n", __FILE__, __LINE__);\
                exit(1);\
            }\
        } while (0)
#else
    #define ASSERT(condition)
#endif

#include "main.h"
#include "string_buffer.h"
#include "atlas.c"
#include "world.c"
#include "player.c"
#include "bird.c"
#include "cartoon_transition.c"
#include "ui.c"
#include "menu.c"

#include "console.c"

#define TIME_CHANGE_SPEED 2.0f

void game() {
    state->global_state = GLOBAL_STATE_GAME;
    if (state->area != AREA_FOREST) {
        setup_area(AREA_FOREST);
    }
    player_prepare();
    state->score = 0;
    update_world_by_score();
    destroy_all_birds();
}

void game_over() {
    state->global_state = GLOBAL_STATE_GAME_OVER;
    menu_prepare();
}

void prepare_cool_time_effect() {
    state->timer_state = TIMER_STATE_SLOW_DOWN;
}

bool cool_time_effect(float time_scale, float wait_time) {
    switch (state->timer_state) {
        default: return true;
        case TIMER_STATE_SLOW_DOWN:
            state->time_scale -= state->delta_time * TIME_CHANGE_SPEED;
            if (state->time_scale <= time_scale) {
                state->time_scale = time_scale;
                state->timer_state = TIMER_STATE_WAIT;
            }
            return false;
        case TIMER_STATE_WAIT:
            state->timer += (state->delta_time / state->time_scale);
            if (state->timer >= wait_time) {
                state->timer_state = TIMER_STATE_SPEED_UP;
                state->timer = 0.0f;
            }
            return false;
        case TIMER_STATE_SPEED_UP:
            state->time_scale += state->delta_time * TIME_CHANGE_SPEED;
            if (state->time_scale >= 1.0f) {
                state->time_scale = 1.0f;
                state->timer_state = TIMER_STATE_NONE;
                return true;
            }
            return false;
    }
}

int main(void) {
    state = (State *)calloc(1, sizeof(State));
    {
        SetTraceLogLevel(DEATHBIRD_LOG_LEVEL);
        SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
        int window_min_width = (int)AREA_TEXTURE_SIZE * (int)GAME_WIDTH_RATIO * GAME_MIN_SIZE;
        int window_min_height = (int)AREA_TEXTURE_SIZE * (int)GAME_HEIGHT_RATIO * GAME_MIN_SIZE;
        InitWindow(window_min_width, window_min_height, "Deathbird");
        SetWindowMinSize(window_min_width, window_min_height);
        SetTargetFPS(60);
        SetExitKey(KEY_NULL);
        HideCursor();

        InitAudioDevice();

        state->font = LoadFont(FONT);

        state->sounds_death_splats[0] = LoadSound("./sounds/splat1.wav");
        state->sounds_death_splats[1] = LoadSound("./sounds/splat2.wav");
        state->sounds_death_splats[2] = LoadSound("./sounds/splat3.wav");
        state->sounds_death_splats[3] = LoadSound("./sounds/splat4.wav");
        state->sounds_death_splats[4] = LoadSound("./sounds/splat5.wav");
        state->sounds_death_splats[5] = LoadSound("./sounds/splat6.wav");
        state->sounds_death_splats[6] = LoadSound("./sounds/splat7.wav");
        state->sounds_death_splats[7] = LoadSound("./sounds/splat8.wav");
        state->sound_explosion = LoadSound("./sounds/explosion.wav");

        state->time_scale = 1.0f;

        atlas_init();
        world_init();
        initialize_birds();
        console_init();
        cartoon_transition_init();
    }

    menu_prepare();

    while (!WindowShouldClose() && !has_flag(state->flags, FLAG_DEATHBIRD_BIG_QUIT)) {

        { // Frame-specific data
            state->frame_data = (FrameData){0};
            state->delta_time = GetFrameTime() * state->time_scale;

            bool should_enable_fullscreen = false;
            bool should_disable_fullscreen = false;

            if ((IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)) && IsKeyPressed(KEY_ENTER)) {
                // do not use raylib "ToggleFullscreen()", it does not work
                // they even say it in their window example
                if (has_flag(state->flags, FLAG_GAME_IS_FULLSCREEN)) {
                    should_disable_fullscreen = true;
                } else {
                    should_enable_fullscreen = true;
                }
            } else if (has_flag(state->flags, FLAG_GAME_IS_FULLSCREEN) && IsWindowResized()) {
                should_disable_fullscreen = true;
            }

            if (should_enable_fullscreen) {
                state->flags |= FLAG_GAME_IS_FULLSCREEN;

                state->cached_window_width = GetScreenWidth();
                state->cached_window_height = GetScreenHeight();

                SetWindowState(FLAG_WINDOW_UNDECORATED|FLAG_WINDOW_TOPMOST);
                int monitor = GetCurrentMonitor();
                SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
                SetWindowPosition(0, 0);
            } else if (should_disable_fullscreen) {
                state->flags &= ~FLAG_GAME_IS_FULLSCREEN;

                ClearWindowState(FLAG_WINDOW_UNDECORATED|FLAG_WINDOW_TOPMOST);
                SetWindowSize(state->cached_window_width, state->cached_window_height);
                int monitor = GetCurrentMonitor();
                SetWindowPosition(
                    GetMonitorWidth(monitor)/2 - state->cached_window_width/2,
                    GetMonitorHeight(monitor)/2 - state->cached_window_height/2
                );
            }

            float w;
            float h;

            if (has_flag(state->flags, FLAG_GAME_IS_FULLSCREEN)) {
                int monitor = GetCurrentMonitor();
                w = (float)GetMonitorWidth(monitor);
                h = (float)GetMonitorHeight(monitor);
            } else {
                w = (float)GetScreenWidth();
                h = (float)GetScreenHeight();
            }

            float padding_w;
            float padding_h;
            float ratio_unit;
            if ((w / GAME_WIDTH_RATIO) < (h / GAME_HEIGHT_RATIO)) {
                ratio_unit = roundf(w / GAME_WIDTH_RATIO);
                padding_w = 0;
                padding_h = h - (ratio_unit * GAME_HEIGHT_RATIO);
            } else {
                ratio_unit = roundf(h / GAME_HEIGHT_RATIO);
                padding_w = w - (ratio_unit * GAME_WIDTH_RATIO);
                padding_h = 0;
            }
            float half_padding_w = padding_w / 2;
            float half_padding_h = padding_h / 2;
            state->game_width = (int)lroundf(w - padding_w);
            state->game_height = (int)lroundf(h - padding_h);
            state->scale_multiplier = ratio_unit / AREA_TEXTURE_SIZE;
            state->game_left = (int)lroundf(half_padding_w);
            state->game_right = (int)lroundf(half_padding_w + (float)state->game_width);
            state->game_top = (int)lroundf(half_padding_h);
            state->game_bottom = (int)lroundf(half_padding_h + (float)state->game_height);
            state->game_center_x = (int)lroundf(half_padding_w + ((float)state->game_width / 2));
            state->game_center_y = (int)lroundf(half_padding_h + ((float)state->game_height / 2));
        }

        state->font_size = (FONT_SIZE / state->font.baseSize) * state->scale_multiplier;

        console_update();

        switch (state->global_state) {
            case GLOBAL_STATE_DEFAULT:
                world_update();
                update_birds();
                if (!update_menu()) {
                    break;
                }
                game();
                // fallthrough

            case GLOBAL_STATE_GAME:
                if (!update_menu()) {
                    break;
                }
                world_update();
                player_update();
                update_birds();
                break;

            case GLOBAL_STATE_GAME_OVER:
                world_update();
                update_birds();
                if (!update_menu()) {
                    break;
                }
                game();
                break;
        }

        BeginDrawing();

        ClearBackground(GAME_OUT_OF_BOUNDS_COLOR);

        switch (state->global_state) {

            case GLOBAL_STATE_DEFAULT:
                world_render();
                render_birds();
                render_menu();
                // TODO cartoon_transition_render();
                break;

            case GLOBAL_STATE_GAME:
                world_render();
                render_birds();
                player_render();
                ui_render();
                render_menu();
                break;

            case GLOBAL_STATE_GAME_OVER:
                world_render();
                render_birds();
                render_menu();
                break;
        }

        console_render();

        if (!has_flag(state->flags, FLAG_SHOW_OUT_OF_BOUNDS)) {
            // render out of bounds padding bars
            if (state->game_left > 0) {
                DrawRectangle(0, 0, state->game_left, state->game_height, GAME_OUT_OF_BOUNDS_COLOR);
                DrawRectangle(state->game_right, 0, state->game_left, state->game_height, GAME_OUT_OF_BOUNDS_COLOR);
            } else if (state->game_top > 0) {
                DrawRectangle(0, 0, state->game_width, state->game_top, GAME_OUT_OF_BOUNDS_COLOR);
                DrawRectangle(0, state->game_bottom, state->game_width, state->game_top, GAME_OUT_OF_BOUNDS_COLOR);
            }
        }

        EndDrawing();
    }

    atlas_cleanup();
    birds_cleanup();

    for (int i = 0; i < DEATH_SOUND_AMOUNT; i++) {
        UnloadSound(state->sounds_death_splats[i]);
    }

    free(state);

    CloseAudioDevice();

    CloseWindow();

    return 0;
}

#endif
