#include "stdlib.h"
#include "math.h"
#include "raylib.h"
#include "main.h"
#include "tex_atlas.c"
#include "scroll_env.c"
#include "player.c"
#include "bird.c"
#include "bird_computer.c"

float randf(float min, float max) {
    float resolution = 999999.999f;
    min *= resolution;
    max *= resolution;
    float result = GetRandomValue(min, max) / resolution;
    return result;
}

Vector2 vec2_normalized(float x, float y) {
    float w = sqrt((x * x) + (y * y));
    x /= w;
    y /= w;
    Vector2 result = { x, y };
    return result;
}

void update_state_dimensions(State *state, int w, int h) {
    state->screen_width = w;
    state->screen_height = h;
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
    state->scale_multiplier = ratio_unit / ENV_TEX_SIZE;
    state->game_left = lroundf(half_padding_w);
    state->game_right = lroundf(half_padding_w + state->game_width);
    state->game_top = lroundf(half_padding_h);
    state->game_bottom = lroundf(half_padding_h + state->game_height);
    state->game_center_x = lroundf(half_padding_w + (state->game_width / 2));
    state->game_center_y = lroundf(half_padding_h + (state->game_height / 2));
}

int catch_window_resize(State *state) {
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    if (state->screen_width != w || state->screen_height != h) {
        update_state_dimensions(state, w, h);
        return 1;
    }
    return 0;
}

int main(void) {
    State *state = (State *)calloc(1, sizeof(State));
    {
        int window_min_width = ENV_TEX_SIZE * GAME_WIDTH_RATIO * GAME_MIN_SIZE;
        int window_min_height = ENV_TEX_SIZE * GAME_HEIGHT_RATIO * GAME_MIN_SIZE;
        update_state_dimensions(state, window_min_width, window_min_height);

        SetTraceLogLevel(
            #ifdef VERBOSE
            LOG_ALL
            #else
            LOG_WARNING
            #endif
        );
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        InitWindow(state->screen_width, state->screen_height, "Deathbird");
        SetWindowMinSize(window_min_width, window_min_height);
#ifdef FULLSCREEN
        int display = GetCurrentMonitor();
        SetWindowSize(GetMonitorWidth(display), GetMonitorHeight(display));
        ToggleFullscreen();
        catch_window_resize(state);
#endif
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
    }

    tex_atlas_init(state);
    bird_computer_init(state);

    state->player.damage = 10;

    int skip_frame = 0;
    while (!WindowShouldClose()) {
        if (skip_frame) {
            skip_frame = 0;
            continue;
        }
        state->delta_time = GetFrameTime();
        if (IsWindowResized()) {
            update_state_dimensions(state, GetScreenWidth(), GetScreenHeight());
            skip_frame = 1;
            BeginDrawing();
            DrawRectangle(0, 0, state->screen_width, state->screen_height, BLACK);
            EndDrawing();
        }

        if (IsKeyPressed(KEY_C)) {
            if (state->game_state == GAME_STATE_DEATHBIRD) {
                state->game_state = GAME_STATE_BIRD_COMPUTER;
            } else {
                state->game_state = GAME_STATE_DEATHBIRD;
            }
        }

        switch (state->game_state) {
        case GAME_STATE_NEXT_LEVEL: {
            state->player.position.x = 0.0f;
            state->player.position.y = 1.0f;
            state->level_bird_amount = 2;
            state->level_requested_birds = 0;
            state->level_spawned_birds = 0;
            state->level_passed_birds = 0;
            state->level_bird_rate = 1.0f;
            state->level_bird_timer = 0.0f;
            switch (state->game_level) {
            case GAME_LEVEL_NONE: state->game_level = GAME_LEVEL_FOREST; break;
            case GAME_LEVEL_FOREST: state->game_level = GAME_LEVEL_FIELD; break;
            case GAME_LEVEL_FIELD: state->game_level = GAME_LEVEL_SNOW; break;
            case GAME_LEVEL_SNOW: state->game_level = GAME_LEVEL_FOREST; break;
            }
            scroll_env_level_setup(state);
            player_level_setup(state);
            bird_level_setup(state);
            state->game_state = GAME_STATE_DEATHBIRD;
        } break;
        case GAME_STATE_DEATHBIRD: {
            scroll_env_update(state);
            // player must be updated before birds because it can change the
            // state of the birds which should be handled the same frame
            player_update(state);
            birds_update(state);
            if (birds_ready_for_exit(state) && player_ready_for_exit(state)) {
                bird_computer_level_setup(state);
                state->game_state = GAME_STATE_BIRD_COMPUTER;
            }
        } break;
        case GAME_STATE_BIRD_COMPUTER: {
            bird_computer_update(state);
        } break;
        }

        BeginDrawing();

        ClearBackground(OUT_OF_BOUNDS_COLOR);

        switch (state->game_state) {
        case GAME_STATE_NEXT_LEVEL:
            continue;
        case GAME_STATE_DEATHBIRD: {
            scroll_env_render(state);
            birds_render(state);
            player_render(state);
        } break;
        case GAME_STATE_BIRD_COMPUTER: {
            bird_computer_render(state);
        } break;
        }

        // render out of bounds padding bars
        if (state->game_left > 0) {
            DrawRectangle(0, 0, state->game_left, state->game_height, OUT_OF_BOUNDS_COLOR);
            DrawRectangle(state->game_right, 0, state->game_left, state->game_height, OUT_OF_BOUNDS_COLOR);
        } else if (state->game_top > 0) {
            DrawRectangle(0, 0, state->game_width, state->game_top, OUT_OF_BOUNDS_COLOR);
            DrawRectangle(0, state->game_bottom, state->game_width, state->game_top, OUT_OF_BOUNDS_COLOR);
        }

        EndDrawing();
    }

    tex_atlas_cleanup(state);
    bird_computer_cleanup(state);

    for (int i = 0; i < DEATH_SOUND_AMOUNT; i++) {
        UnloadSound(state->sounds_death_splats[i]);
    }

    free(state);

    CloseAudioDevice();

    CloseWindow();

    return 0;
}