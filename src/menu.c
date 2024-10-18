#include "raylib.h"
#include "main.h"

typedef struct Menu_Dimensions {
    float font_size;
    float x_fract;
    float y_fract;
    float header_size;
    float line_section_size;
    float footer_size;
    float line_size;
} Menu_Dimensions;

static void draw_header_text(State *state, Menu_Dimensions *dimensions, char *text) {
    const Vector2 text_dimensions = MeasureTextEx(state->menu_font, text, dimensions->font_size, 0.0f);
    Vector2 text_origin = (Vector2){ 0, -(dimensions->header_size / 2.0f) + (text_dimensions.y / 2.0f) };
    Vector2 position = {
        .x = state->game_center_x - (text_dimensions.x / 2),
        .y = state->game_top
    };
    DrawTextPro(
        state->menu_font,
        text,
        position,
        text_origin,
        0.0f,
        dimensions->font_size,
        0.0f,
        MENU_TEXT_COLOR
    );
}

static void draw_option_text(State *state, Menu_Dimensions *dimensions, char *text, int line_idx) {
    if (line_idx < 0 || line_idx >= MENU_LINE_COUNT) {
        return;
    }
    const Vector2 text_dimensions = MeasureTextEx(state->menu_font, text, dimensions->font_size, 0.0f);
    Vector2 text_origin = { 0, -(dimensions->line_size / 2.0f) + (text_dimensions.y / 2.0f) };
    Vector2 position = {
        .x = state->game_left + (dimensions->x_fract * 2),
        .y = state->game_top + dimensions->header_size + (dimensions->line_size * line_idx)
    };
    DrawTextPro(
        state->menu_font,
        text,
        position,
        text_origin,
        0.0f,
        dimensions->font_size,
        0.0f,
        MENU_TEXT_COLOR
    );
}

static void draw_option_cursor(State *state, Menu_Dimensions *dimensions) {
    char *cursor = ">";
    const Vector2 text_dimensions = MeasureTextEx(state->menu_font, cursor, dimensions->font_size, 0.0f);
    Vector2 text_origin = { 0, -(dimensions->line_size / 2.0f) + (text_dimensions.y / 2.0f) };
    Vector2 position = {
        .x = state->game_left + dimensions->x_fract,
        .y = state->game_top + dimensions->header_size + (dimensions->line_size * state->menu_option_idx)
    };
    DrawTextPro(
        state->menu_font,
        cursor,
        position,
        text_origin,
        0.0f,
        dimensions->font_size,
        0.0f,
        MENU_TEXT_COLOR
    );
}

static void draw_info_text_line(State *state, Menu_Dimensions *dimensions, char *text, int line_idx) {
    const Vector2 text_dimensions = MeasureTextEx(state->menu_font, text, dimensions->font_size, 0.0f);
    Vector2 text_origin = { 0, -(dimensions->line_size / 2.0f) + (text_dimensions.y / 2.0f) };
    Vector2 position = {
        .x = state->game_center_x + (dimensions->x_fract * 2),
        .y = state->game_top + dimensions->header_size + (dimensions->line_size * line_idx)
    };
    DrawTextPro(
        state->menu_font,
        text,
        position,
        text_origin,
        0.0f,
        dimensions->font_size,
        0.0f,
        MENU_TEXT_COLOR
    );
}

static void draw_info_box(State *state, Menu_Dimensions *dimensions, char *text) {
    Rectangle rec = {
        .x = state->game_left + state->game_width * 0.25f,
        .y = state->game_top + state->game_height * 0.25f,
        .width = state->game_width * 0.5f,
        .height = state->game_height * 0.5f,
    };
    DrawRectangleRec(rec, MENU_BG_COLOR);
    DrawRectangleLinesEx(rec, state->scale_multiplier, MENU_FG_COLOR);
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
            state->menu_font,
            buffer,
            text_position,
            text_origin,
            0.0f,
            dimensions->font_size,
            0,
            MENU_TEXT_COLOR
        );
        text_position.y += dimensions->y_fract;
        if (text[text_idx] == '\0') {
            break;
        } else {
            text_idx++;
        }
    }
}

static void draw_scroll_bar(State *state, Menu_Dimensions *dimensions, int item_count, int item_area_offset) {
    float full_size = dimensions->line_section_size - dimensions->y_fract;
    Rectangle rec = {
        .y = dimensions->header_size + (dimensions->y_fract / 2),
        .width = dimensions->x_fract / 2.0f,
        .height = full_size
    };
    rec.x = state->game_center_x - (dimensions->x_fract);
    DrawRectangleLinesEx(rec, state->scale_multiplier, MENU_FG_COLOR);
    float scroll_bar_fract = full_size / item_count;
    rec.x += state->scale_multiplier;
    rec.y += (item_area_offset * scroll_bar_fract) + state->scale_multiplier;
    float scale_multiplier_x2 = state->scale_multiplier * 2;
    rec.width -= scale_multiplier_x2;
    rec.height = (MENU_LINE_COUNT * scroll_bar_fract) - scale_multiplier_x2;
    DrawRectangleRec(rec, MENU_FG_COLOR);
}

void menu_init(State *state) {
    state->menu_font = LoadFont(MENU_FONT);
    state->menu_option_count = 7;
}

void menu_cleanup(State *state) {
    UnloadFont(state->menu_font);
}

void menu_level_setup(State *state) {
    state->menu_option_idx = 0;
    state->menu_option_area_offset = 0;
    if (state->level_idx == 0) {
        state->menu_option_idx_results = 0;
        state->menu_option_idx_continue = 1;
        state->menu_option_idx_multipliers = -1;
        state->menu_option_count = 2;
    } else {
        state->menu_option_idx_results = 0;
        state->menu_option_idx_continue = 1;
        state->menu_option_idx_multipliers = 2;
        state->menu_option_count = 3;
    }
    state->areas_discovered_bits |= (1 << (state->next_level_data.area - 1));
}

void menu_game_over_setup(State *state) {
    state->menu_state = MENU_STATE_GAME_OVER;
}

void menu_update(State *state) {
    if (state->menu_state == MENU_STATE_GAME_OVER) {
        return;
    }

    if (IsKeyPressed(KEY_DOWN) && state->menu_option_idx < (state->menu_option_count - 1)) {
        if (state->menu_option_idx < MENU_LINE_COUNT - 1) {
            state->menu_option_idx++;
        } else if (state->menu_option_area_offset < (state->menu_option_count - MENU_LINE_COUNT)) {
            state->menu_option_area_offset++;
        }
    }
    if (IsKeyPressed(KEY_UP)) {
        if (state->menu_option_idx > 0) {
            state->menu_option_idx--;
        } else if (state->menu_option_area_offset > 0) {
            state->menu_option_area_offset--;
        }
    }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        switch (state->menu_state) {
        default:
        case MENU_STATE_DEFAULT: {
            int option_idx_with_offset = state->menu_option_idx + state->menu_option_area_offset;
            if (option_idx_with_offset == state->menu_option_idx_results ||
                option_idx_with_offset == state->menu_option_idx_multipliers
            ) {
                state->menu_state = MENU_STATE_INFO_BOX;
            } else if (option_idx_with_offset == state->menu_option_idx_continue) {
                state->global_state = GLOBAL_STATE_MENU_FADE_OUT;
            }
        } break;
        case MENU_STATE_INFO_BOX: {
            state->menu_state = MENU_STATE_DEFAULT;
        } break;
        }
    }
}

void menu_render(State *state) {
    const float y_fract = state->game_height / 16;
    const float line_section_size = y_fract * 11;
    Menu_Dimensions dimensions = {
        .font_size = (MENU_FONT_SIZE / state->menu_font.baseSize) * state->scale_multiplier,
        .x_fract = state->game_width / 32,
        .y_fract = y_fract,
        .header_size = y_fract * 2,
        .line_section_size = line_section_size,
        .line_size = line_section_size / MENU_LINE_COUNT,
        .footer_size = y_fract * 3,
    };

    // options
    switch (state->menu_state) {
    default:
    case MENU_STATE_DEFAULT: {
        // background
        DrawRectangleRec(game_rectangle(state), MENU_BG_COLOR);

        { // foreground lines
            float line_thickness = state->scale_multiplier;
            Vector2 start = {state->game_left, state->game_top + dimensions.header_size};
            Vector2 end = {state->game_right, start.y};
            DrawLineEx(start, end, line_thickness, MENU_FG_COLOR);
            start.x = state->game_center_x;
            end.x = start.x;
            end.y = state->game_bottom;
            DrawLineEx(start, end, line_thickness, MENU_FG_COLOR);
        }

        { // cursor background
            float left_padding = (dimensions.x_fract * 1.7);
            float vertical_padding = 0.3f * dimensions.y_fract;
            Vector2 position = {
                .x = state->game_left + left_padding,
                .y = state->game_top
                    + dimensions.header_size
                    + (dimensions.line_size * state->menu_option_idx)
                    + vertical_padding
            };
            Vector2 size = {
                .x = (state->game_width / 2) - left_padding - (dimensions.x_fract * 2),
                .y = dimensions.line_size - (vertical_padding * 2)
            };
            DrawRectangleV(position, size, MENU_CURSOR_BG_COLOR);
        }

        if (state->menu_option_count > MENU_LINE_COUNT) {
            draw_scroll_bar(
                state,
                &dimensions,
                state->menu_option_count,
                state->menu_option_area_offset
            );
        }

        draw_header_text(state, &dimensions, "Welcome to the bird computer");

        for (int option_idx = state->menu_option_area_offset; option_idx < state->menu_option_count; option_idx++) {
            if (option_idx == state->menu_option_idx_results) {
                draw_option_text(state, &dimensions, "Level results", option_idx);
                if (option_idx == state->menu_option_idx) {
                    int info_line_idx = 0;
                    char buffer[32];
                    sprintf(buffer, "Level: %i", state->level_idx);
                    draw_info_text_line(state, &dimensions, buffer, info_line_idx);
                    info_line_idx++;
                    sprintf(buffer, "Score: %i/%i", state->level_score, state->level_current_data.required_score);
                    draw_info_text_line(state, &dimensions, buffer, info_line_idx);
                    info_line_idx++;
                    int obliterated_birds_total = 0;
                    for (int i = 0; i < BIRD_TYPES_TOTAL; i++) {
                        obliterated_birds_total += state->birds_destroyed[i];
                    }
                    sprintf(buffer, "Birds obliterated: %i/%i", obliterated_birds_total, state->level_current_data.total_birds);
                    draw_info_text_line(state, &dimensions, buffer, info_line_idx);
                    info_line_idx++;
                    int red_birds_destroyed = 0;
                    for (int i = AREA_FOREST; i <= AREA_MOUNTAINS; i++) {
                        int flag = 1 << i;
                        if (has_flag(state->red_birds_destroyed_bits, flag)) {
                            red_birds_destroyed++;
                        }
                    };
                    sprintf(buffer, "Red birds: %i/3", red_birds_destroyed);
                    draw_info_text_line(state, &dimensions, buffer, info_line_idx);
                }
            } else if (option_idx == state->menu_option_idx_continue) {
                draw_option_text(state, &dimensions, "Go to next level", option_idx);
                if (option_idx == state->menu_option_idx) {
                    float map_size = state->game_width / 2 - (dimensions.x_fract * 4);
                    float map_line_size = 2.0f * state->scale_multiplier;
                    float map_cell_size = map_size / 3;
                    float tex_cell_size = GAME_WIDTH_RATIO / (state->game_width / (map_cell_size - (map_line_size * 2)));
                    float map_left = state->game_center_x + (dimensions.x_fract * 2);
                    float map_top = state->game_top + dimensions.header_size + (dimensions.x_fract * 2);
                    for (int map_row = 0; map_row < 3; map_row++) {
                        for (int map_col = 0; map_col < 3; map_col++) {
                            int cell_idx = (map_row * 3) + map_col;
                            bool is_map_location = cell_idx == (state->next_level_data.area - 1);
                            Tex tex;
                            int tex_amount = 0;
                            uint16 discovered_level_bit = (1 << cell_idx);
                            if (has_flag(state->areas_discovered_bits, discovered_level_bit)) {
                                switch (cell_idx) {
                                case 0: {
                                    tex = TEX_AREA_NIGHT_SKY;
                                    tex_amount = 4;
                                } break;
                                case 1: {
                                    tex = TEX_AREA_DAY_SKY;
                                    tex_amount = 4;
                                } break;
                                case 2: {
                                    tex = TEX_AREA_WINTER_SKY;
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
                                DrawRectangleLinesEx(outer_rec, map_line_size, MENU_ACTIVE_LEVEL_COLOR);
                                float cell_2 = map_cell_size / 2;
                                float cell_8 = map_cell_size / 8;
                                {
                                    // left arrow
                                    Vector2 v1 = { map_left - cell_8, map_top + cell_2 + (map_row * map_cell_size) };
                                    Vector2 v2 = { v1.x - cell_8, v1.y - cell_8 };
                                    Vector2 v3 = { v2.x, v1.y + cell_8 };
                                    DrawTriangle(v1, v2, v3, MENU_ACTIVE_LEVEL_COLOR);
                                }
                                {
                                    // top arrow
                                    Vector2 v1 = { map_left + cell_2 + (map_col * map_cell_size), map_top - cell_8 };
                                    Vector2 v2 = { v1.x + cell_8, v1.y - cell_8 };
                                    Vector2 v3 = { v1.x - cell_8, v2.y };
                                    DrawTriangle(v1, v2, v3, MENU_ACTIVE_LEVEL_COLOR);
                                }
                            }
                            Vector2 tex_position = {
                                .x = map_left + map_line_size + (map_cell_size * map_col),
                                .y = map_top + map_line_size + (map_cell_size * map_row),
                            };
                            if (tex_amount > 0) {
                                Vector2 origin = {0};
                                for (int i = 0; i < tex_amount; i++) {
                                    atlas_draw_raw(state, tex + i, tex_position, 0, tex_cell_size, origin, OPAQUE);
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
                                    state->menu_font,
                                    unknown_text,
                                    dimensions.font_size,
                                    0
                                );
                                Vector2 unknown_text_position = {
                                    .x = outer_rec.x + (map_cell_size / 2) - unknwon_text_dimensions.x / 2,
                                    .y = outer_rec.y + (map_cell_size / 2) - unknwon_text_dimensions.y / 2
                                };
                                DrawTextPro(
                                    state->menu_font,
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
            } else if (option_idx == state->menu_option_idx_multipliers) {
                draw_option_text(state, &dimensions, "Highest multipliers", option_idx);
                if (option_idx == state->menu_option_idx) {
                    int info_line_idx = 0;
                    char buffer[32];
                    for (int i = 0; i < MENU_LINE_COUNT; i++) {
                        if (state->bird_highest_multipliers[i] == 0) {
                            break;
                        }
                        int multiplier_amount = state->bird_highest_multipliers[i];
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
    case MENU_STATE_INFO_BOX: {
        // background
        DrawRectangleRec(game_rectangle(state), BLACK);

        if (state->menu_option_idx_results == state->menu_option_idx) {
            char *text =
                "You are definitely a bad\n"
                "person and that is true";
            draw_info_box(state, &dimensions, text);
        }
    } break;
    case MENU_STATE_GAME_OVER: {
        // background
        DrawRectangleRec(game_rectangle(state), BLACK);

        char *text = "You are a horrible person";
        const Vector2 text_dimensions = MeasureTextEx(state->menu_font, text, dimensions.font_size, 0.0f);
        Vector2 text_origin = {0};
        Vector2 position = {
            .x = state->game_center_x - (text_dimensions.x / 2),
            .y = state->game_center_y - (text_dimensions.y / 2)
        };
        DrawTextPro(
            state->menu_font,
            text,
            position,
            text_origin,
            0.0f,
            dimensions.font_size,
            0.0f,
            WHITE
        );
    } break;
    }
}