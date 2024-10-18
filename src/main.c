#include <stdlib.h>
#include <math.h>
#include "raylib.h"
#include "main.h"
#include "atlas.c"
#include "level.c"
#include "player.c"
#include "bird.c"
#include "menu.c"
#include "fader.c"
#include "portal.c"

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
        .width = state->window_width,
        .height = state->window_height
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

int main(void) {
    State *state = (State *)calloc(1, sizeof(State));
    {
        int window_min_width = AREA_TEXTURE_SIZE * GAME_WIDTH_RATIO * GAME_MIN_SIZE;
        int window_min_height = AREA_TEXTURE_SIZE * GAME_HEIGHT_RATIO * GAME_MIN_SIZE;
        update_state_dimensions_window(state, window_min_width, window_min_height);

        SetTraceLogLevel(
            #if VERBOSE
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

        atlas_init(state);
        level_setup_next(state);
        portal_init(state);
        birds_init(state);
        menu_init(state);
        menu_level_setup(state);
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
                atlas_debug(state);
                continue;
            }
            if (IsKeyPressed(KEY_W)) {
                state->level_score = state->level_current_data.required_score;
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
            skip_frames = 2;
            continue;
        }
        if (IsKeyPressed(KEY_ENTER) && (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT))) {
            toggle_state_dimensions_fullscreen(state);
            skip_frames = 2;
            continue;
        }

        switch (state->global_state) {
        case GLOBAL_STATE_MENU: {
            menu_update(state);
        } break;
        case GLOBAL_STATE_MENU_FADE_OUT: {
            if (state->fader_state == FADER_STATE_OUT_COMPLETE) {
                level_go_to_next(state);
                player_level_setup(state);
                state->global_state = GLOBAL_STATE_GAME_FADE_IN;
            } else {
                fade_out(state);
            }
        } break;
        case GLOBAL_STATE_GAME_FADE_IN: {
            level_update(state);
            if (state->fader_state == FADER_STATE_IN_COMPLETE) {
                Portal_Bits portal_color_bit = portal_area_color(state->level_current_data.area);
                portal_setup(state, PORTAL_BIT_EXHALE | portal_color_bit);
                state->global_state = GLOBAL_STATE_PRE_GAME_PORTAL_APPEAR;
            } else {
                fade_in(state);
            }
        } break;
        case GLOBAL_STATE_PRE_GAME_PORTAL_APPEAR: {
            level_update(state);
            if (portal_appear(state)) {
                state->global_state = GLOBAL_STATE_PRE_GAME_PORTAL_EXHALE;
            }
        } break;
        case GLOBAL_STATE_PRE_GAME_PORTAL_EXHALE: {
            level_update(state);
            player_update(state);
            birds_update(state);
            if (portal_exhale(state)) {
                bird_level_setup(state);
                state->global_state = GLOBAL_STATE_PRE_GAME_PORTAL_DISAPPEAR;
            }
        } break;
        case GLOBAL_STATE_PRE_GAME_PORTAL_DISAPPEAR: {
            level_update(state);
            player_update(state);
            if (portal_disappear(state)) {
                state->global_state = GLOBAL_STATE_GAME;
            }
        } break;
        case GLOBAL_STATE_GAME: {
            level_update(state);
            // player must be updated before birds because it can change the
            // state of the birds which should be handled the same frame
            player_update(state);
            birds_update(state);
            if (level_score_reached(state)) {
                level_setup_next(state);
                Portal_Bits portal_color_bit = portal_area_color(state->next_level_data.area);
                portal_setup(state, PORTAL_BIT_INHALE | portal_color_bit);
                birds_give_alive_ones_to_portal(state);
                state->player_state = PLAYER_STATE_INHALED_BY_PORTAL;
                state->global_state = GLOBAL_STATE_POST_GAME_PORTAL_APPEAR;
            } else if (birds_ran_out(state) && state->player_state == PLAYER_STATE_GROUNDED) {
                portal_setup(state, PORTAL_BIT_NONE);
                state->global_state = GLOBAL_STATE_POST_GAME_PORTAL_APPEAR;
            }
        } break;
        case GLOBAL_STATE_POST_GAME_PORTAL_APPEAR: {
            level_update(state);
            birds_update(state);
            player_update(state);
            if (portal_appear(state)) {
                state->global_state = GLOBAL_STATE_POST_GAME_PORTAL_INHALE;
            }
        } break;
        case GLOBAL_STATE_POST_GAME_PORTAL_INHALE: {
            level_update(state);
            player_update(state);
            birds_update(state);
            if (portal_inhale(state)) {
                state->global_state = GLOBAL_STATE_POST_GAME_PORTAL_DISAPPEAR;
            }
        } break;
        case GLOBAL_STATE_POST_GAME_PORTAL_DISAPPEAR: {
            level_update(state);
            if (portal_disappear(state)) {
                state->global_state = GLOBAL_STATE_GAME_FADE_OUT;
            }
        } break;
        case GLOBAL_STATE_GAME_FADE_OUT: {
            level_update(state);
            player_update(state);
            if (state->fader_state == FADER_STATE_OUT_COMPLETE) {
                menu_level_setup(state);
                state->global_state = GLOBAL_STATE_MENU_FADE_IN;
            } else {
                fade_out(state);
            }
        } break;
        case GLOBAL_STATE_MENU_FADE_IN: {
            if (state->fader_state == FADER_STATE_IN_COMPLETE) {
                state->global_state = GLOBAL_STATE_MENU;
            } else {
                fade_in(state);
            }
        } break;
        }

        BeginDrawing();

        ClearBackground(GAME_OUT_OF_BOUNDS_COLOR);

        switch (state->global_state) {
        case GLOBAL_STATE_MENU_FADE_IN:
        case GLOBAL_STATE_MENU_FADE_OUT: {
            menu_render(state);
            fader_render(state);
        } break;
        case GLOBAL_STATE_MENU: {
            menu_render(state);
        } break;
        case GLOBAL_STATE_GAME_FADE_IN:
        case GLOBAL_STATE_GAME_FADE_OUT: {
            level_render(state);
            fader_render(state);
        } break;
        case GLOBAL_STATE_PRE_GAME_PORTAL_APPEAR:
        case GLOBAL_STATE_POST_GAME_PORTAL_DISAPPEAR: {
            level_render(state);
            portal_render(state);
        } break;
        case GLOBAL_STATE_PRE_GAME_PORTAL_EXHALE:
        case GLOBAL_STATE_PRE_GAME_PORTAL_DISAPPEAR:
        case GLOBAL_STATE_POST_GAME_PORTAL_APPEAR:
        case GLOBAL_STATE_POST_GAME_PORTAL_INHALE: {
            level_render(state);
            portal_render(state);
            birds_render(state);
            player_render(state);
        } break;
        case GLOBAL_STATE_GAME: {
            level_render(state);
            birds_render(state);
            player_render(state);
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