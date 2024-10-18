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

typedef enum Option_Type {
    OPTION_TYPE_RESULTS,
    OPTION_TYPE_CONTINUE,
    OPTION_TYPE_SHOP,
    OPTION_TYPE_MULTIPLIERS,
} Option_Type;

static void draw_default_menu_background(State *state, Menu_Dimensions *dimensions) {
    DrawRectangleRec(game_rectangle(state), MENU_BG_COLOR);
    char *header = "Welcome to the bird computer";
    Vector2 text_dimensions = MeasureTextEx(state->menu_font, header, dimensions->font_size, 0.0f);
    Vector2 text_origin = (Vector2){ 0, -(dimensions->header_size / 2.0f) + (text_dimensions.y / 2.0f) };
    Vector2 position = {
        .x = state->game_center_x - (text_dimensions.x / 2),
        .y = state->game_top
    };
    DrawTextPro(state->menu_font, header, position, text_origin, 0.0f, dimensions->font_size, 0.0f, MENU_TEXT_COLOR);
    { // foreground lines
        float line_thickness = state->scale_multiplier;
        Vector2 start = {state->game_left, state->game_top + dimensions->header_size};
        Vector2 end = {state->game_right, start.y};
        DrawLineEx(start, end, line_thickness, MENU_FG_COLOR);
        start.x = state->game_center_x;
        end.x = start.x;
        end.y = state->game_bottom;
        DrawLineEx(start, end, line_thickness, MENU_FG_COLOR);
    }
}

static void cursor_movement(State *state, uint8 *idx, uint8 *offset, uint8 count) {
    if (IsKeyPressed(KEY_DOWN) && *idx < (count - 1)) {
        if (*idx < MENU_LINE_COUNT - 1) {
            (*idx)++;
        } else if (*offset < (count - MENU_LINE_COUNT)) {
            (*offset)++;
        }
    }
    if (IsKeyPressed(KEY_UP)) {
        if (*idx > 0) {
            (*idx)--;
        } else if (*offset > 0) {
            (*offset)--;
        }
    }
}

static void draw_text(State *state, Menu_Dimensions *dimensions, char *text, int line_idx, Vector2 position) {
    if (line_idx < 0 || line_idx >= MENU_LINE_COUNT) {
        return;
    }
    Vector2 text_dimensions = MeasureTextEx(state->menu_font, text, dimensions->font_size, 0.0f);
    Vector2 text_origin = { 0, -(dimensions->line_size / 2.0f) + (text_dimensions.y / 2.0f) };
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
    Vector2 position = {
        .x = state->game_left + (dimensions->x_fract * 2),
        .y = state->game_top + dimensions->header_size + (dimensions->line_size * line_idx)
    };
    draw_text(state, dimensions, text, line_idx, position);
}

static void draw_suboption_text(State *state, Menu_Dimensions *dimensions, char *text, int line_idx) {
    Vector2 position = {
        .x = state->game_center_x + (dimensions->x_fract * 2),
        .y = state->game_top + dimensions->header_size + (dimensions->line_size * line_idx)
    };
    draw_text(state, dimensions, text, line_idx, position);
}

static void draw_cursor(State *state, Menu_Dimensions *dimensions, float x_pivot, float line_idx) {
    { // background
        float left_padding = (dimensions->x_fract * 1.7);
        float vertical_padding = 0.3f * dimensions->y_fract;
        Vector2 position = {
            .x = x_pivot + left_padding,
            .y = state->game_top
                + dimensions->header_size
                + (dimensions->line_size * line_idx)
                + vertical_padding
        };
        Vector2 size = {
            .x = (state->game_width / 2) - left_padding - (dimensions->x_fract * 2),
            .y = dimensions->line_size - (vertical_padding * 2)
        };
        DrawRectangleV(position, size, MENU_CURSOR_BG_COLOR);
    }
    char *cursor = ">";
    Vector2 text_dimensions = MeasureTextEx(state->menu_font, cursor, dimensions->font_size, 0.0f);
    Vector2 origin = { 0, -(dimensions->line_size / 2.0f) + (text_dimensions.y / 2.0f) };
    Vector2 position = {
        .x = x_pivot + dimensions->x_fract,
        .y = state->game_top + dimensions->header_size + (dimensions->line_size * line_idx)
    };
    DrawTextPro(state->menu_font, cursor, position, origin, 0.0f, dimensions->font_size, 0.0f, MENU_TEXT_COLOR);
}

static void draw_option_cursor(State *state, Menu_Dimensions *dimensions) {
    draw_cursor(state, dimensions, state->game_left, state->menu_option_idx);
}

static void draw_suboption_cursor(State *state, Menu_Dimensions *dimensions) {
    draw_cursor(state, dimensions, state->game_center_x, state->menu_suboption_idx);
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
        DrawTextPro(state->menu_font, buffer, text_position, text_origin, 0.0f, dimensions->font_size, 0, MENU_TEXT_COLOR);
        text_position.y += dimensions->y_fract;
        if (text[text_idx] == '\0') {
            break;
        } else {
            text_idx++;
        }
    }
}

static void draw_shop_buy_box(State *state, Menu_Dimensions *dimensions, char *item_name) {
    Rectangle rec = {
        .x = state->game_left + state->game_width * 0.2f,
        .y = state->game_top + state->game_height * 0.3f,
        .width = state->game_width * 0.6f,
        .height = state->game_height * 0.4f,
    };
    DrawRectangleRec(rec, MENU_BG_COLOR);
    DrawRectangleLinesEx(rec, state->scale_multiplier, MENU_FG_COLOR);
    float base_y_position = rec.y + dimensions->y_fract;
    Vector2 text_position = { .x = rec.x + dimensions->x_fract, .y = base_y_position };
    Vector2 text_origin = {0};
    char buffer[32];
    sprintf(buffer, "Buy the %s?", item_name);
    DrawTextPro(state->menu_font, buffer, text_position, text_origin, 0.0f, dimensions->font_size, 0, MENU_TEXT_COLOR);

    Vector2 position;
    { // yes/no background
        float left_padding = (dimensions->x_fract * 1.7);
        float top_padding = rec.height * 0.7f;
        switch (state->menu_state) {
        default: break;
        case MENU_STATE_SHOP_BUY_YES: position.x = rec.x + left_padding; break;
        case MENU_STATE_SHOP_BUY_NO: position.x = rec.x + left_padding + (rec.width / 2); break;
        }
        position.y = rec.y + top_padding;
        Vector2 size = {
            .x = (rec.width / 3),
            .y = dimensions->y_fract
        };
        DrawRectangleV(position, size, MENU_CURSOR_BG_COLOR);
    }
    char *cursor = ">";
    Vector2 text_dimensions = MeasureTextEx(state->menu_font, cursor, dimensions->font_size, 0.0f);
    Vector2 origin = { 0, -(dimensions->line_size / 2.0f) + (text_dimensions.y / 2.0f) };
    DrawTextPro(state->menu_font, cursor, position, origin, 0.0f, dimensions->font_size, 0.0f, MENU_TEXT_COLOR);
    DrawTextPro(state->menu_font, "Yes", text_position, text_origin, 0.0f, dimensions->font_size, 0, MENU_TEXT_COLOR);
    DrawTextPro(state->menu_font, "No", text_position, text_origin, 0.0f, dimensions->font_size, 0, MENU_TEXT_COLOR);
}

static void draw_scroll_bar(State *state, Menu_Dimensions *dimensions, int item_count, int item_area_offset, float x_position) {
    float full_size = dimensions->line_section_size - dimensions->y_fract;
    Rectangle rec = {
        .x = state->game_center_x - (dimensions->x_fract),
        .y = dimensions->header_size + (dimensions->y_fract / 2),
        .width = dimensions->x_fract / 2.0f,
        .height = full_size
    };
    DrawRectangleLinesEx(rec, state->scale_multiplier, MENU_FG_COLOR);
    float scroll_bar_fract = full_size / item_count;
    rec.x += state->scale_multiplier;
    rec.y += (item_area_offset * scroll_bar_fract) + state->scale_multiplier;
    float scale_multiplier_x2 = state->scale_multiplier * 2;
    rec.width -= scale_multiplier_x2;
    rec.height = (MENU_LINE_COUNT * scroll_bar_fract) - scale_multiplier_x2;
    DrawRectangleRec(rec, MENU_FG_COLOR);
}

static void draw_option_scroll_bar(State *state, Menu_Dimensions *dimensions, int item_count, int item_area_offset) {
    float x = state->game_center_x - (dimensions->x_fract);
    draw_scroll_bar(state, dimensions, item_count, item_area_offset, x);
}

static void draw_suboption_scroll_bar(State *state, Menu_Dimensions *dimensions, int item_count, int item_area_offset) {
    float x = state->game_right - (dimensions->x_fract);
    draw_scroll_bar(state, dimensions, item_count, item_area_offset, x);
}

static void get_option_text(char *buffer, Option_Type option_type) {
    switch (option_type) {
    case OPTION_TYPE_RESULTS: {
        sprintf(buffer, "Level results");
    } break;
    case OPTION_TYPE_CONTINUE: {
        sprintf(buffer, "Go to next level");
    } break;
    case OPTION_TYPE_SHOP: {
        sprintf(buffer, "Bird warehouse");
    } break;
    case OPTION_TYPE_MULTIPLIERS: {
        sprintf(buffer, "Highest multipliers");
    } break;
    }
}

static void view_option_data(State *state, Menu_Dimensions *dimensions, Option_Type option_type) {
    switch (option_type) {
    case OPTION_TYPE_RESULTS: {
        int info_line_idx = 0;
        char buffer[32];
        sprintf(buffer, "Level: %i", state->level_idx);
        draw_suboption_text(state, dimensions, buffer, info_line_idx);
        info_line_idx++;
        sprintf(buffer, "Score: %i/%i", state->level_score, state->level_current_data.required_score);
        draw_suboption_text(state, dimensions, buffer, info_line_idx);
        info_line_idx++;
        int obliterated_birds_total = 0;
        for (int i = 0; i < BIRD_TYPES_TOTAL; i++) {
            obliterated_birds_total += state->birds_destroyed[i];
        }
        sprintf(buffer, "Birds obliterated: %i/%i", obliterated_birds_total, state->level_current_data.total_birds);
        draw_suboption_text(state, dimensions, buffer, info_line_idx);
        info_line_idx++;
        int red_birds_destroyed = 0;
        for (int i = AREA_FOREST; i <= AREA_MOUNTAINS; i++) {
            int flag = 1 << i;
            if (has_flag(state->red_birds_destroyed_bits, flag)) {
                red_birds_destroyed++;
            }
        };
        sprintf(buffer, "Red birds: %i/3", red_birds_destroyed);
        draw_suboption_text(state, dimensions, buffer, info_line_idx);
    } break;
    case OPTION_TYPE_CONTINUE: {
        float map_size = state->game_width / 2 - (dimensions->x_fract * 4);
        float map_line_size = 2.0f * state->scale_multiplier;
        float map_cell_size = map_size / 3;
        float tex_cell_size = GAME_WIDTH_RATIO / (state->game_width / (map_cell_size - (map_line_size * 2)));
        float map_left = state->game_center_x + (dimensions->x_fract * 2);
        float map_top = state->game_top + dimensions->header_size + (dimensions->x_fract * 2);
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
                        dimensions->font_size,
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
                        dimensions->font_size,
                        0.0f,
                        WHITE
                    );
                }
            }
        }
    } break;
    case OPTION_TYPE_SHOP: {
        if (state->menu_state == MENU_STATE_SHOP) {
            draw_suboption_cursor(state, dimensions);
        }
        int item_idx = 0;
        for (int i = 0; i < SHOP_ITEM_TOTAL; i++) {
            if (state->shop_items[i] != SHOP_ITEM_STATE_AVAILABLE) {
                continue;
            }
            switch (i) {
            default: {
                draw_suboption_text(state, dimensions, "Unknown item", item_idx);
            } break;
            case SHOP_ITEM_PORTAL_YELLOW: {
                draw_suboption_text(state, dimensions, "Yellow Portal", item_idx);
            } break;
            case SHOP_ITEM_PORTAL_WHITE: {
                draw_suboption_text(state, dimensions, "White Portal", item_idx);
            } break;
            }
            item_idx++;
        }
        state->menu_suboption_count = item_idx;
    } break;
    case OPTION_TYPE_MULTIPLIERS: {
        int info_line_idx = 0;
        char buffer[32];
        for (int i = 0; i < MENU_LINE_COUNT; i++) {
            if (state->bird_highest_multipliers[i] == 0) {
                break;
            }
            int multiplier_amount = state->bird_highest_multipliers[i];
            sprintf(buffer, "%ix (%i score)", multiplier_amount, multiplier_amount * multiplier_amount);
            draw_suboption_text(state, dimensions, buffer, info_line_idx);
            info_line_idx++;
        }
        if (info_line_idx == 0) {
            draw_suboption_text(state, dimensions, "No multipliers", info_line_idx);
        }
    } break;
    }
}

static int get_active_shop_item_idx(State *state) {
    int idx_available = -1;
    for (int i = 0; i < SHOP_ITEM_TOTAL; i++) {
        if (state->shop_items[i] == SHOP_ITEM_STATE_AVAILABLE) {
            idx_available++;
            if (i == state->menu_suboption_idx) {
                return idx_available;
            }
        }
    }
    return -1;
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
    state->menu_option_offset = 0;
    state->menu_option_count = 4;
    state->areas_discovered_bits |= (1 << (state->next_level_data.area - 1));
    state->shop_items[SHOP_ITEM_PORTAL_YELLOW] = SHOP_ITEM_STATE_AVAILABLE;
    state->shop_items[SHOP_ITEM_PORTAL_WHITE] = SHOP_ITEM_STATE_AVAILABLE;
}

void menu_game_over_setup(State *state) {
    state->menu_state = MENU_STATE_GAME_OVER;
}

void menu_update(State *state) {
    if (state->menu_state == MENU_STATE_GAME_OVER) {
        return;
    }

    bool select = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE);
    bool left = IsKeyPressed(KEY_LEFT);
    bool right = IsKeyPressed(KEY_RIGHT);

    switch (state->menu_state) {
    default:
    case MENU_STATE_DEFAULT: {
        cursor_movement(state, &state->menu_option_idx, &state->menu_option_offset, state->menu_option_count);
        if (select) {
            switch (state->menu_option_idx) {
            default: state->menu_state = MENU_STATE_INFO_BOX; break;
            case OPTION_TYPE_CONTINUE: state->global_state = GLOBAL_STATE_MENU_FADE_OUT; break;
            case OPTION_TYPE_SHOP: state->menu_state = MENU_STATE_SHOP; break;
            }
        } else if (right) {
            switch (state->menu_option_idx) {
            default: break;
            case OPTION_TYPE_SHOP: state->menu_state = MENU_STATE_SHOP; break;
            }
        }
    } break;
    case MENU_STATE_INFO_BOX: {
        if (select) {
            state->menu_state = MENU_STATE_DEFAULT;
        }
    } break;
    case MENU_STATE_SHOP: {
        cursor_movement(state, &state->menu_suboption_idx, &state->menu_suboption_offset, state->menu_suboption_count);
        if (left || IsKeyPressed(KEY_BACKSPACE)) {
            state->menu_state = MENU_STATE_DEFAULT;
        }
        if (select) {
            state->menu_state = MENU_STATE_SHOP_BUY_NO;
        }
    } break;
    case MENU_STATE_SHOP_BUY_YES: {
        if (select) {
            state->menu_state = MENU_STATE_SHOP;
        }
        if (right) {
            state->menu_state = MENU_STATE_SHOP_BUY_NO;
        }
    } break;
    case MENU_STATE_SHOP_BUY_NO: {
        if (select) {
            state->menu_state = MENU_STATE_SHOP;
        }
        if (left) {
            state->menu_state = MENU_STATE_SHOP_BUY_YES;
        }
    } break;
    }
}

void menu_render(State *state) {
    float y_fract = state->game_height / 16;
    float line_section_size = y_fract * 11;
    Menu_Dimensions dimensions = {
        .font_size = (MENU_FONT_SIZE / state->menu_font.baseSize) * state->scale_multiplier,
        .x_fract = state->game_width / 32,
        .y_fract = y_fract,
        .header_size = y_fract * 2,
        .line_section_size = line_section_size,
        .line_size = line_section_size / MENU_LINE_COUNT,
        .footer_size = y_fract * 3,
    };

    switch (state->menu_state) {
    default:
    case MENU_STATE_DEFAULT: {
        draw_default_menu_background(state, &dimensions);
        if (state->menu_option_count > MENU_LINE_COUNT) {
            draw_option_scroll_bar(
                state,
                &dimensions,
                state->menu_option_count,
                state->menu_option_offset
            );
        }
        draw_option_cursor(state, &dimensions);
        char buffer[32];
        for (int option_idx = state->menu_option_offset; option_idx < state->menu_option_count; option_idx++) {
            get_option_text(buffer, option_idx);
            draw_option_text(state, &dimensions, buffer, option_idx);
            if (option_idx == state->menu_option_idx) {
                view_option_data(state, &dimensions, option_idx);
            }
        }
    } break;
    case MENU_STATE_INFO_BOX: {
        DrawRectangleRec(game_rectangle(state), BLACK);
        char *text = "You are definitely a bad\nperson and that is true";
        draw_info_box(state, &dimensions, text);
    } break;
    case MENU_STATE_SHOP: {
        draw_default_menu_background(state, &dimensions);
        view_option_data(state, &dimensions, OPTION_TYPE_SHOP);
    } break;
    case MENU_STATE_SHOP_BUY_YES:
    case MENU_STATE_SHOP_BUY_NO: {
        DrawRectangleRec(game_rectangle(state), BLACK);
        int item_idx = get_active_shop_item_idx(state);
        switch (item_idx) {
        case SHOP_ITEM_PORTAL_YELLOW: draw_shop_buy_box(state, &dimensions, "yellow portal"); break;
        case SHOP_ITEM_PORTAL_WHITE: draw_shop_buy_box(state, &dimensions, "white portal"); break;
        }
    } break;
    case MENU_STATE_GAME_OVER: {
        DrawRectangleRec(game_rectangle(state), BLACK);
        char *text = "You are a horrible person";
        Vector2 text_dimensions = MeasureTextEx(state->menu_font, text, dimensions.font_size, 0.0f);
        Vector2 origin = {0};
        Vector2 position = {
            .x = state->game_center_x - (text_dimensions.x / 2),
            .y = state->game_center_y - (text_dimensions.y / 2)
        };
        DrawTextPro(state->menu_font, text, position, origin, 0.0f, dimensions.font_size, 0.0f, WHITE);
    } break;
    }
}