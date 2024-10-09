#include "raylib.h"
#include "main.h"

static void draw_header_text(State *state, Bird_Computer_Dimensions *dimensions, char *text) {
    const Vector2 text_dimensions = MeasureTextEx(state->bird_computer.font, text, dimensions->font_size, 0.0f);
    Vector2 text_origin = (Vector2){ 0, -(dimensions->header_size / 2.0f) + (text_dimensions.y / 2.0f) };
    Vector2 position = {
        .x = state->game_center_x - (text_dimensions.x / 2),
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

static void draw_info_text_line(State *state, Bird_Computer_Dimensions *dimensions, char *text, int line_idx) {
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
        bird_computer->option_idx_multipliers = -1;
        bird_computer->option_count = 1;
    } else {
        bird_computer->option_idx_results = 0;
        bird_computer->option_idx_continue = 1;
        bird_computer->option_idx_multipliers = 2;
        bird_computer->option_count = 3;
    }
    bird_computer->discovered_levels |= (1 << (state->next_level_data.environment - 1));
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
        default:
        case BIRD_COMPUTER_STATE_DEFAULT: {
            int option_idx_with_offset = bird_computer->option_idx + bird_computer->option_area_offset;
            if (option_idx_with_offset == bird_computer->option_idx_results ||
                option_idx_with_offset == bird_computer->option_idx_multipliers
            ) {
                bird_computer->state = BIRD_COMPUTER_STATE_INFO_BOX;
            } else if (option_idx_with_offset == bird_computer->option_idx_continue) {
                state->game_state = GAME_STATE_BIRD_COMPUTER_FADE_OUT;
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
        .y = state->game_top + dimensions.header_size,
        .width = state->game_width / 2,
        .height = dimensions.line_section_size
    };
    DrawRectangleLinesEx(rec, state->scale_multiplier, BIRD_COMPUTER_FG_COLOR);
    rec.x = state->game_center_x;
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
        default:
        case BIRD_COMPUTER_STATE_DEFAULT: {
            for (int option_idx = bird_computer->option_area_offset; option_idx < bird_computer->option_count; option_idx++) {
                if (option_idx == bird_computer->option_idx_results) {
                    draw_option_text(state, &dimensions, "Current round results", option_idx);
                    if (option_idx == bird_computer->option_idx) {
                        int info_line_idx = 0;
                        char buffer[32];
                        sprintf(buffer, "Current round: %i", state->current_round);
                        draw_info_text_line(state, &dimensions, buffer, info_line_idx);
                        info_line_idx++;
                        sprintf(buffer, "Round score: %i/%i", state->player.level_score, state->current_level_data.required_score);
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
                    draw_option_text(state, &dimensions, "Start next round", option_idx);
                    if (option_idx == bird_computer->option_idx) {
                        float map_size = state->game_width / 2 - (dimensions.x_fract * 4);
                        float map_line_size = 2.0f * state->scale_multiplier;
                        float map_cell_size = map_size / 3;
                        float tex_cell_size = GAME_WIDTH_RATIO / (state->game_width / (map_cell_size - (map_line_size * 2)));
                        float map_left = state->game_center_x + (dimensions.x_fract * 2);
                        float map_top = state->game_top + dimensions.header_size + (dimensions.x_fract * 2);
                        for (int map_row = 0; map_row < 3; map_row++) {
                            for (int map_col = 0; map_col < 3; map_col++) {
                                int cell_idx = (map_row * 3) + map_col;
                                bool is_map_location = cell_idx == (state->next_level_data.environment - 1);
                                Tex tex;
                                int tex_amount = 0;
                                uint16 discovered_level_bits = (1 << cell_idx);
                                if ((bird_computer->discovered_levels & discovered_level_bits) == discovered_level_bits) {
                                    switch (cell_idx) {
                                    case 0: {
                                        tex = TEX_ENV_NIGHT_SKY;
                                        tex_amount = 4;
                                    } break;
                                    case 1: {
                                        tex = TEX_ENV_DAY_SKY;
                                        tex_amount = 4;
                                    } break;
                                    case 2: {
                                        tex = TEX_ENV_WINTER_SKY;
                                        tex_amount = 5;
                                    } break;
                                    }
                                }
                                Rectangle outer_rec = {
                                    .x = map_left + (map_cell_size * map_col),
                                    .y = map_top + (map_cell_size * map_row),
                                    .width = map_cell_size,
                                    .height = map_cell_size
                                };
                                if (is_map_location) {
                                    DrawRectangleLinesEx(outer_rec, map_line_size, BIRD_COMPUTER_ACTIVE_LEVEL_COLOR);
                                    float cell_2 = map_cell_size / 2;
                                    float cell_8 = map_cell_size / 8;
                                    {
                                        // left arrow
                                        Vector2 v1 = { map_left - cell_8, map_top + cell_2 + (map_row * map_cell_size) };
                                        Vector2 v2 = { v1.x - cell_8, v1.y - cell_8 };
                                        Vector2 v3 = { v2.x, v1.y + cell_8 };
                                        DrawTriangle(v1, v2, v3, BIRD_COMPUTER_ACTIVE_LEVEL_COLOR);
                                    }
                                    {
                                        // top arrow
                                        Vector2 v1 = { map_left + cell_2 + (map_col * map_cell_size), map_top - cell_8 };
                                        Vector2 v2 = { v1.x + cell_8, v1.y - cell_8 };
                                        Vector2 v3 = { v1.x - cell_8, v2.y };
                                        DrawTriangle(v1, v2, v3, BIRD_COMPUTER_ACTIVE_LEVEL_COLOR);
                                    }
                                }
                                Vector2 tex_position = {
                                    .x = map_left + map_line_size + (map_cell_size * map_col),
                                    .y = map_top + map_line_size + (map_cell_size * map_row),
                                };
                                if (tex_amount > 0) {
                                    for (int i = 0; i < tex_amount; i++) {
                                        tex_atlas_draw_raw(state, tex + i, tex_position, 0, tex_cell_size);
                                    }
                                } else {
                                    Rectangle inner_rec = {
                                        .x = map_left + map_line_size + (map_cell_size * map_col),
                                        .y = map_top + map_line_size + (map_cell_size * map_row),
                                        .width = map_cell_size - (map_line_size * 2),
                                        .height = map_cell_size - (map_line_size * 2)
                                    };
                                    DrawRectangleRec(inner_rec, BLACK);
                                    char *unknown_text = "???";
                                    Vector2 unknwon_text_dimensions = MeasureTextEx(
                                        bird_computer->font,
                                        unknown_text,
                                        dimensions.font_size,
                                        0
                                    );
                                    Vector2 unknown_text_position = {
                                        .x = outer_rec.x + (map_cell_size / 2) - unknwon_text_dimensions.x / 2,
                                        .y = outer_rec.y + (map_cell_size / 2) - unknwon_text_dimensions.y / 2
                                    };
                                    DrawTextPro(
                                        bird_computer->font,
                                        unknown_text,
                                        unknown_text_position,
                                        (Vector2){0},
                                        0.0f,
                                        dimensions.font_size,
                                        0.0f,
                                        WHITE
                                    );
                                }
                            }
                        }
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
            }
        } break;
    }
}