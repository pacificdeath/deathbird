#include <stdlib.h>
#include <math.h>
#include "../raylib-5.0_win64_mingw-w64/include/raylib.h"
#include "main.h"
#include "text_in_blackness.c"
#include "atlas.c"
#include "level.c"
#include "player.c"
#include "bird.c"
#include "terminal.c"
#include "menu.c"
#include "fader.c"
#include "portal.c"
#include "cartoon_transition.c"

#define AREA_TEXTURE_SIZE 128.0f

#if DEBUG
Vector2 to_pixel_position(State *state, Vector2 game_position) {
    return (Vector2) {
        state->game_left + (game_position.x + 1) / 2.0f * state->game_width,
        state->game_bottom - (game_position.y + 1) / 2.0f * state->game_height
    };
}

Vector2 to_pixel_size(State *state, Vector2 game_size) {
    return (Vector2) {
        game_size.x / 2.0f * state->game_width,
        game_size.y / 2.0f * state->game_height
    };
}
#endif

Rectangle game_rectangle(State *state) {
    return (Rectangle) {
        .x = state->game_left,
        .y = state->game_top,
        .width = state->game_width,
        .height = state->game_height
    };
}

bool has_flag(int flags, int flag) {
    return (flags & flag) == flag;
}

float randf(float min, float max) {
    float resolution = 999999.999f;
    min *= resolution;
    max *= resolution;
    float result = GetRandomValue(min, max) / resolution;
    return result;
}

Vector2 vec2_normalized(float x, float y) {
    float w = sqrt((x * x) + (y * y));
    if (w == 0) {
        return (Vector2){0};
    }
    Vector2 result = {
        x /= w,
        y /= w
    };
    return result;
}

Vector2 vec2_direction(Vector2 from, Vector2 to) {
    return (Vector2) {
        .x = to.x - from.x,
        .y = to.y - from.y
    };
}

float vec2_distance(Vector2 a, Vector2 b) {
    float x = fabs(a.x - b.x);
    float y = fabs(a.y - b.y);
    return sqrtf(x*x + y*y);
}

void update_state_dimensions(State *state, int w, int h) {
    float padding_w;
    float padding_h;
    float ratio_unit;
    if ((w / GAME_WIDTH_RATIO) < (h / GAME_HEIGHT_RATIO)) {
        ratio_unit = lroundf(w / GAME_WIDTH_RATIO);
        padding_w = 0;
        padding_h = h - (ratio_unit * GAME_HEIGHT_RATIO);
    } else {
        ratio_unit = lroundf(h / GAME_HEIGHT_RATIO);
        padding_w = w - (ratio_unit * GAME_WIDTH_RATIO);
        padding_h = 0;
    }
    float half_padding_w = padding_w / 2;
    float half_padding_h = padding_h / 2;
    state->game_width = lroundf(w - padding_w);
    state->game_height = lroundf(h - padding_h);
    state->scale_multiplier = ratio_unit / AREA_TEXTURE_SIZE;
    state->game_left = lroundf(half_padding_w);
    state->game_right = lroundf(half_padding_w + state->game_width);
    state->game_top = lroundf(half_padding_h);
    state->game_bottom = lroundf(half_padding_h + state->game_height);
    state->game_center_x = lroundf(half_padding_w + (state->game_width / 2));
    state->game_center_y = lroundf(half_padding_h + (state->game_height / 2));
}

void update_state_dimensions_window(State *state, int w, int h) {
    state->window_width = w;
    state->window_height = h;
    update_state_dimensions(state, w, h);
}

void toggle_state_dimensions_fullscreen(State *state) {
    if (IsWindowFullscreen()) {
        ToggleFullscreen();
        SetWindowSize(state->window_width, state->window_height);
        update_state_dimensions(state, state->window_width, state->window_height);
        ShowCursor();
    } else {
        int monitor = GetCurrentMonitor();
        int w = GetMonitorWidth(monitor);
        int h = GetMonitorHeight(monitor);
        ToggleFullscreen();
        SetWindowSize(w, h);
        update_state_dimensions(state, w, h);
        HideCursor();
    }
}

int catch_window_resize(State *state) {
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    if (state->window_width != w || state->window_height != h) {
        update_state_dimensions_window(state, w, h);
        return 1;
    }
    return 0;
}

void travel_to_area(State *state, Area area) {
    state->area = area;
    state->birds_requested = 0;
    menu_level_setup(state);
    player_level_setup(state);
    level_load_next(state);
    birds_prepare_shader(state, area);
}

int main(void) {
    State *state = (State *)calloc(1, sizeof(State));
    {
        int window_min_width = AREA_TEXTURE_SIZE * GAME_WIDTH_RATIO * GAME_MIN_SIZE;
        int window_min_height = AREA_TEXTURE_SIZE * GAME_HEIGHT_RATIO * GAME_MIN_SIZE;
        update_state_dimensions_window(state, window_min_width, window_min_height);

        SetTraceLogLevel(
            #if DEBUG
            LOG_ALL
            #else
            LOG_WARNING
            #endif
        );
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        InitWindow(state->window_width, state->window_height, "Deathbird");
        SetWindowMinSize(window_min_width, window_min_height);
        SetTargetFPS(60);

        InitAudioDevice();

        state->sounds_death_splats[0] = LoadSound("./sounds/splat1.wav");
        state->sounds_death_splats[1] = LoadSound("./sounds/splat2.wav");
        state->sounds_death_splats[2] = LoadSound("./sounds/splat3.wav");
        state->sounds_death_splats[3] = LoadSound("./sounds/splat4.wav");
        state->sounds_death_splats[4] = LoadSound("./sounds/splat5.wav");
        state->sounds_death_splats[5] = LoadSound("./sounds/splat6.wav");
        state->sounds_death_splats[6] = LoadSound("./sounds/splat7.wav");
        state->sounds_death_splats[7] = LoadSound("./sounds/splat8.wav");
        state->sound_explosion = LoadSound("./sounds/explosion.wav");

        atlas_init(state);
        state->area = AREA_INDUSTRIAL;
        level_load_0(state);
        portal_init(state);
        player_init(&state->player);
        player_level_setup(state);
        birds_init(state);
        terminal_init(state);
        menu_init(state);
        menu_level_setup(state);
        cartoon_transition_init(state);
    }
    portal_setup(state, PORTAL_BIT_RED);

    int skip_frames = 0;
    while (!WindowShouldClose()) {
        #if DEBUG
            static bool atlas_debug_active = false;
            if (IsKeyPressed(KEY_A)) {
                atlas_debug_active = !atlas_debug_active;
            }
            if (atlas_debug_active) {
                static int shader_idx = 0;
                if (IsKeyPressed(KEY_S)) {
                    shader_idx++;
                    if (shader_idx > AREA_CASTLE) {
                        shader_idx = 0;
                    }
                }
                if (shader_idx >= AREA_FOREST) {
                    birds_prepare_shader(state, shader_idx);
                }
                atlas_debug(state, shader_idx >= AREA_FOREST);
                continue;
            }
        #endif
        if (skip_frames > 0) {
            skip_frames--;
            BeginDrawing();
            DrawRectangle(0, 0, state->window_width, state->window_height, BLACK);
            EndDrawing();
            continue;
        }
        state->delta_time = GetFrameTime();
        if (IsWindowResized()) {
            update_state_dimensions_window(state, GetScreenWidth(), GetScreenHeight());
            menu_update_dimensions(state);
            terminal_update_dimensions(state);
            skip_frames = 2;
            continue;
        }
        if (IsKeyPressed(KEY_ENTER) && (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT))) {
            toggle_state_dimensions_fullscreen(state);
            skip_frames = 2;
            continue;
        }

        switch (state->global_state) {
        case GLOBAL_STATE_TERMINAL_MENU_STARTUP: {
            if (terminal_update(state)) {
                state->global_state = GLOBAL_STATE_MENU;
            }
        } break;
        case GLOBAL_STATE_TERMINAL_MANUAL_INPUT: {
            if (terminal_update(state)) {
                state->global_state = GLOBAL_STATE_MENU;
            }
        } break;
        case GLOBAL_STATE_MENU: {
            int area = menu_update(state);
            if (area != AREA_NONE) {
                travel_to_area(state, area);
                state->global_state = GLOBAL_STATE_GAME;
            }
        } break;
        case GLOBAL_STATE_TERMINAL_GAME_STARTUP: {
            if (terminal_update(state)) {
                state->global_state = GLOBAL_STATE_GAME;
            }
        } break;
        case GLOBAL_STATE_GAME: {
            level_update(state);
            // player must be updated before birds because it can change the
            // state of the birds which should be handled the same frame
            player_update(state);
            birds_update(state);
            cartoon_transition_update(state);
            bool level_success = state->portal_fuel >= state->current_level.required_fuel;
            bool level_fail = false;
            if (level_success || level_fail) {
                state->portal_fuel = 0;
                if (level_success) {
                    birds_destroy_all(state);
                    state->global_state = GLOBAL_STATE_WIN;
                } else {
                    portal_setup(state, PORTAL_BIT_INHALE | PORTAL_BIT_BLACK);
                    birds_give_alive_ones_to_portal(state);
                    menu_game_over_setup(state);
                }
            }
        } break;
        case GLOBAL_STATE_WIN: {
            level_update(state);
            birds_update(state);
            cartoon_transition_update(state);
            if (birds_all_available(state) && cartoon_transition_is_full_darkness(state)) {
                switch (state->area) {
                case AREA_FOREST: travel_to_area(state, AREA_MEADOWS); break;
                case AREA_MEADOWS: travel_to_area(state, AREA_MOUNTAINS); break;
                case AREA_MOUNTAINS: travel_to_area(state, AREA_INDUSTRIAL); break;
                case AREA_INDUSTRIAL: travel_to_area(state, AREA_CASTLE); break;
                case AREA_CASTLE: travel_to_area(state, AREA_FOREST); break;
                default: travel_to_area(state, AREA_FOREST); break;
                }
                state->global_state = GLOBAL_STATE_GAME;
            }
        } break;
        case GLOBAL_STATE_GAME_OVER: {
            if (state->fader_state != FADER_STATE_IN_COMPLETE) {
                fade_in(state);
            }
        } break;
        }

        BeginDrawing();

        ClearBackground(GAME_OUT_OF_BOUNDS_COLOR);

        switch (state->global_state) {
        case GLOBAL_STATE_TERMINAL_MENU_STARTUP:
        case GLOBAL_STATE_TERMINAL_GAME_STARTUP:
        case GLOBAL_STATE_TERMINAL_MANUAL_INPUT: {
            terminal_render(state);
        } break;
        case GLOBAL_STATE_MENU: {
            menu_render(state);
        } break;
        case GLOBAL_STATE_GAME: {
            level_render(state);
            birds_render(state);
            player_render(state);
            cartoon_transition_render(state);
        } break;
        case GLOBAL_STATE_WIN: {
            level_render(state);
            birds_render(state);
            cartoon_transition_render(state);
        } break;
        case GLOBAL_STATE_GAME_OVER: {
            game_over_text(state);
            fader_render(state);
        } break;
        }

        // render out of bounds padding bars
        if (state->game_left > 0) {
            DrawRectangle(0, 0, state->game_left, state->game_height, GAME_OUT_OF_BOUNDS_COLOR);
            DrawRectangle(state->game_right, 0, state->game_left, state->game_height, GAME_OUT_OF_BOUNDS_COLOR);
        } else if (state->game_top > 0) {
            DrawRectangle(0, 0, state->game_width, state->game_top, GAME_OUT_OF_BOUNDS_COLOR);
            DrawRectangle(0, state->game_bottom, state->game_width, state->game_top, GAME_OUT_OF_BOUNDS_COLOR);
        }

        EndDrawing();
    }

    atlas_cleanup(state);
    birds_cleanup(state);
    menu_cleanup(state);
    portal_cleanup(state);

    for (int i = 0; i < DEATH_SOUND_AMOUNT; i++) {
        UnloadSound(state->sounds_death_splats[i]);
    }

    free(state);

    CloseAudioDevice();

    CloseWindow();

    return 0;
}

