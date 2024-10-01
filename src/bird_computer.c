#include <stdio.h>
#include "raylib.h"
#include "main.h"

static void draw_header_text(State *state, Bird_Computer_Dimensions *dimensions, char *text) {
    const Vector2 text_dimensions = MeasureTextEx(state->bird_computer.font, text, dimensions->font_size, 0.0f);
    Vector2 text_origin = (Vector2){ 0, -(dimensions->header_size / 2.0f) + (text_dimensions.y / 2.0f) };
    Vector2 position = {
        .x = state->game_left + (state->game_width - text_dimensions.x) / 2,
        .y = state->game_top
    };
    DrawTextPro(
        state->bird_computer.font,
        text,
        position,
        text_origin,
        0.0f,
        dimensions->font_size,
        0.0f,
        BIRD_COMPUTER_TEXT_COLOR
    );
}

static void draw_option_text(State *state, Bird_Computer_Dimensions *dimensions, char *text, int line_idx) {
    if (line_idx < 0 || line_idx >= BIRD_COMPUTER_LINE_COUNT) {
        return;
    }
    const Vector2 text_dimensions = MeasureTextEx(state->bird_computer.font, text, dimensions->font_size, 0.0f);
    Vector2 text_origin = { 0, -(dimensions->line_size / 2.0f) + (text_dimensions.y / 2.0f) };
    Vector2 position = {
        .x = state->game_left + (dimensions->x_fract * 2),
        .y = state->game_top + dimensions->header_size + (dimensions->line_size * line_idx)
    };
    DrawTextPro(
        state->bird_computer.font,
        text,
        position,
        text_origin,
        0.0f,
        dimensions->font_size,
        0.0f,
        BIRD_COMPUTER_TEXT_COLOR
    );
}

static void draw_option_cursor(State *state, Bird_Computer_Dimensions *dimensions) {
    char *cursor = ">";
    const Vector2 text_dimensions = MeasureTextEx(state->bird_computer.font, cursor, dimensions->font_size, 0.0f);
    Vector2 text_origin = { 0, -(dimensions->line_size / 2.0f) + (text_dimensions.y / 2.0f) };
    Vector2 position = {
        .x = state->game_left + dimensions->x_fract,
        .y = state->game_top + dimensions->header_size + (dimensions->line_size * state->bird_computer.option_idx)
    };
    DrawTextPro(
        state->bird_computer.font,
        cursor,
        position,
        text_origin,
        0.0f,
        dimensions->font_size,
        0.0f,
        BIRD_COMPUTER_TEXT_COLOR
    );
}

void draw_info_text_line(State *state, Bird_Computer_Dimensions *dimensions, char *text, int line_idx) {
    const Vector2 text_dimensions = MeasureTextEx(state->bird_computer.font, text, dimensions->font_size, 0.0f);
    Vector2 text_origin = { 0, -(dimensions->line_size / 2.0f) + (text_dimensions.y / 2.0f) };
    Vector2 position = {
        .x = state->game_center_x + (dimensions->x_fract * 2),
        .y = state->game_top + dimensions->header_size + (dimensions->line_size * line_idx)
    };
    DrawTextPro(
        state->bird_computer.font,
        text,
        position,
        text_origin,
        0.0f,
        dimensions->font_size,
        0.0f,
        BIRD_COMPUTER_TEXT_COLOR
    );
}

static void draw_info_box(State *state, Bird_Computer_Dimensions *dimensions, char *text) {
    Rectangle rec = {
        .x = state->game_left + dimensions->x_fract,
        .y = state->game_top + dimensions->y_fract,
        .width = state->game_width - (dimensions->x_fract * 2),
        .height = state->game_height - (dimensions->y_fract * 2),
    };
    DrawRectangleRec(rec, BIRD_COMPUTER_BG_COLOR);
    DrawRectangleLinesEx(rec, state->scale_multiplier, BIRD_COMPUTER_FG_COLOR);
    float base_y_position = rec.y + dimensions->y_fract;
    Vector2 text_position = { .x = rec.x + dimensions->x_fract, .y = base_y_position };
    Vector2 text_origin = {0};
    char buffer[64];
    int text_idx = 0;
    while (true) {
        int buffer_idx = 0;
        while (text[text_idx] != '\n' && text[text_idx] != '\0') {
            buffer[buffer_idx] = text[text_idx];
            text_idx++;
            buffer_idx++;
        }
        buffer[buffer_idx] = '\0';
        DrawTextPro(
            state->bird_computer.font,
            buffer,
            text_position,
            text_origin,
            0.0f,
            dimensions->font_size,
            0,
            BIRD_COMPUTER_TEXT_COLOR
        );
        text_position.y += dimensions->y_fract;
        if (text[text_idx] == '\0') {
            break;
        } else {
            text_idx++;
        }
    }
}

static void draw_scroll_bar(State *state, Bird_Computer_Dimensions *dimensions, int item_count, int item_area_offset) {
    float full_size = dimensions->line_section_size - dimensions->y_fract;
    Rectangle rec = {
        .y = dimensions->header_size + (dimensions->y_fract / 2),
        .width = dimensions->x_fract / 2.0f,
        .height = full_size
    };
    rec.x = state->game_center_x - (dimensions->x_fract);
    DrawRectangleLinesEx(rec, state->scale_multiplier, BIRD_COMPUTER_FG_COLOR);
    float scroll_bar_fract = full_size / item_count;
    rec.x += state->scale_multiplier;
    rec.y += (item_area_offset * scroll_bar_fract) + state->scale_multiplier;
    float scale_multiplier_x2 = state->scale_multiplier * 2;
    rec.width -= scale_multiplier_x2;
    rec.height = (BIRD_COMPUTER_LINE_COUNT * scroll_bar_fract) - scale_multiplier_x2;
    DrawRectangleRec(rec, BIRD_COMPUTER_FG_COLOR);
}

void bird_computer_init(State *state) {
    Bird_Computer *bird_computer = &state->bird_computer;
    bird_computer->font = LoadFont(BIRD_COMPUTER_FONT);
    bird_computer->option_count = 7;
}

void bird_computer_cleanup(State *state) {
    UnloadFont(state->bird_computer.font);
}

void bird_computer_level_setup(State *state) {
    Bird_Computer *bird_computer = &state->bird_computer;
    bird_computer->option_idx = 0;
    bird_computer->option_area_offset = 0;
    if (state->current_round == 0) {
        bird_computer->option_idx_results = -1;
        bird_computer->option_idx_continue = 0;
        bird_computer->option_idx_multipliers = 1;
        bird_computer->option_idx_shop = -1;
        bird_computer->option_count = 2;
    } else {
        bird_computer->option_idx_results = 0;
        bird_computer->option_idx_continue = 1;
        bird_computer->option_idx_multipliers = 2;
        bird_computer->option_idx_shop = 3;
        bird_computer->option_count = 4;
    }
}

void bird_computer_update(State *state) {
    Bird_Computer *bird_computer = &state->bird_computer;
    if (IsKeyPressed(KEY_DOWN) && bird_computer->option_idx < (bird_computer->option_count - 1)) {
        if (bird_computer->option_idx < BIRD_COMPUTER_LINE_COUNT - 1) {
            bird_computer->option_idx++;
        } else if (bird_computer->option_area_offset < (bird_computer->option_count - BIRD_COMPUTER_LINE_COUNT)) {
            bird_computer->option_area_offset++;
        }
    }
    if (IsKeyPressed(KEY_UP)) {
        if (bird_computer->option_idx > 0) {
            bird_computer->option_idx--;
        } else if (bird_computer->option_area_offset > 0) {
            bird_computer->option_area_offset--;
        }
    }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        switch (bird_computer->state) {
        case BIRD_COMPUTER_STATE_DEFAULT: {
            int option_idx_with_offset = bird_computer->option_idx + bird_computer->option_area_offset;
            if (option_idx_with_offset == bird_computer->option_idx_results ||
                option_idx_with_offset == bird_computer->option_idx_multipliers
            ) {
                bird_computer->state = BIRD_COMPUTER_STATE_INFO_BOX;
            } else if (option_idx_with_offset == bird_computer->option_idx_continue) {
                state->game_state = GAME_STATE_NEXT_LEVEL;
            }
        } break;
        case BIRD_COMPUTER_STATE_INFO_BOX: {
            bird_computer->state = BIRD_COMPUTER_STATE_DEFAULT;
        } break;
        }
    }
}

void bird_computer_render(State *state) {
    Bird_Computer *bird_computer = &state->bird_computer;
    const float y_fract = state->game_height / 16;
    const float line_section_size = y_fract * 11;
    Bird_Computer_Dimensions dimensions = {
        .font_size = (BIRD_COMPUTER_FONT_SIZE / bird_computer->font.baseSize) * state->scale_multiplier,
        .x_fract = state->game_width / 32,
        .y_fract = y_fract,
        .header_size = y_fract * 2,
        .line_section_size = line_section_size,
        .line_size = line_section_size / BIRD_COMPUTER_LINE_COUNT,
        .footer_size = y_fract * 3,
    };

    // background
    DrawRectangle(state->game_left, state->game_top, state->game_width, state->game_height, BIRD_COMPUTER_BG_COLOR);
    Rectangle rec = {
        .x = state->game_left,
        .y = dimensions.header_size,
        .width = state->game_width / 2,
        .height = dimensions.line_section_size
    };
    DrawRectangleLinesEx(rec, state->scale_multiplier, BIRD_COMPUTER_FG_COLOR);
    rec.x = state->game_left + (state->game_width / 2);
    DrawRectangleLinesEx(rec, state->scale_multiplier, BIRD_COMPUTER_FG_COLOR);
    {
        // cursor background
        float left_padding = (dimensions.x_fract * 1.7);
        float vertical_padding = 0.3f * dimensions.y_fract;
        Vector2 position = {
            .x = state->game_left + left_padding,
            .y = state->game_top
                + dimensions.header_size
                + (dimensions.line_size * bird_computer->option_idx)
                + vertical_padding
        };
        Vector2 size = {
            .x = (state->game_width / 2) - left_padding - (dimensions.x_fract * 2),
            .y = dimensions.line_size - (vertical_padding * 2)
        };
        DrawRectangleV(position, size, BIRD_COMPUTER_CURSOR_BG_COLOR);
    }

    if (bird_computer->option_count > BIRD_COMPUTER_LINE_COUNT) {
        draw_scroll_bar(
            state,
            &dimensions,
            bird_computer->option_count,
            bird_computer->option_area_offset
        );
    }

    draw_header_text(state, &dimensions, "Welcome to the bird computer");

    // options
    switch (bird_computer->state) {
        case BIRD_COMPUTER_STATE_DEFAULT: {
            for (int option_idx = bird_computer->option_area_offset; option_idx < bird_computer->option_count; option_idx++) {
                if (option_idx == bird_computer->option_idx_results) {
                    switch (state->current_level_data.environment) {
                        case LEVEL_ENVIRONMENT_NONE: {
                            draw_option_text(state, &dimensions, "Results from nothing", option_idx);
                        } break;
                        case LEVEL_ENVIRONMENT_FOREST: {
                            draw_option_text(state, &dimensions, "Forest round results", option_idx);
                        } break;
                        case LEVEL_ENVIRONMENT_MEADOWS: {
                            draw_option_text(state, &dimensions, "Meadows round results", option_idx);
                        } break;
                        case LEVEL_ENVIRONMENT_MOUNTAINS: {
                            draw_option_text(state, &dimensions, "Mountains round results", option_idx);
                        } break;
                    }
                    if (option_idx == bird_computer->option_idx) {
                        int info_line_idx = 0;
                        char buffer[32];
                        sprintf(buffer, "Current round: %i", state->current_round);
                        draw_info_text_line(state, &dimensions, buffer, info_line_idx);
                        info_line_idx++;
                        sprintf(buffer, "Round score: %i", state->player.level_score);
                        draw_info_text_line(state, &dimensions, buffer, info_line_idx);
                        info_line_idx++;
                        int obliterated_birds_total = 0;
                        for (int i = 0; i < BIRD_TYPES_TOTAL; i++) {
                            obliterated_birds_total += state->player.obliterated_birds[i];
                        }
                        sprintf(buffer, "Birds obliterated: %i/%i", obliterated_birds_total, state->current_level_data.total_birds);
                        draw_info_text_line(state, &dimensions, buffer, info_line_idx);
                    }
                } else if (option_idx == bird_computer->option_idx_continue) {
                    switch (state->next_level_data.environment) {
                        case LEVEL_ENVIRONMENT_NONE: {
                            draw_option_text(state, &dimensions, "Continue to nothing?", option_idx);
                        } break;
                        case LEVEL_ENVIRONMENT_FOREST: {
                            draw_option_text(state, &dimensions, "Continue to Forest", option_idx);
                        } break;
                        case LEVEL_ENVIRONMENT_MEADOWS: {
                            draw_option_text(state, &dimensions, "Continue to Meadows", option_idx);
                        } break;
                        case LEVEL_ENVIRONMENT_MOUNTAINS: {
                            draw_option_text(state, &dimensions, "Continue to Mountains", option_idx);
                        } break;
                    }
                    if (option_idx == bird_computer->option_idx) {
                        char buffer[32];
                        Tex tex;
                        switch (state->next_level_data.environment) {
                        case LEVEL_ENVIRONMENT_NONE: {
                            draw_info_text_line(state, &dimensions, "Nothing", 4);
                        }
                        case LEVEL_ENVIRONMENT_FOREST: {
                            tex = TEX_ENV_NIGHT_SKY;
                            draw_info_text_line(state, &dimensions, "Forest", 4);
                        } break;
                        case LEVEL_ENVIRONMENT_MEADOWS: {
                            tex = TEX_ENV_DAY_SKY;
                            draw_info_text_line(state, &dimensions, "Meadows", 4);
                        } break;
                        case LEVEL_ENVIRONMENT_MOUNTAINS: {
                            tex = TEX_ENV_WINTER_SKY;
                            draw_info_text_line(state, &dimensions, "Mountains", 4);
                        } break;
                        }
                        Vector2 position = {
                            .x = state->game_center_x + (state->game_width / 4),
                            .y = state->game_top + dimensions.header_size + (dimensions.y_fract * 3.5)
                        };
                        for (int i = 0; i < state->next_level_data.scroll_env_amount; i++) {
                            tex_atlas_draw_raw(state, tex + i, position, 0, 0.6f);
                        }
                        sprintf(buffer, "Upcoming birds: %i", state->next_level_data.total_birds);
                        draw_info_text_line(state, &dimensions, buffer, 5);
                        sprintf(buffer, "Bird frequency: %.2f/sec", state->next_level_data.bird_frequency);
                        draw_info_text_line(state, &dimensions, buffer, 6);
                    }
                } else if (option_idx == bird_computer->option_idx_multipliers) {
                    draw_option_text(state, &dimensions, "Highest multipliers", option_idx);
                    if (option_idx == bird_computer->option_idx) {
                        int info_line_idx = 0;
                        char buffer[32];
                        for (int i = 0; i < BIRD_COMPUTER_LINE_COUNT; i++) {
                            if (state->player.highest_multipliers[i] == 0) {
                                break;
                            }
                            int multiplier_amount = state->player.highest_multipliers[i];
                            sprintf(buffer, "%ix (%i score)", multiplier_amount, multiplier_amount * multiplier_amount);
                            draw_info_text_line(state, &dimensions, buffer, info_line_idx);
                            info_line_idx++;
                        }
                        if (info_line_idx == 0) {
                            draw_info_text_line(state, &dimensions, "No multipliers", info_line_idx);
                        }
                    }
                } else if (option_idx == bird_computer->option_idx_shop) {
                    switch (state->current_level_data.environment) {
                        case LEVEL_ENVIRONMENT_NONE: {
                            draw_option_text(state, &dimensions, "The empty shop", option_idx);
                        } break;
                        case LEVEL_ENVIRONMENT_FOREST: {
                            draw_option_text(state, &dimensions, "The Forest Shop", option_idx);
                        } break;
                        case LEVEL_ENVIRONMENT_MEADOWS: {
                            draw_option_text(state, &dimensions, "The Meadow Shop", option_idx);
                        } break;
                        case LEVEL_ENVIRONMENT_MOUNTAINS: {
                            draw_option_text(state, &dimensions, "The Mountain Shop", option_idx);
                        } break;
                    }
                }
            }
            draw_option_cursor(state, &dimensions);
        } break;
        case BIRD_COMPUTER_STATE_INFO_BOX: {
            if (bird_computer->option_idx_results == bird_computer->option_idx) {
                char *text =
                    "You are definitely a bad person\n"
                    "and that is true";
                draw_info_box(state, &dimensions, text);
            } else if (bird_computer->option_idx_multipliers == bird_computer->option_idx) {
                char *text =
                    "When you smash a bunch of birds in a row\n"
                    "without touching the ground or ceiling\n"
                    "you get a score multiplier:\n"
                    "2x-Multiplier: 4 score\n"
                    "3x-Multiplier: 9 score\n"
                    "4x-Multiplier: 16 score\n"
                    "5x-Multiplier: 25 score\n"
                    "6x-Multiplier: 36 score\n"
                    "7x-Multiplier: 49 score\n"
                    "9x-Multiplier: 64 score\n"
                    "9x-Multiplier: 81 score\n"
                    "Max-Multiplier: 100 score";
                draw_info_box(state, &dimensions, text);
            }
        } break;
    }
}