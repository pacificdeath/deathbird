#include <stdio.h>
#include "raylib.h"
#include "main.h"

static Vector2 scroll_tex_size() {
    return (Vector2) {
        .x = 2.0f / GAME_WIDTH_RATIO,
        .y = 2.0f / GAME_HEIGHT_RATIO
    };
}

static Vector2 scroll_tex_offset(Vector2 tex_size) {
    return (Vector2) {
        .x = -(tex_size.x),
        .y = -(tex_size.y / 2)
    };
}

static int setup_texture_scrollers_for_current_level(State *state) {
    Texture_Scroller *tex_scrollers = state->tex_scrollers;
    int texture_scroller_amount = 0;
    switch (state->level_current_data.area) {
    default: break;
    case AREA_FOREST: {
        tex_scrollers[0].tex = TEX_AREA_NIGHT_SKY;
        tex_scrollers[0].horizontal_textures = 4;
        tex_scrollers[0].horizontal_speed = -0.03f;
        tex_scrollers[0].vertical_textures = 2;
        tex_scrollers[0].vertical_speed = 0.0f;
        tex_scrollers[0].opacity = 255;

        tex_scrollers[1].tex = TEX_AREA_OCEAN,
        tex_scrollers[1].horizontal_textures = 4,
        tex_scrollers[1].horizontal_speed = 0.03f,
        tex_scrollers[1].vertical_textures = 1,
        tex_scrollers[1].vertical_speed = 0.0f;
        tex_scrollers[1].opacity = 255;

        tex_scrollers[2].tex = TEX_AREA_HILLS;
        tex_scrollers[2].horizontal_textures = 4;
        tex_scrollers[2].horizontal_speed = 0.15f;
        tex_scrollers[2].vertical_textures = 1;
        tex_scrollers[2].vertical_speed = 0.0f;
        tex_scrollers[2].opacity = 255;

        tex_scrollers[3].tex = TEX_AREA_TREES;
        tex_scrollers[3].horizontal_textures = 4;
        tex_scrollers[3].horizontal_speed = 0.75f;
        tex_scrollers[3].vertical_textures = 1;
        tex_scrollers[3].vertical_speed = 0.0f;
        tex_scrollers[3].opacity = 255;

        texture_scroller_amount = 4;
    } break;
    case AREA_MEADOWS: {
        tex_scrollers[0].tex = TEX_AREA_DAY_SKY;
        tex_scrollers[0].horizontal_textures = 4;
        tex_scrollers[0].horizontal_speed = -0.03f;
        tex_scrollers[0].vertical_textures = 2;
        tex_scrollers[0].vertical_speed = 0.0f;
        tex_scrollers[0].opacity = 255;

        tex_scrollers[1].tex = TEX_AREA_CLOUDS;
        tex_scrollers[1].horizontal_textures = 4;
        tex_scrollers[1].horizontal_speed = 0.03f;
        tex_scrollers[1].vertical_textures = 1;
        tex_scrollers[1].vertical_speed = 0.0f;
        tex_scrollers[1].opacity = 255;

        tex_scrollers[2].tex = TEX_AREA_MEADOWS;
        tex_scrollers[2].horizontal_textures = 4;
        tex_scrollers[2].horizontal_speed = 0.15f;
        tex_scrollers[2].vertical_textures = 1;
        tex_scrollers[2].vertical_speed = 0.0f;
        tex_scrollers[2].opacity = 255;

        tex_scrollers[3].tex = TEX_AREA_FENCE;
        tex_scrollers[3].horizontal_textures = 4;
        tex_scrollers[3].horizontal_speed = 0.75f;
        tex_scrollers[3].vertical_textures = 1;
        tex_scrollers[3].vertical_speed = 0.0f;
        tex_scrollers[3].opacity = 255;

        texture_scroller_amount = 4;
    } break;
    case AREA_MOUNTAINS: {
        tex_scrollers[0].tex = TEX_AREA_WINTER_SKY;
        tex_scrollers[0].horizontal_textures = 4;
        tex_scrollers[0].horizontal_speed = -0.03f;
        tex_scrollers[0].vertical_textures = 2;
        tex_scrollers[0].vertical_speed = 0.0f;
        tex_scrollers[0].opacity = 255;

        tex_scrollers[1].tex = TEX_AREA_MOUNTAINS;
        tex_scrollers[1].horizontal_textures = 4;
        tex_scrollers[1].horizontal_speed = 0.03f;
        tex_scrollers[1].vertical_textures = 1;
        tex_scrollers[1].vertical_speed = 0.0f;
        tex_scrollers[1].opacity = 255;

        tex_scrollers[2].tex = TEX_AREA_WINTER_HILLS;
        tex_scrollers[2].horizontal_textures = 4;
        tex_scrollers[2].horizontal_speed = 0.15f;
        tex_scrollers[2].vertical_textures = 1;
        tex_scrollers[2].vertical_speed = 0.0f;
        tex_scrollers[2].opacity = 255;

        tex_scrollers[3].tex = TEX_AREA_WINTER_TREES;
        tex_scrollers[3].horizontal_textures = 4;
        tex_scrollers[3].horizontal_speed = 0.75f;
        tex_scrollers[3].vertical_textures = 1;
        tex_scrollers[3].vertical_speed = 0.0f;
        tex_scrollers[3].opacity = 255;

        tex_scrollers[4].tex = TEX_AREA_SNOW;
        tex_scrollers[4].horizontal_textures = 4;
        tex_scrollers[4].horizontal_speed = -0.45f;
        tex_scrollers[4].vertical_textures = 3;
        tex_scrollers[4].vertical_speed = -0.75f;
        tex_scrollers[4].opacity = 128;

        texture_scroller_amount = 5;
    } break;
    case AREA_RED_BIRD_NEST: {
        tex_scrollers[0].tex = TEX_AREA_WEIRD_WALL;
        tex_scrollers[0].horizontal_textures = 4;
        tex_scrollers[0].horizontal_speed = -0.03f;
        tex_scrollers[0].vertical_textures = 2;
        tex_scrollers[0].vertical_speed = 0.0f;
        tex_scrollers[0].opacity = 255;

        texture_scroller_amount = 1;
    } break;
    }
    for (int i = 0; i < texture_scroller_amount; i++) {
        tex_scrollers[i].scroll.x = 0.0f;
        tex_scrollers[i].scroll.y = 0.0f;
    }
    return texture_scroller_amount;
}

bool level_score_reached(State *state) {
    return state->level_score >= state->level_current_data.required_score;
}

void level_setup_next(State *state) {
    Area area;
    switch (state->level_current_data.area) {
    default:
    case AREA_NONE: {
        area = AREA_FOREST;
    } break;
    case AREA_FOREST: {
        area = AREA_MEADOWS;
    } break;
    case AREA_MEADOWS: {
        area = AREA_MOUNTAINS;
    } break;
    case AREA_MOUNTAINS: {
        area = AREA_FOREST;
    } break;
    }
    int next_level_idx = state->level_idx + 1;

    state->next_level_data.area = area;

    int extra_birds = next_level_idx * LEVEL_BIRD_AMOUNT_MULTIPLIER;
    int extra_birds_random = GetRandomValue(LEVEL_MIN_RANDOM_BIRD_AMOUNT, LEVEL_MAX_RANDOM_BIRD_AMOUNT);
    state->next_level_data.total_birds = LEVEL_BASE_BIRD_TOTAL + extra_birds + extra_birds_random;
    state->next_level_data.required_score = state->next_level_data.total_birds * 5.0f;

    if (next_level_idx >= LEVELS_BEFORE_MIN_FREQUENCY) {
        state->next_level_data.bird_frequency = LEVEL_MIN_BIRD_FREQUENCY;
    } else {
        float min_max_frequency_diff = LEVEL_MAX_BIRD_FREQUENCY - LEVEL_MIN_BIRD_FREQUENCY;
        float bird_frequency_loss_per_level = min_max_frequency_diff / LEVELS_BEFORE_MIN_FREQUENCY;
        float bird_frequency_loss = bird_frequency_loss_per_level * next_level_idx;
        state->next_level_data.bird_frequency = LEVEL_MAX_BIRD_FREQUENCY - bird_frequency_loss;
    }
    float extra_bird_frequency = randf(LEVEL_MIN_RANDOM_BIRD_FREQUENCY, LEVEL_MAX_RANDOM_BIRD_FREQUENCY);
    state->next_level_data.bird_frequency += extra_bird_frequency;

    state->next_level_data.requested_birds = 0;
    state->next_level_data.spawned_birds = 0;
    state->next_level_data.passed_birds = 0;
}

void level_go_to_next(State *state) {
    state->level_idx++;
    state->level_current_data = state->next_level_data;
    state->level_current_data.tex_scroller_amount = setup_texture_scrollers_for_current_level(state);
}

void level_update(State *state) {
    Texture_Scroller *tex_scrollers = state->tex_scrollers;
    Vector2 tex_size = scroll_tex_size();
    for (int i = 0; i < state->level_current_data.tex_scroller_amount; i++) {
        tex_scrollers[i].scroll.x += (tex_scrollers[i].horizontal_speed * state->delta_time);
        if (tex_scrollers[i].horizontal_speed > 0.0f && tex_scrollers[i].scroll.x > 0.0f) {
            tex_scrollers[i].scroll.x -= tex_size.x;
        } else if (tex_scrollers[i].horizontal_speed < 0.0f && tex_scrollers[i].scroll.x < -tex_size.x) {
            tex_scrollers[i].scroll.x += tex_size.x;
        }
        tex_scrollers[i].scroll.y += (tex_scrollers[i].vertical_speed * state->delta_time);
        if (tex_scrollers[i].vertical_speed > 0.0f && tex_scrollers[i].scroll.y > 0.0f) {
            tex_scrollers[i].scroll.y -= tex_size.y;
        } else if (tex_scrollers[i].vertical_speed < 0.0f && tex_scrollers[i].scroll.y < -tex_size.y) {
            tex_scrollers[i].scroll.y += tex_size.y;
        }
    }
}

void level_render(State *state) {
    Texture_Scroller *tex_scrollers = state->tex_scrollers;
    Vector2 tex_size = scroll_tex_size();
    Vector2 tex_offset = scroll_tex_offset(tex_size);
    for (int i = 0; i < state->level_current_data.tex_scroller_amount; i++) {
        for (int x = 0; x < tex_scrollers[i].horizontal_textures; x++) {
            for (int y = 0; y < tex_scrollers[i].vertical_textures; y++) {
                Vector2 position = {
                    tex_offset.x + tex_scrollers[i].scroll.x + (tex_size.x * x),
                    tex_offset.y + tex_scrollers[i].scroll.y + (tex_size.y * y)
                };
                Color color = {255,255,255,tex_scrollers[i].opacity};
                atlas_draw(state, tex_scrollers[i].tex, position, 0.0f, 1.01f, color);
            }
        }
    }
    char buffer[16];
    sprintf(buffer, " Level: %i ", state->level_idx);
    int font_size = (MENU_FONT_SIZE / state->menu_font.baseSize) * state->scale_multiplier;
    Vector2 level_idx_text_dimensions = MeasureTextEx(state->menu_font, buffer, font_size, 0);
    Vector2 level_idx_position = { .x = state->game_left, .y = state->game_top };
    DrawRectangleV(level_idx_position, level_idx_text_dimensions, LEVEL_IDX_TEXT_BG_COLOR);
    DrawTextPro(state->menu_font, buffer, level_idx_position, (Vector2) { 0, 0 }, 0, font_size, 0, LEVEL_IDX_TEXT_COLOR);

    { // Progress bars
        Vector2 bar_position = {
            .x = state->game_left + state->game_width / 4,
            .y = state->game_top,
        };
        Vector2 bar_bg_size = {
            .x = state->game_width / 2,
            .y = state->game_height / 20,
        };
        if (level_score_reached(state)) {
            DrawRectangleV(bar_position, bar_bg_size, LEVEL_SCORE_BAR_FILLED_FG_COLOR);
        } else {
            DrawRectangleV(bar_position, bar_bg_size, LEVEL_SCORE_BAR_BG_COLOR);

            float fill_ratio = (float)(state->level_score) / (float)(state->level_current_data.required_score);
            Vector2 score_fill_size = {
                .x = bar_bg_size.x * fill_ratio,
                .y = bar_bg_size.y
            };

            DrawRectangleV(bar_position, score_fill_size, LEVEL_SCORE_BAR_FILL_FG_COLOR);

            if (state->bird_multiplier > 0) {
                Vector2 preview_fill_position = {
                    .x = bar_position.x + score_fill_size.x,
                    .y = bar_position.y
                };
                float x = state->bird_multiplier;
                float multiplier = (float)(x*x);
                float preview_fill_ratio = multiplier / (float)(state->level_current_data.required_score);
                Vector2 preview_fill_size = {
                    .x = bar_bg_size.x * preview_fill_ratio,
                    .y = bar_bg_size.y
                };
                float max_preview_x_size = bar_bg_size.x - score_fill_size.x;
                if (preview_fill_size.x > max_preview_x_size) {
                    preview_fill_size.x = max_preview_x_size;
                }
                Color preview_fill_color = LEVEL_SCORE_BAR_PREVIEW_FILL_FG_COLOR;
                preview_fill_color.a = 128.0f * (sin(GetTime() * 20.0f) + 1.0f);
                DrawRectangleV(preview_fill_position, preview_fill_size, preview_fill_color);
            }
        }
        sprintf(buffer, "%i / %i", state->level_score, state->level_current_data.required_score);
        Vector2 score_text_dimensions = MeasureTextEx(state->menu_font, buffer, font_size, 0);
        Vector2 score_text_position = {
            .x = state->game_center_x - (score_text_dimensions.x / 2),
            .y = bar_position.y + (bar_bg_size.y / 2) - (score_text_dimensions.y / 2)
        };
        DrawTextPro(state->menu_font, buffer, score_text_position, (Vector2) { 0, 0 }, 0, font_size, 0, LEVEL_SCORE_TEXT_COLOR);

        bar_position.y += bar_bg_size.y;
        DrawRectangleV(bar_position, bar_bg_size, LEVEL_PASSED_BIRDS_BAR_BG_COLOR);

        float birds_passed_fill_ratio = (float)state->level_current_data.passed_birds / (float)state->level_current_data.total_birds;
        Vector2 passed_birds_fill_size = {
            .x = bar_bg_size.x * birds_passed_fill_ratio,
            .y = bar_bg_size.y
        };
        DrawRectangleV(bar_position, passed_birds_fill_size, LEVEL_PASSED_BIRDS_BAR_FG_COLOR);
    }

    if (state->bird_multiplier_timer > 0.0f) {
        sprintf(buffer, " %ix-Multiplier ", state->bird_multiplier_display);
        Vector2 multiplier_text_dimensions = MeasureTextEx(state->menu_font, buffer, font_size, 0);
        Vector2 multiplier_text_position = {
            .x = state->game_right - multiplier_text_dimensions.x,
            .y = state->game_top
        };
        DrawRectangleV(multiplier_text_position, multiplier_text_dimensions, LEVEL_MULTIPLIER_TEXT_BG_COLOR);
        DrawTextPro(state->menu_font, buffer, multiplier_text_position, (Vector2) { 0, 0 }, 0, font_size, 0, LEVEL_MULTIPLIER_TEXT_COLOR);
    }
}
