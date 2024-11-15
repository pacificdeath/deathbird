#include "raylib.h"
#include "main.h"

typedef enum Option {
    OPTION_RESULTS,
    OPTION_CONTINUE,
    OPTION_SHOP,
    OPTION_MULTIPLIERS,
    OPTION_TOTAL
} Option;

static void draw_default_menu_background(State *state) {
    Menu *menu = &state->menu;

    DrawRectangle(menu->left, menu->top, menu->width, menu->height, MENU_BG_COLOR);
    char *header = "The Bird Computer";
    Vector2 header_menu = MeasureTextEx(menu->font, header, menu->font_size, 0.0f);
    Vector2 header_origin = { (header_menu.x / 2.0f), (header_menu.y / 2.0f) };
    Vector2 header_position = {
        .x = menu->center_x,
        .y = menu->top + (menu->header_size / 2.0f)
    };
    DrawTextPro(menu->font, header, header_position, header_origin, 0.0f, menu->font_size, 0.0f, MENU_TEXT_COLOR);
    { // foreground lines
        float line_thickness = state->scale_multiplier;
        Vector2 start = {menu->left, menu->top + menu->header_size};
        Vector2 end = {menu->right, start.y};
        DrawLineEx(start, end, line_thickness, MENU_FG_COLOR);
        start.x = menu->center_x;
        end.x = start.x;
        end.y = start.y + menu->line_section_size;
        DrawLineEx(start, end, line_thickness, MENU_FG_COLOR);
        start.y = end.y;
        start.x = menu->left;
        end.x = menu->right;
        DrawLineEx(start, end, line_thickness, MENU_FG_COLOR);
    }
    char uranium_text[32];
    sprintf(uranium_text, "Uranium: %u", menu->uranium);
    Vector2 uranium_origin = { 0, (header_menu.y / 2.0f) };
    Vector2 uranium_position = {
        .x = menu->left + menu->x_fract,
        .y = menu->top + menu->header_size + menu->line_section_size + (menu->footer_size / 2)
    };
    DrawTextPro(menu->font, uranium_text, uranium_position, uranium_origin, 0.0f, menu->font_size, 0.0f, MENU_TEXT_COLOR);
}

static void cursor_movement(uint8 *idx, uint8 *offset, uint8 count) {
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

static void draw_text(State *state, char *text, int line_idx, Vector2 position) {
    Menu *menu = &state->menu;

    if (line_idx < 0 || line_idx >= MENU_LINE_COUNT) {
        return;
    }
    Vector2 text_menu = MeasureTextEx(menu->font, text, menu->font_size, 0.0f);
    Vector2 text_origin = { 0, -(menu->line_size / 2.0f) + (text_menu.y / 2.0f) };
    DrawTextPro(menu->font, text, position, text_origin, 0.0f, menu->font_size, 0.0f, MENU_TEXT_COLOR);
}

static void draw_option_text(State *state, char *text, int line_idx) {
    Menu *menu = &state->menu;

    Vector2 position = {
        .x = menu->left + (menu->width * 0.1f),
        .y = menu->top + menu->header_size + (menu->line_size * line_idx)
    };
    draw_text(state, text, line_idx, position);
}

static void draw_suboption_text(State *state, char *text, int line_idx) {
    Menu *menu = &state->menu;

    Vector2 position = {
        .x = menu->center_x + (menu->width * 0.1f),
        .y = menu->top + menu->header_size + (menu->line_size * line_idx)
    };
    draw_text(state, text, line_idx, position);
}

static void draw_cursor(State *state, float x_pivot, float line_idx) {
    Menu *menu = &state->menu;

    { // background
        float left_padding = (menu->width * 0.05f);
        float vertical_padding = 0.3f * menu->y_fract;
        Vector2 position = {
            .x = x_pivot + left_padding,
            .y = menu->top
                + menu->header_size
                + (menu->line_size * line_idx)
                + vertical_padding
        };
        Vector2 size = {
            .x = (menu->width / 2) - left_padding - (menu->x_fract * 2),
            .y = menu->line_size - (vertical_padding * 2)
        };
        DrawRectangleV(position, size, MENU_CURSOR_BG_COLOR);
    }
    char *cursor = ">";
    Vector2 text_menu = MeasureTextEx(menu->font, cursor, menu->font_size, 0.0f);
    Vector2 origin = { 0, -(menu->line_size / 2.0f) + (text_menu.y / 2.0f) };
    Vector2 position = {
        .x = x_pivot + menu->x_fract,
        .y = menu->top + menu->header_size + (menu->line_size * line_idx)
    };
    DrawTextPro(menu->font, cursor, position, origin, 0.0f, menu->font_size, 0.0f, MENU_TEXT_COLOR);
}

static void draw_suboption_highlight(State *state, float line_idx) {
    Menu *menu = &state->menu;

    float left_padding = (menu->x_fract * 1.7);
    float vertical_padding = 0.3f * menu->y_fract;
    Rectangle rec = {
        .x = menu->center_x + left_padding,
        .y = menu->top
            + menu->header_size
            + (menu->line_size * line_idx)
            + vertical_padding,
        .width = (menu->width / 2) - left_padding - (menu->x_fract * 2),
        .height = menu->line_size - (vertical_padding * 2)
    };
    DrawRectangleLinesEx(rec, menu->decor_line_size, MENU_HIGHLIGHT_COLOR);
}

static void draw_option_cursor(State *state) {
    Menu *menu = &state->menu;

    draw_cursor(state, menu->left, menu->option_idx);
}

static void draw_suboption_cursor(State *state) {
    Menu *menu = &state->menu;

    draw_cursor(state, menu->center_x, menu->suboption_idx);
}

static void draw_info_box(State *state, char *text, int anim_frames) {
    Menu *menu = &state->menu;

    Rectangle rec = {
        .x = menu->left + menu->width * 0.25f,
        .y = menu->top + menu->height * 0.25f,
        .width = menu->width / 2,
        .height = menu->height / 2,
    };
    DrawRectangleRec(rec, MENU_BG_COLOR);
    DrawRectangleLinesEx(rec, menu->decor_line_size, MENU_FG_COLOR);
    float base_y_position = rec.y + (rec.height * 0.25f);
    Vector2 text_position = { menu->center_x, base_y_position };
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
        Vector2 text_menu = MeasureTextEx(menu->font, buffer, menu->font_size, 0);
        Vector2 text_origin = { text_menu.x / 2, text_menu.y / 2 };
        DrawTextPro(menu->font, buffer, text_position, text_origin, 0.0f, menu->font_size, 0, MENU_TEXT_COLOR);
        text_position.y += menu->y_fract;
        if (text[text_idx] == '\0') {
            break;
        } else {
            text_idx++;
        }
    }
    if (anim_frames > 0) {
        atlas_draw(state, menu->anim[menu->anim_frame], (Vector2){0}, 0.0f, 1.0f, OPAQUE);
        menu->anim_timer += state->delta_time;
        if (menu->anim_timer > GAME_ANIM_SPEED) {
            menu->anim_frame = (menu->anim_frame + 1) % anim_frames;
            menu->anim_timer = 0.0f;
        }
    }

    float left_padding = (menu->x_fract * 6.0f);
    float top_padding = rec.height * 0.75f;

    Vector2 cursor_bg_size = {
        .x = rec.width - (left_padding * 2.0f),
        .y = menu->y_fract
    };

    Vector2 text_offset = { cursor_bg_size.x / 2, cursor_bg_size.y / 2 };

    char *ok_text = "Ok";
    Vector2 ok_menu = MeasureTextEx(menu->font, ok_text, menu->font_size, 0.0f);
    Vector2 ok_position = { rec.x + left_padding, rec.y + top_padding };
    Vector2 ok_origin = { -text_offset.x + (ok_menu.x / 2), -text_offset.y + (ok_menu.y / 2) };

    char *cursor = ">";
    Vector2 cursor_menu = MeasureTextEx(menu->font, ok_text, menu->font_size, 0.0f);
    Vector2 cursor_origin = { (cursor_menu.x * 1.0f), -text_offset.y + (cursor_menu.y / 2) };

    DrawRectangleV(ok_position, cursor_bg_size, MENU_CURSOR_BG_COLOR);
    DrawTextPro(menu->font, cursor, ok_position, cursor_origin, 0.0f, menu->font_size, 0.0f, MENU_TEXT_COLOR);
    DrawTextPro(menu->font, ok_text, ok_position, ok_origin, 0.0f, menu->font_size, 0, MENU_TEXT_COLOR);
}

static void draw_shop_buy_box(State *state, char *item_name) {
    Menu *menu = &state->menu;

    Rectangle rec = {
        .x = menu->left + menu->width * 0.2f,
        .y = menu->top + menu->height * 0.3f,
        .width = menu->width * 0.6f,
        .height = menu->height * 0.4f,
    };
    DrawRectangleRec(rec, MENU_BG_COLOR);
    DrawRectangleLinesEx(rec, state->scale_multiplier, MENU_FG_COLOR);

    char custom_text[32];
    sprintf(custom_text, "Buy the %s?", item_name);
    Vector2 custom_text_menu = MeasureTextEx(menu->font, custom_text, menu->font_size, 0.0f);
    Vector2 custom_text_position = { rec.x + (rec.width / 2), rec.y + (rec.height * 0.3f) };
    Vector2 custom_text_origin = { custom_text_menu.x / 2, custom_text_menu.y / 2 };
    DrawTextPro(menu->font, custom_text, custom_text_position, custom_text_origin, 0.0f, menu->font_size, 0, MENU_TEXT_COLOR);

    float left_padding = (menu->x_fract * 2.0f);
    float top_padding = rec.height * 0.7f;

    Vector2 cursor_bg_size = {
        .x = (rec.width / 2) - (left_padding * 2.0f),
        .y = menu->y_fract
    };

    Vector2 text_offset = { cursor_bg_size.x / 2, cursor_bg_size.y / 2 };

    char *yes_text = "Yes";
    Vector2 yes_menu = MeasureTextEx(menu->font, yes_text, menu->font_size, 0.0f);
    Vector2 yes_position = { rec.x + left_padding, rec.y + top_padding };
    Vector2 yes_origin = { -text_offset.x + (yes_menu.x / 2), -text_offset.y + (yes_menu.y / 2) };

    char *no_text = "No";
    Vector2 no_menu = MeasureTextEx(menu->font, no_text, menu->font_size, 0.0f);
    Vector2 no_position = { rec.x + left_padding + (rec.width / 2), rec.y + top_padding };
    Vector2 no_origin = { -text_offset.x + (no_menu.x / 2), -text_offset.y + (no_menu.y / 2) };

    char *cursor = ">";
    Vector2 cursor_menu = MeasureTextEx(menu->font, no_text, menu->font_size, 0.0f);
    Vector2 cursor_origin = { (cursor_menu.x * 1.0f), -text_offset.y + (cursor_menu.y / 2) };

    switch (menu->state) {
    default: break;
    case MENU_STATE_SHOP_BUY_YES: {
        DrawRectangleV(yes_position, cursor_bg_size, MENU_CURSOR_BG_COLOR);
        DrawTextPro(menu->font, cursor, yes_position, cursor_origin, 0.0f, menu->font_size, 0.0f, MENU_TEXT_COLOR);
    } break;
    case MENU_STATE_SHOP_BUY_NO: {
        DrawRectangleV(no_position, cursor_bg_size, MENU_CURSOR_BG_COLOR);
        DrawTextPro(menu->font, cursor, no_position, cursor_origin, 0.0f, menu->font_size, 0.0f, MENU_TEXT_COLOR);
    } break;
    }
    DrawTextPro(menu->font, yes_text, yes_position, yes_origin, 0.0f, menu->font_size, 0, MENU_TEXT_COLOR);
    DrawTextPro(menu->font, no_text, no_position, no_origin, 0.0f, menu->font_size, 0, MENU_TEXT_COLOR);
}

static void draw_scroll_bar(State *state, int item_count, int item_area_offset, float x_position) {
    Menu *menu = &state->menu;

    float full_size = menu->line_section_size - menu->y_fract;
    Rectangle rec = {
        .x = menu->center_x - (menu->x_fract),
        .y = menu->header_size + (menu->y_fract / 2),
        .width = menu->x_fract / 2.0f,
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

static void draw_option_scroll_bar(State *state, int item_count, int item_area_offset) {
    Menu *menu = &state->menu;

    float x = menu->center_x - (menu->x_fract);
    draw_scroll_bar(state, item_count, item_area_offset, x);
}

static void draw_suboption_scroll_bar(State *state, int item_count, int item_area_offset) {
    Menu *menu = &state->menu;

    float x = menu->right - (menu->x_fract);
    draw_scroll_bar(state, item_count, item_area_offset, x);
}

static void get_option_text(char *buffer, Option option_type) {
    switch (option_type) {
    case OPTION_RESULTS: {
        sprintf(buffer, "Level results");
    } break;
    case OPTION_CONTINUE: {
        sprintf(buffer, "Just play the game");
    } break;
    case OPTION_SHOP: {
        sprintf(buffer, "Bird warehouse");
    } break;
    case OPTION_MULTIPLIERS: {
        sprintf(buffer, "Highest multipliers");
    } break;
    }
}

static int get_active_notification(State *state) {
    for (int i = 0; i < MENU_NOTIFICATION_TOTAL; i++) {
        int flag = 1 << i;
        if (has_flag(state->menu.notifications, flag)) {
            return flag;
        }
    }
    return 0;
}

static int count_shop_items(State *state) {
    int count = 0;
    for (int i = 0; i < SHOP_ITEM_TOTAL; i++) {
        if (state->menu.shop_items[i] == SHOP_ITEM_STATE_AVAILABLE) {
            count++;
        }
    }
    return count;
}

static int get_active_shop_item(State *state) {
    Shop_Item idx_available = -1;
    for (int i = 0; i < SHOP_ITEM_TOTAL; i++) {
        if (state->menu.shop_items[i] == SHOP_ITEM_STATE_AVAILABLE) {
            idx_available++;
            if (idx_available == state->menu.suboption_idx) {
                return i;
            }
        }
    }
    return -1;
}

static int get_shop_item_cost(State *state, Shop_Item item) {
    switch (item) {
    default: return 0;
    case SHOP_ITEM_BOOST_OF_HASTE: return (state->player.vertical_speed * 100.0f);
    case SHOP_ITEM_BIRD_FOOD: return 100;
    }
}

static bool is_item_owned(State *state, Shop_Item item) {
    return state->menu.shop_items[item] == SHOP_ITEM_STATE_OWNED;
}

static int get_active_option(State* state) {
    int idx = -1;
    for (int i = 0; i < OPTION_TOTAL; i++) {
        int flag = 1 << i;
        if (has_flag(state->menu.available_options, flag)) {
            idx++;
        }
        if (state->menu.option_idx == i) {
            return idx;
        }
    }
    return -1;
}

static void enable_option(State *state, Option option) {
    Option flag = 1 << option;
    state->menu.available_options |= flag;
    state->menu.option_count++;
}

static void view_option_data(State *state, Option option_type) {
    Menu *menu = &state->menu;

    switch (option_type) {
    case OPTION_RESULTS: {
        int info_line_idx = 0;
        char buffer[32];
        sprintf(buffer, "Level: %u", state->level_idx);
        draw_suboption_text(state, buffer, info_line_idx);
        info_line_idx++;
        sprintf(buffer, "Score: %i", state->level_score);
        draw_suboption_text(state, buffer, info_line_idx);
        info_line_idx++;
        int obliterated_birds_total = 0;
        for (int i = 0; i < BIRD_TYPES_TOTAL; i++) {
            obliterated_birds_total += state->birds_destroyed[i];
        }
        sprintf(buffer, "Birds obliterated: %i", obliterated_birds_total);
        draw_suboption_text(state, buffer, info_line_idx);
        info_line_idx++;
        int red_birds_destroyed = 0;
        for (int i = AREA_FOREST; i <= AREA_DEEP_FOREST; i++) {
            int flag = 1 << i;
            if (has_flag(state->red_birds_destroyed_bits, flag)) {
                red_birds_destroyed++;
            }
        };
        sprintf(buffer, "Red birds: %i/3", red_birds_destroyed);
        draw_suboption_text(state, buffer, info_line_idx);
    } break;
    case OPTION_CONTINUE: {
        float map_size = menu->width / 2 - (menu->x_fract * 4);
        float map_line_size = 2.0f * state->scale_multiplier;
        float map_cell_size = map_size / 3;
        float tex_cell_size = GAME_WIDTH_RATIO / (menu->width / (map_cell_size - (map_line_size * 2)));
        float map_left = menu->center_x + (menu->x_fract * 2);
        float map_top = menu->top + menu->header_size + (menu->x_fract * 2);
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
                    Vector2 unknwon_text_menu = MeasureTextEx(
                        menu->font,
                        unknown_text,
                        menu->font_size,
                        0
                    );
                    Vector2 unknown_text_position = {
                        .x = outer_rec.x + (map_cell_size / 2) - unknwon_text_menu.x / 2,
                        .y = outer_rec.y + (map_cell_size / 2) - unknwon_text_menu.y / 2
                    };
                    DrawTextPro(
                        menu->font,
                        unknown_text,
                        unknown_text_position,
                        (Vector2){0},
                        0.0f,
                        menu->font_size,
                        0.0f,
                        WHITE
                    );
                }
            }
        }
    } break;
    case OPTION_SHOP: {
        if (menu->state == MENU_STATE_SHOP) {
            draw_suboption_cursor(state);
        }
        int item_idx = 0;
        for (int i = 0; i < SHOP_ITEM_TOTAL; i++) {
            if (menu->shop_items[i] != SHOP_ITEM_STATE_AVAILABLE) {
                continue;
            }
            char buffer[32];
            switch (i) {
            default: {
                sprintf(buffer, "Unknown item");
            } break;
            case SHOP_ITEM_BOOST_OF_HASTE: {
                sprintf(buffer, "%s: %i", "Boost of Haste", get_shop_item_cost(state, i));
            } break;
            case SHOP_ITEM_BIRD_FOOD: {
                sprintf(buffer, "%s: %i", "Bird food", get_shop_item_cost(state, i));
            } break;
            }
            draw_suboption_text(state, buffer, item_idx);
            item_idx++;
        }
        menu->suboption_count = item_idx;
    } break;
    case OPTION_MULTIPLIERS: {
        int info_line_idx = 0;
        char buffer[32];
        for (int i = 0; i < MENU_LINE_COUNT; i++) {
            if (state->bird_highest_multipliers[i] == 0) {
                break;
            }
            int multiplier_amount = state->bird_highest_multipliers[i];
            sprintf(buffer, "%ix (%i score)", multiplier_amount, multiplier_amount * multiplier_amount);
            draw_suboption_text(state, buffer, info_line_idx);
            info_line_idx++;
        }
        if (info_line_idx == 0) {
            draw_suboption_text(state, "No multipliers", info_line_idx);
        }
    } break;
    }
}

void menu_update_dimensions(State *state) {
    Menu *menu = &state->menu;
    menu->font_size = (MENU_FONT_SIZE / menu->font.baseSize) * state->scale_multiplier;
    float game_x_fract = state->game_width * 0.01f;
    float game_y_fract = state->game_height * 0.03f;

    float x_padding = game_x_fract * 8.0f;
    float y_padding = game_y_fract * 8.0f;

    menu->width = state->game_width - (2.0f * x_padding);
    menu->height = state->game_height - (2.0f * y_padding);

    menu->x_fract = menu->width * 0.02f;
    menu->y_fract = menu->height * 0.03f;

    menu->left = state->game_left + x_padding;
    menu->right = menu->left + menu->width;

    menu->top = state->game_top + y_padding;
    menu->bottom = menu->top + menu->height;

    menu->center_x = menu->left + (menu->width * 0.5f);
    menu->center_y = menu->top + (menu->height * 0.5f);

    menu->header_size = menu->height * 0.1f;
    menu->line_section_size = menu->height * 0.8f;
    menu->line_size = menu->line_section_size / MENU_LINE_COUNT;
    menu->footer_size = menu->height * 0.1f;
    menu->decor_line_size = 5;
}

void menu_add_notification(State *state, enum Menu_Notification notification) {
    Menu_Notification flag = 1 << notification;
    state->menu.notifications |= flag;
}

void menu_init(State *state) {
    Menu *menu = &state->menu;
    menu->font = LoadFont(MENU_FONT);
    menu->option_count = 0;
    enable_option(state, OPTION_RESULTS);
    enable_option(state, OPTION_CONTINUE);
    enable_option(state, OPTION_SHOP);
    enable_option(state, OPTION_MULTIPLIERS);
    menu->boost_of_haste_level = 1;
    menu->bird_food_level = 1;

    menu_update_dimensions(state);
}

void menu_cleanup(State *state) {
    Menu *menu = &state->menu;
    UnloadFont(menu->font);
}

void menu_level_setup(State *state) {
    Menu *menu = &state->menu;

    menu->option_idx = 0;
    menu->option_offset = 0;
    state->areas_discovered_bits |= (1 << (state->next_level_data.area - 1));
    menu->shop_items[SHOP_ITEM_BOOST_OF_HASTE] = SHOP_ITEM_STATE_AVAILABLE;
    menu->shop_items[SHOP_ITEM_BIRD_FOOD] = SHOP_ITEM_STATE_AVAILABLE;
    menu->anim_timer = 0.0f;
    menu->anim_frame = 0;

    menu->uranium += state->birds_destroyed[BIRD_TYPE_WHITE] * 1;
    menu->uranium += state->birds_destroyed[BIRD_TYPE_YELLOW] * 1;
    menu->uranium += state->birds_destroyed[BIRD_TYPE_UMBRELLA_ABOVE] * 2;
    menu->uranium += state->birds_destroyed[BIRD_TYPE_UMBRELLA_UNDER] * 2;
    menu->uranium += state->birds_destroyed[BIRD_TYPE_BROWN] * 1;
    menu->uranium += state->birds_destroyed[BIRD_TYPE_GIANT] * 5;
    menu->uranium += state->birds_destroyed[BIRD_TYPE_RED] * 10;

    if (state->birds_destroyed[BIRD_TYPE_RED] > 0) {
        menu_add_notification(state, MENU_NOTIFICATION_RED_BIRD_DESTROYED);
    }
}

void menu_game_over_setup(State *state) {
    Menu *menu = &state->menu;

    menu->option_idx = 0;
    menu->option_offset = 0;
    state->areas_discovered_bits = 1 << (AREA_FOREST - 1);
    menu->anim_timer = 0.0f;
    menu->anim_frame = 0;
    menu->uranium = 0;
    menu->state = MENU_STATE_GAME_OVER;
}

void menu_update(State *state) {
    Menu *menu = &state->menu;

    #if DEBUG
    int number_key = KEY_ONE;
    for (int i = 0; i < 3; i++) {
        if (IsKeyPressed(number_key + i)) {
            state->next_level_data.area = AREA_FOREST + i;
        }
    }
    #endif


    bool select = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE);

    if (menu->state == MENU_STATE_GAME_OVER) {
        if (select) {
            menu->state = MENU_STATE_DEFAULT;
        } else {
            return;
        }
    }

    bool left = IsKeyPressed(KEY_LEFT);
    bool right = IsKeyPressed(KEY_RIGHT);

    switch (menu->state) {
    default:
    case MENU_STATE_NOTIFICATION: {
        int notification = get_active_notification(state);
        if (!notification) {
            menu->state = MENU_STATE_DEFAULT;
        } else if (select) {
            menu->notifications &= ~notification;
        }
    } break;
    case MENU_STATE_DEFAULT: {
        cursor_movement(&menu->option_idx, &menu->option_offset, menu->option_count);
        int active_option = get_active_option(state);
        if (select) {
            switch (active_option) {
            default: break;
            case OPTION_CONTINUE: {
                state->global_state = GLOBAL_STATE_GAME;
            } break;
            case OPTION_SHOP: {
                if (count_shop_items(state) > 0) {
                    menu->state = MENU_STATE_SHOP;
                }
            } break;
            }
        } else if (right) {
            switch (active_option) {
            default: break;
            case OPTION_SHOP: {
                if (count_shop_items(state) > 0) {
                    menu->state = MENU_STATE_SHOP;
                }
            } break;
            }
        }
    } break;
    case MENU_STATE_SHOP: {
        cursor_movement(&menu->suboption_idx, &menu->suboption_offset, menu->suboption_count);
        if (left || IsKeyPressed(KEY_BACKSPACE)) {
            menu->state = MENU_STATE_DEFAULT;
        }
        if (select) {
            Shop_Item item = get_active_shop_item(state);
            int cost = get_shop_item_cost(state, item);
            if (cost > menu->uranium) {
                menu->state = MENU_STATE_SHOP_TOO_EXPENSIVE;
            } else {
                menu->state = MENU_STATE_SHOP_BUY_NO;
            }
        }
    } break;
    case MENU_STATE_SHOP_BUY_YES: {
        if (select) {
            Shop_Item item = get_active_shop_item(state);
            menu->uranium -= get_shop_item_cost(state, item);
            menu->shop_items[item] = SHOP_ITEM_STATE_OWNED;
            int shop_item_count = count_shop_items(state);
            if (shop_item_count == 0) {
                menu->state = MENU_STATE_DEFAULT;
            } else {
                menu->state = MENU_STATE_SHOP;
                if (menu->suboption_idx == shop_item_count) {
                    menu->suboption_idx--;
                }
            }
        }
        if (right) {
            menu->state = MENU_STATE_SHOP_BUY_NO;
        }
    } break;
    case MENU_STATE_SHOP_BUY_NO: {
        if (select) {
            menu->state = MENU_STATE_SHOP;
        }
        if (left) {
            menu->state = MENU_STATE_SHOP_BUY_YES;
        }
    } break;
    case MENU_STATE_SHOP_TOO_EXPENSIVE: {
        if (select) {
            menu->state = MENU_STATE_SHOP;
        }
    } break;
    }
}

void menu_render(State *state) {
    Menu *menu = &state->menu;

    DrawRectangle(state->game_left, state->game_top, state->game_width, state->game_height, MENU_BG_OVERLAY_COLOR);

    switch (menu->state) {
    default:
    case MENU_STATE_NOTIFICATION: {
        switch (get_active_notification(state)) {
        case 1 << MENU_NOTIFICATION_RED_BIRD_DESTROYED: {
            int anim_frames = 4;
            for (int i = 0; i < anim_frames; i++) {
                menu->anim[i] = TEX_RED_BIRD_1 + i;
            }
            draw_info_box(state, "A red bird was destroyed", anim_frames);
        } break;
        }
    } break;
    case MENU_STATE_DEFAULT: {
        draw_default_menu_background(state);
        if (menu->option_count > MENU_LINE_COUNT) {
            draw_option_scroll_bar(state, menu->option_count, menu->option_offset);
        }
        draw_option_cursor(state);
        char buffer[32];
        for (int option_idx = menu->option_offset; option_idx < menu->option_count; option_idx++) {
            get_option_text(buffer, option_idx);
            draw_option_text(state, buffer, option_idx);
            if (option_idx == menu->option_idx) {
                view_option_data(state, option_idx);
            }
        }
    } break;
    case MENU_STATE_SHOP: {
        draw_default_menu_background(state);
        view_option_data(state, OPTION_SHOP);
    } break;
    case MENU_STATE_SHOP_BUY_YES:
    case MENU_STATE_SHOP_BUY_NO: {
        Shop_Item item = get_active_shop_item(state);
        char item_name[32];
        switch (item) {
        case SHOP_ITEM_BOOST_OF_HASTE: TextCopy(item_name, "Boost of Haste"); break;
        case SHOP_ITEM_BIRD_FOOD: TextCopy(item_name, "Bird Food"); break;
        }
        draw_shop_buy_box(state, item_name);
    } break;
    case MENU_STATE_SHOP_TOO_EXPENSIVE: {
        draw_info_box(state, "This thing is too expensive", 0);
    } break;
    case MENU_STATE_GAME_OVER: {
        DrawRectangle(menu->left, menu->top, menu->width, menu->height, MENU_BG_OVERLAY_COLOR);
        char *text = "You are a horrible person";
        Vector2 text_menu = MeasureTextEx(menu->font, text, menu->font_size, 0.0f);
        Vector2 origin = {0};
        Vector2 position = {
            .x = menu->left - (text_menu.x / 2),
            .y = menu->top - (text_menu.y / 2)
        };
        DrawTextPro(menu->font, text, position, origin, 0.0f, menu->font_size, 0.0f, WHITE);
    } break;
    }
}