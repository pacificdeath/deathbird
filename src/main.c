#define TEXTURE_PATH ".\\textures"
#define ATLAS_PNG_PATH ".\\atlas\\atlas.png"
#define ATLAS_DATA_GENERATED_H_PATH ".\\atlas\\atlas_data.generated.h"

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

#include "../raylib/include/raylib.h"
#include "../raylib/include/rlgl.h"

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
#include "text_in_blackness.c"
#include "atlas.c"
#include "level.c"
#include "player.c"
#include "bird.c"
#include "console.c"
#include "fader.c"
#include "portal.c"
#include "cartoon_transition.c"
#include "ui.c"

#define TIME_CHANGE_SPEED 2.0f

inline Vector2 to_pixel_position(State *state, Vector2 game_position) {
    return (Vector2) {
        state->game_left + (game_position.x + 1) / 2.0f * state->game_width,
        state->game_bottom - (game_position.y + 1) / 2.0f * state->game_height
    };
}

inline Vector2 to_pixel_size(State *state, Vector2 game_size) {
    return (Vector2) {
        game_size.x / 2.0f * state->game_width,
        game_size.y / 2.0f * state->game_height
    };
}

inline Rectangle game_rectangle(State *state) {
    return (Rectangle) {
        .x = state->game_left,
        .y = state->game_top,
        .width = state->game_width,
        .height = state->game_height
    };
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

void transition_setup(int transition_type, int transition_value) {
    switch (transition_type) {
        default: ASSERT(false); break;
        case TRANSITION_FADE:
            switch (transition_value) {
                default: ASSERT(false); break;
                case TRANSITION_VALUE_MIN: state->fader_level = FADER_MIN; break;
                case TRANSITION_VALUE_MAX: state->fader_level = FADER_MAX; break;
            }
            state->cartoon_transition_size = CARTOON_TRANSITION_FULL_TRANSPARENCY;
            break;
        case TRANSITION_CARTOON:
            switch (transition_value) {
                default: ASSERT(false); break;
                case TRANSITION_VALUE_MIN: state->cartoon_transition_size = 0.0f; break;
                case TRANSITION_VALUE_MAX: state->cartoon_transition_size = CARTOON_TRANSITION_FULL_TRANSPARENCY; break;
            }
            state->fader_level = FADER_MAX;
            cartoon_transition_update(state);
            break;
    }
}

void area_init() {
    switch (state->area) {
        default:
            break;
        case AREA_VOID:
            reset_birds();
            state->player.state = PLAYER_STATE_GROUNDED;
            transition_setup(TRANSITION_FADE, TRANSITION_VALUE_MAX);
            break;
        case AREA_SKY:
            spawn_bird_shark("Terminal Bird Shark", 80 * get_loop_amount());
            break;
    }
}

bool area_update() {
    switch (state->area) {
        default:
            cartoon_transition_update(state);
            return level_target_reached();
        case AREA_MEADOWS:
            cartoon_transition_update(state);
            if (!level_target_reached()) return false;
            if (!bird_boss_exists()) {
                spawn_1_wheel_bird("Wheel Bird", 10 * get_loop_amount());
                return false;
            }
            return bird_boss_defeated();
        case AREA_GREEN:
            cartoon_transition_update(state);
            if (!level_target_reached()) return false;
            if (!bird_boss_exists()) {
                spawn_bird_shark("Bird Shark 2000", 20 * get_loop_amount());
                return false;
            }
            return bird_boss_defeated();
        case AREA_VOID:
            fade_in(state);
            return level_target_reached();
        case AREA_INDUSTRIAL:
            fade_in(state);
            if (!level_target_reached()) return false;
            if (!bird_boss_exists()) {
                spawn_2_wheel_birds("2 Wheel Birds?", 20 * get_loop_amount());
                return false;
            }
            return bird_boss_defeated();
        case AREA_CASTLE:
            if (!fade_in(state)) return false;
            if (!portal_exhale(state)) return false;
            return level_target_reached();
        case AREA_SKY:
            cartoon_transition_update(state);
            if (!cartoon_transition_is_full_transparency(state)) return false;
            if (!portal_exhale(state)) return false;
            return bird_boss_defeated();
    }
}

void area_pre_transition() {
    switch (state->area) {
        default:
            break;
        case AREA_MEADOWS: {
            prepare_cool_time_effect();
        } break;
        case AREA_GREEN:
            state->fader_level = FADER_MIN;
            portal_setup(state, (Color){0,0,0,255});
            prepare_cool_time_effect();
            give_alive_birds_to_portal();
            state->player.state = PLAYER_STATE_INHALED_BY_PORTAL;
            break;
        case AREA_INDUSTRIAL:
            portal_setup(state, (Color){255,0,0,255});
            prepare_cool_time_effect();
            give_alive_birds_to_portal();
            state->player.state = PLAYER_STATE_INHALED_BY_PORTAL;
            break;
        case AREA_CASTLE:
            transition_setup(TRANSITION_CARTOON, TRANSITION_VALUE_MAX);
            portal_setup(state, (Color){0,0,255,255});
            give_alive_birds_to_portal();
            state->player.state = PLAYER_STATE_INHALED_BY_PORTAL;
            break;
        case AREA_SKY:
            prepare_cool_time_effect();
            break;
    }
}

bool area_transition() {
    switch (state->area) {
        default:
            cartoon_transition_update(state);
            if (!cartoon_transition_is_full_darkness(state)) return false;
            return are_all_birds_available(state);
        case AREA_MEADOWS: {
            if (!cool_time_effect(0.1f, 2.0f)) return false;
            cartoon_transition_update(state);
            if (!cartoon_transition_is_full_darkness(state)) return false;
            return are_all_birds_available(state);
        } break;
        case AREA_GREEN:
            // & to have both functions run
            if (!(cool_time_effect(0.1f, 2.0f) & portal_inhale(state))) return false;
            return fade_out(state);
        case AREA_INDUSTRIAL:
            // & to have both functions run
            if (!(cool_time_effect(0.1f, 2.0f) & portal_inhale(state))) return false;
            return fade_out(state);
        case AREA_VOID:
            if (!are_all_birds_available()) return false;
            transition_setup(TRANSITION_FADE, TRANSITION_VALUE_MAX);
            return true;
        case AREA_CASTLE:
            if (!portal_inhale(state)) return false;
            cartoon_transition_update(state);
            return (cartoon_transition_is_full_darkness(state));
        case AREA_SKY:
            if (!cool_time_effect(0.1f, 2.0f)) return false;
            cartoon_transition_update(state);
            return cartoon_transition_is_full_darkness(state);
    }
}

void travel_to_area(int area, int level) {
    state->area = area;
    state->portal_fuel = 0;
    level_set(level);
    player_level_setup(state);
    prepare_bird_shader(area);
    restore_bosses();
    update_bird_spawn_weights();
    area_init();
}

void travel_to_next_area() {
    int area = (state->area + 1) % AREA_TOTAL;
    if (area <= AREA_NONE) {
        area = AREA_NONE + 1;
    }
    travel_to_area(area, state->level_idx + 1);
}

int main(void) {
    state = (State *)calloc(1, sizeof(State));
    {
        int window_min_width = AREA_TEXTURE_SIZE * GAME_WIDTH_RATIO * GAME_MIN_SIZE;
        int window_min_height = AREA_TEXTURE_SIZE * GAME_HEIGHT_RATIO * GAME_MIN_SIZE;
        update_state_dimensions_window(state, window_min_width, window_min_height);

        SetTraceLogLevel(DEATHBIRD_LOG_LEVEL);
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        InitWindow(state->window_width, state->window_height, "Deathbird");
        SetWindowMinSize(window_min_width, window_min_height);
        SetTargetFPS(60);

        InitAudioDevice();

        state->font = LoadFont("bios.ttf");
        update_font_size();

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
        state->time_scale = 1.0f;
        level_set(1);
        portal_init(state);
        player_init(&state->player);
        player_level_setup(state);
        initialize_birds();
        console_init();
        cartoon_transition_init(state);
    }

    int skip_frames = 0;
    while (!WindowShouldClose()) {
        if (has_flag(state->flags, FLAG_CONSOLE)) {
            console_update();
        }
        if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_C)) {
            if (has_flag(state->flags, FLAG_CONSOLE)) {
                state->flags &= ~FLAG_CONSOLE;
                console_reset();
            } else {
                state->flags |= FLAG_CONSOLE;
            }
        }

        if (skip_frames > 0) {
            skip_frames--;
            BeginDrawing();
            DrawRectangle(0, 0, state->window_width, state->window_height, BLACK);
            EndDrawing();
            continue;
        }
        state->delta_time = GetFrameTime() * state->time_scale;
        if (IsWindowResized()) {
            update_state_dimensions_window(state, GetScreenWidth(), GetScreenHeight());
            update_font_size();
            console_update_dimensions();
            skip_frames = 2;
            continue;
        }
        if (IsKeyPressed(KEY_ENTER) && (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT))) {
            toggle_state_dimensions_fullscreen(state);
            skip_frames = 2;
            continue;
        }

        switch (state->global_state) {
            case GLOBAL_STATE_DEFAULT:
                level_update(state);
                break;

            case GLOBAL_STATE_GAME:
                level_update(state);
                // player must be updated before birds because it can change the
                // state of the birds which should be handled the same frame
                player_update(state);
                update_birds();
                if (state->player.lives == 0) {
                    state->global_state = GLOBAL_STATE_GAME_OVER;
                    transition_setup(TRANSITION_FADE, TRANSITION_VALUE_MAX);
                    break;
                }
                if (!area_update()) {
                    break;
                }
                state->player.state = PLAYER_STATE_RETURN_TO_GROUND;
                state->portal_fuel = 0;
                destroy_all_birds();
                player_add_life();
                area_pre_transition();
                state->global_state = GLOBAL_STATE_TRANSITION;
                break;

            case GLOBAL_STATE_TRANSITION:
                level_update(state);
                player_update(state);
                update_birds();
                if (!area_transition()) {
                    break;
                }
                travel_to_next_area();
                state->global_state = GLOBAL_STATE_GAME;
                break;

            case GLOBAL_STATE_GAME_OVER:
                fade_in(state);
                if (!player_key_pressed(KEY_ENTER) && !player_key_pressed(KEY_SPACE)) {
                    break;
                }
                player_init(&state->player);
                travel_to_area(AREA_FOREST, 1);
                reset_birds();
                transition_setup(TRANSITION_CARTOON, TRANSITION_VALUE_MIN);
                state->global_state = GLOBAL_STATE_GAME;
                break;
        }

        BeginDrawing();

        ClearBackground(GAME_OUT_OF_BOUNDS_COLOR);

        switch (state->global_state) {

            case GLOBAL_STATE_DEFAULT:
                level_render(state);
                break;

            case GLOBAL_STATE_GAME:
                switch (state->area) {
                    default:
                        level_render(state);
                        render_birds();
                        player_render(state);
                        cartoon_transition_render(state);
                        ui_render();
                        break;
                    case AREA_VOID:
                        level_render(state);
                        render_birds();
                        player_render(state);
                        fader_render(state);
                        break;
                    case AREA_INDUSTRIAL:
                        level_render(state);
                        render_birds();
                        ui_render();
                        fader_render(state);
                        player_render(state);
                        break;
                    case AREA_CASTLE:
                        level_render(state);
                        portal_render(state, PORTAL_RENDER_MODE_EXHALE);
                        render_birds();
                        player_render(state);
                        fader_render(state);
                        ui_render();
                        break;
                    case AREA_SKY:
                        level_render(state);
                        portal_render(state, PORTAL_RENDER_MODE_EXHALE);
                        render_birds();
                        player_render(state);
                        cartoon_transition_render(state);
                        ui_render();
                        break;
                }
                break;

            case GLOBAL_STATE_TRANSITION:
                switch (state->area) {
                    default:
                        level_render(state);
                        render_birds();
                        player_render(state);
                        cartoon_transition_render(state);
                        ui_render();
                        break;
                    case AREA_GREEN:
                        level_render(state);
                        portal_render(state, PORTAL_RENDER_MODE_INHALE);
                        fader_render(state);
                        render_birds();
                        player_render(state);
                        break;
                    case AREA_INDUSTRIAL:
                        level_render(state);
                        portal_render(state, PORTAL_RENDER_MODE_INHALE);
                        fader_render(state);
                        render_birds();
                        player_render(state);
                        ui_render();
                        break;
                    case AREA_VOID:
                        level_render(state);
                        render_birds();
                        player_render(state);
                        break;
                    case AREA_CASTLE:
                        level_render(state);
                        portal_render(state, PORTAL_RENDER_MODE_INHALE);
                        render_birds();
                        player_render(state);
                        cartoon_transition_render(state);
                        ui_render();
                        break;
                }
                break;

            case GLOBAL_STATE_GAME_OVER:
                game_over_text(state);
                fader_render(state);
                break;

            case GLOBAL_STATE_BOSSBATTLE:
                player_render(state);
                break;

        }

        if (has_flag(state->flags, FLAG_CONSOLE)) {
            console_render();
        }

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

    atlas_cleanup(state);
    unload_bird_shaders();
    portal_cleanup(state);

    for (int i = 0; i < DEATH_SOUND_AMOUNT; i++) {
        UnloadSound(state->sounds_death_splats[i]);
    }

    free(state);

    CloseAudioDevice();

    CloseWindow();

    return 0;
}

#endif
