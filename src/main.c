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
#include "atlas.c"
#include "level.c"
#include "player.c"
#include "bird.c"
#include "fader.c"
#include "portal.c"
#include "cartoon_transition.c"
#include "ui.c"
#include "menu.c"

#ifdef DEBUG
#include "console.c"
#else
static inline void console_init() {}
static inline void console_update() {}
static inline void console_update_dimensions() {}
static inline void console_render() {}
#endif

#define TIME_CHANGE_SPEED 2.0f

inline Vector2 to_pixel_position(Vector2 game_position) {
    return (Vector2) {
        state->game_left + (game_position.x + 1) / 2.0f * state->game_width,
        state->game_bottom - (game_position.y + 1) / 2.0f * state->game_height
    };
}

inline Vector2 to_game_position(Vector2 pixel_position) {
    return (Vector2) {
        ((pixel_position.x - state->game_left) / state->game_width) * 2.0f - 1.0f,
        ((state->game_bottom - pixel_position.y) / state->game_height) * 2.0f - 1.0f
    };
}

inline Vector2 to_pixel_size(Vector2 game_size) {
    return (Vector2) {
        game_size.x / 2.0f * state->game_width,
        game_size.y / 2.0f * state->game_height
    };
}

inline Rectangle game_rectangle() {
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
            cartoon_transition_update();
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
            cartoon_transition_update();
            return level_target_reached();
        case AREA_MEADOWS:
            cartoon_transition_update();
            if (!level_target_reached()) return false;
            if (!bird_boss_exists()) {
                spawn_hat_bird("The Fat Bird", 10 * get_loop_amount());
                // spawn_1_wheel_bird("Wheel Bird", 10 * get_loop_amount());
                return false;
            }
            return bird_boss_defeated();
        case AREA_GREEN:
            cartoon_transition_update();
            if (!level_target_reached()) return false;
            if (!bird_boss_exists()) {
                spawn_bird_shark("Bird Shark 2000", 20 * get_loop_amount());
                return false;
            }
            return bird_boss_defeated();
        case AREA_VOID:
            fade_in();
            return level_target_reached();
        case AREA_INDUSTRIAL:
            fade_in();
            if (!level_target_reached()) return false;
            if (!bird_boss_exists()) {
                spawn_giant_wheel_bird("2 Wheel Birds?", 20 * get_loop_amount());
                return false;
            }
            return bird_boss_defeated();
        case AREA_CASTLE:
            if (!fade_in()) return false;
            if (!portal_exhale()) return false;
            return level_target_reached();
        case AREA_SKY:
            cartoon_transition_update();
            if (!cartoon_transition_is_full_transparency()) return false;
            if (!portal_exhale()) return false;
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
            portal_setup((Color){0,0,0,255});
            prepare_cool_time_effect();
            give_alive_birds_to_portal();
            state->player.state = PLAYER_STATE_INHALED_BY_PORTAL;
            break;
        case AREA_INDUSTRIAL:
            portal_setup((Color){255,0,0,255});
            prepare_cool_time_effect();
            give_alive_birds_to_portal();
            state->player.state = PLAYER_STATE_INHALED_BY_PORTAL;
            break;
        case AREA_CASTLE:
            transition_setup(TRANSITION_CARTOON, TRANSITION_VALUE_MAX);
            portal_setup((Color){0,0,255,255});
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
        default: {
            cartoon_transition_update();
            if (!cartoon_transition_is_full_darkness()) return false;
            return are_all_birds_available();
        }
        case AREA_MEADOWS: {
            if (!cool_time_effect(0.1f, 2.0f)) return false;
            cartoon_transition_update();
            if (!cartoon_transition_is_full_darkness()) return false;
            return are_all_birds_available();
        }
        case AREA_GREEN: {
            bool a = cool_time_effect(0.1f, 2.0f);
            bool b = portal_inhale();
            if (!(a && b)) return false;
            return fade_out();
        }
        case AREA_INDUSTRIAL: {
            bool a = cool_time_effect(0.1f, 2.0f);
            bool b = portal_inhale();
            if (!(a && b)) return false;
            return fade_out();
        }
        case AREA_VOID: {
            if (!are_all_birds_available()) return false;
            transition_setup(TRANSITION_FADE, TRANSITION_VALUE_MAX);
            return true;
        }
        case AREA_CASTLE: {
            if (!portal_inhale()) return false;
            cartoon_transition_update();
            return (cartoon_transition_is_full_darkness());
        }
        case AREA_SKY: {
            if (!cool_time_effect(0.1f, 2.0f)) return false;
            cartoon_transition_update();
            return cartoon_transition_is_full_darkness();
        }
    }
}

void travel_to_area(int area, int level) {
    level_setup(area, level);
    player_level_setup();
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

void prepare_default_state() {
    state->global_state = GLOBAL_STATE_DEFAULT;
    travel_to_area(AREA_FOREST, 1);
    transition_setup(TRANSITION_CARTOON, TRANSITION_VALUE_MIN);
    initialize_menu();
}

int main(void) {
    state = (State *)calloc(1, sizeof(State));
    {
        SetTraceLogLevel(DEATHBIRD_LOG_LEVEL);
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        int window_min_width = AREA_TEXTURE_SIZE * GAME_WIDTH_RATIO * GAME_MIN_SIZE;
        int window_min_height = AREA_TEXTURE_SIZE * GAME_HEIGHT_RATIO * GAME_MIN_SIZE;
        InitWindow(window_min_width, window_min_height, "Deathbird");
        SetWindowMinSize(window_min_width, window_min_height);
        SetTargetFPS(60);
        SetExitKey(KEY_NULL);

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

        atlas_init();
        state->time_scale = 1.0f;
        portal_init();
        player_init(&state->player);
        player_level_setup();
        initialize_birds();
        console_init();
        cartoon_transition_init();
    }

    prepare_default_state();

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
                w = GetMonitorWidth(monitor);
                h = GetMonitorHeight(monitor);
            } else {
                w = GetScreenWidth();
                h = GetScreenHeight();
            }

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
            state->game_center_x = lroundf(half_padding_w + (state->game_width / 2.0f));
            state->game_center_y = lroundf(half_padding_h + (state->game_height / 2.0f));
        }

        state->font_size = (FONT_SIZE / state->font.baseSize) * state->scale_multiplier;

        console_update();

        switch (state->global_state) {
            case GLOBAL_STATE_DEFAULT:
                level_update();
                player_update();
                cartoon_transition_update();
                if (!cartoon_transition_is_full_transparency()) {
                    break;
                }
                if (!update_menu()) {
                    break;
                }
                state->global_state = GLOBAL_STATE_GAME;
                // fallthrough

            case GLOBAL_STATE_GAME:
                if (!update_menu()) {
                    break;
                }
                level_update();
                player_update();
                update_birds();
                if (state->player.lives == 0) {
                    state->global_state = GLOBAL_STATE_GAME_OVER;
                    initialize_menu();
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
                level_update();
                player_update();
                update_birds();
                if (!area_transition()) {
                    break;
                }
                travel_to_next_area();
                state->global_state = GLOBAL_STATE_GAME;
                break;

            case GLOBAL_STATE_GAME_OVER:
                level_update();
                update_birds();
                if (!update_menu()) {
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
                level_render();
                player_render();
                render_menu();
                cartoon_transition_render();
                break;

            case GLOBAL_STATE_GAME:
                switch (state->area) {
                    default:
                        level_render();
                        render_birds();
                        player_render();
                        cartoon_transition_render();
                        ui_render();
                        break;
                    case AREA_VOID:
                        level_render();
                        render_birds();
                        player_render();
                        fader_render();
                        break;
                    case AREA_INDUSTRIAL:
                        level_render();
                        render_birds();
                        ui_render();
                        fader_render();
                        player_render();
                        break;
                    case AREA_CASTLE:
                        level_render();
                        portal_render(PORTAL_RENDER_MODE_EXHALE);
                        render_birds();
                        player_render();
                        fader_render();
                        ui_render();
                        break;
                    case AREA_SKY:
                        level_render();
                        portal_render(PORTAL_RENDER_MODE_EXHALE);
                        render_birds();
                        player_render();
                        cartoon_transition_render();
                        ui_render();
                        break;
                }
                render_menu();
                break;

            case GLOBAL_STATE_TRANSITION:
                switch (state->area) {
                    default:
                        level_render();
                        render_birds();
                        player_render();
                        cartoon_transition_render();
                        ui_render();
                        break;
                    case AREA_GREEN:
                        level_render();
                        portal_render(PORTAL_RENDER_MODE_INHALE);
                        fader_render();
                        render_birds();
                        player_render();
                        break;
                    case AREA_INDUSTRIAL:
                        level_render();
                        portal_render(PORTAL_RENDER_MODE_INHALE);
                        fader_render();
                        render_birds();
                        player_render();
                        ui_render();
                        break;
                    case AREA_VOID:
                        level_render();
                        render_birds();
                        player_render();
                        break;
                    case AREA_CASTLE:
                        level_render();
                        portal_render(PORTAL_RENDER_MODE_INHALE);
                        render_birds();
                        player_render();
                        cartoon_transition_render();
                        ui_render();
                        break;
                }
                render_menu();
                break;

            case GLOBAL_STATE_GAME_OVER:
                level_render();
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
    unload_bird_shaders();
    portal_cleanup();

    for (int i = 0; i < DEATH_SOUND_AMOUNT; i++) {
        UnloadSound(state->sounds_death_splats[i]);
    }

    free(state);

    CloseAudioDevice();

    CloseWindow();

    return 0;
}

#endif
