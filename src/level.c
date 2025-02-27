#include <stdio.h>
#include "../raylib-5.0_win64_mingw-w64/include/raylib.h"
#include "main.h"

#define MIN_BIRD_FREQUENCY 0.05f
#define MAX_BIRD_FREQUENCY 0.1f
#define LEVELS_BEFORE_MIN_FREQUENCY 31
#define LEVEL_IDX_TEXT_BG_COLOR ((Color) { 0, 0, 0, 255 })
#define LEVEL_IDX_TEXT_COLOR ((Color) { 255, 255, 255, 255 })
#define MULTIPLIER_TEXT_BG_COLOR ((Color) { 255, 0, 0, 255 })
#define MULTIPLIER_TEXT_COLOR ((Color) { 255, 255, 255, 255 })

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
    switch (state->area) {
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
    case AREA_INDUSTRIAL: {
        tex_scrollers[0].tex = TEX_AREA_GREEN_SKY;
        tex_scrollers[0].horizontal_textures = 4;
        tex_scrollers[0].horizontal_speed = -0.03f;
        tex_scrollers[0].vertical_textures = 2;
        tex_scrollers[0].vertical_speed = 0.0f;
        tex_scrollers[0].opacity = 255;

        tex_scrollers[1].tex = TEX_AREA_GREY_CLOUDS;
        tex_scrollers[1].horizontal_textures = 4;
        tex_scrollers[1].horizontal_speed = 0.03f;
        tex_scrollers[1].vertical_textures = 1;
        tex_scrollers[1].vertical_speed = 0.0f;
        tex_scrollers[1].opacity = 255;

        tex_scrollers[2].tex = TEX_AREA_INDUSTRIAL_FIELD;
        tex_scrollers[2].horizontal_textures = 4;
        tex_scrollers[2].horizontal_speed = 0.15f;
        tex_scrollers[2].vertical_textures = 1;
        tex_scrollers[2].vertical_speed = 0.0f;
        tex_scrollers[2].opacity = 255;

        tex_scrollers[3].tex = TEX_AREA_INDUSTRIAL_FENCE;
        tex_scrollers[3].horizontal_textures = 4;
        tex_scrollers[3].horizontal_speed = 0.75f;
        tex_scrollers[3].vertical_textures = 1;
        tex_scrollers[3].vertical_speed = 0.0f;
        tex_scrollers[3].opacity = 255;

        tex_scrollers[4].tex = TEX_AREA_RAIN;
        tex_scrollers[4].horizontal_textures = 3;
        tex_scrollers[4].horizontal_speed = 0.0f;
        tex_scrollers[4].vertical_textures = 3;
        tex_scrollers[4].vertical_speed = -2.0f;
        tex_scrollers[4].opacity = 64;

        texture_scroller_amount = 5;
    } break;
    case AREA_CASTLE: {
        tex_scrollers[0].tex = TEX_AREA_WINTER_SKY;
        tex_scrollers[0].horizontal_textures = 4;
        tex_scrollers[0].horizontal_speed = -0.03f;
        tex_scrollers[0].vertical_textures = 2;
        tex_scrollers[0].vertical_speed = 0.0f;
        tex_scrollers[0].opacity = 255;

        tex_scrollers[1].tex = TEX_AREA_CASTLE_MOUNTAIN;
        tex_scrollers[1].horizontal_textures = 4;
        tex_scrollers[1].horizontal_speed = 0.03f;
        tex_scrollers[1].vertical_textures = 1;
        tex_scrollers[1].vertical_speed = 0.0f;
        tex_scrollers[1].opacity = 255;

        tex_scrollers[2].tex = TEX_AREA_CASTLE_SILHOUETTES;
        tex_scrollers[2].horizontal_textures = 4;
        tex_scrollers[2].horizontal_speed = 0.15f;
        tex_scrollers[2].vertical_textures = 1;
        tex_scrollers[2].vertical_speed = 0.0f;
        tex_scrollers[2].opacity = 255;

        tex_scrollers[3].tex = TEX_AREA_CASTLE_BRICKS;
        tex_scrollers[3].horizontal_textures = 4;
        tex_scrollers[3].horizontal_speed = 0.75f;
        tex_scrollers[3].vertical_textures = 1;
        tex_scrollers[3].vertical_speed = 0.0f;
        tex_scrollers[3].opacity = 255;

        texture_scroller_amount = 4;
    } break;
    }
    for (int i = 0; i < texture_scroller_amount; i++) {
        ASSERT(i < TEXTURE_SCROLLER_CAP);
        tex_scrollers[i].scroll.x = 0.0f;
        tex_scrollers[i].scroll.y = 0.0f;
    }
    return texture_scroller_amount;
}

static Level generate_level_data(State *state) {
    Level level = {0};

    if (state->level_idx >= LEVELS_BEFORE_MIN_FREQUENCY) {
        level.bird_frequency = MIN_BIRD_FREQUENCY;
    } else {
        float min_max_frequency_diff = MAX_BIRD_FREQUENCY - MIN_BIRD_FREQUENCY;
        float bird_frequency_loss_per_level = min_max_frequency_diff / LEVELS_BEFORE_MIN_FREQUENCY;
        float bird_frequency_loss = bird_frequency_loss_per_level * state->level_idx;
        level.bird_frequency = MAX_BIRD_FREQUENCY - bird_frequency_loss;
        level.required_fuel = 10 + (state->level_idx * 5);
    }

    return level;
}

void level_load_0(State *state) {
    state->level_idx = 0;
    state->current_level = generate_level_data(state);
    state->current_level.tex_scroller_amount = setup_texture_scrollers_for_current_level(state);
}

void level_load_next(State *state) {
    state->level_idx++;
    state->current_level = generate_level_data(state);
    state->current_level.tex_scroller_amount = setup_texture_scrollers_for_current_level(state);
}

void level_update(State *state) {
    Texture_Scroller *tex_scrollers = state->tex_scrollers;
    Vector2 tex_size = scroll_tex_size();
    for (int i = 0; i < state->current_level.tex_scroller_amount; i++) {
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

static void render_bar(Rectangle rec, float fill_percent) {
    DrawRectangleRec(rec, (Color){128,128,128,128});
    float full_w = rec.width;
    float fill_w = (full_w / 100.0f) * fill_percent;
    rec.width = fill_w;
    DrawRectangleRec(rec, (Color){0,255,0,255});
}

inline static int calc_font_size(State *state) {
    return (MENU_FONT_SIZE / state->menu.font.baseSize) * state->scale_multiplier;
}

Vector2 text_box(State *state, char *buffer, Vector2 position, Color bg_color, Color fg_color) {
    int font_size = calc_font_size(state);
    Vector2 text_dimensions = MeasureTextEx(state->menu.font, buffer, font_size, 0);
    DrawRectangleV(position, text_dimensions, bg_color);
    DrawTextPro(state->menu.font, buffer, position, (Vector2) { 0, 0 }, 0, font_size, 0, fg_color);
    return text_dimensions;
}

void level_render(State *state) {
    Texture_Scroller *tex_scrollers = state->tex_scrollers;
    Vector2 tex_size = scroll_tex_size();
    Vector2 tex_offset = scroll_tex_offset(tex_size);
    for (int i = 0; i < state->current_level.tex_scroller_amount; i++) {
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
    Vector2 position = { .x = state->game_left, .y = state->game_top };
    Vector2 text_dimensions = text_box(state, buffer, position, LEVEL_IDX_TEXT_BG_COLOR, LEVEL_IDX_TEXT_COLOR);

    float fuel_bar_width = state->game_width * 0.4f;
    Rectangle fuel_bar_rec = {
        .x = state->game_center_x - (fuel_bar_width * 0.5f),
        .y = state->game_top,
        .width = fuel_bar_width,
        .height = text_dimensions.y,
    };
    float fill_percent = (100.0f / state->current_level.required_fuel) * state->portal_fuel;
    render_bar(fuel_bar_rec, fill_percent);

    int font_size = calc_font_size(state);
    if (state->bird_multiplier_timer > 0.0f) {
        sprintf(buffer, " %ix-Multiplier ", state->bird_multiplier_display);
        Vector2 multiplier_text_dimensions = MeasureTextEx(state->menu.font, buffer, font_size, 0);
        Vector2 multiplier_text_position = {
            .x = state->game_right - multiplier_text_dimensions.x,
            .y = state->game_top
        };
        DrawRectangleV(multiplier_text_position, multiplier_text_dimensions, MULTIPLIER_TEXT_BG_COLOR);
        DrawTextPro(state->menu.font, buffer, multiplier_text_position, (Vector2){0}, 0, font_size, 0, MULTIPLIER_TEXT_COLOR);
    }
}
