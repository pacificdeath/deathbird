enum Option {
    OPTION_RESULTS,
    OPTION_PLAY,
    OPTION_FREEPLAY,
    OPTION_MULTIPLIERS,
    OPTION_TERMINAL,
} Option;

enum FreeplayOption {
    OPTION_FREEPLAY_FOREST,
    OPTION_FREEPLAY_MEADOWS,
    OPTION_FREEPLAY_MOUNTAINS,
    OPTION_FREEPLAY_INDUSTRIAL,
    OPTION_FREEPLAY_CASTLE,
};

static char* get_option_text(Menu *menu, int option) {
    switch (menu->state) {
    case MENU_STATE_DEFAULT:
        switch (option) {
        default: return "Horrible code";
        case OPTION_RESULTS: return "Level results";
        case OPTION_PLAY: return "Just play the game";
        case OPTION_FREEPLAY: return "Freeplay";
        case OPTION_MULTIPLIERS: return "Highest multipliers";
        case OPTION_TERMINAL: return "The Bird Terminal 1999";
        }
    case MENU_STATE_FREEPLAY:
        switch (option) {
        default: return "Horrible code";
        case OPTION_FREEPLAY_FOREST: return "Night of the Deathbird";
        case OPTION_FREEPLAY_MEADOWS: return "A Wonderful Day for a Picnic";
        case OPTION_FREEPLAY_MOUNTAINS: return "The Snow Level of Death";
        case OPTION_FREEPLAY_INDUSTRIAL: return "Deathfields";
        case OPTION_FREEPLAY_CASTLE: return "House of Birdbrain";
        }
    }
}

static char *get_description_text(Menu *menu) {
    switch (menu->state) {
    case MENU_STATE_DEFAULT:
        switch (menu->option_idx) {
        default: return "Horrible code";
        case OPTION_RESULTS: return "Level results";
        case OPTION_PLAY: return "Just play the game";
        case OPTION_FREEPLAY: return "Freeplay";
        case OPTION_MULTIPLIERS: return "Highest multipliers";
        case OPTION_TERMINAL: return "Manually handle the bird computer";
        }
    case MENU_STATE_FREEPLAY:
        switch (menu->option_idx) {
        default: return "Horrible code";
        case OPTION_FREEPLAY_FOREST: return "Night of the Deathbird";
        case OPTION_FREEPLAY_MEADOWS: return "A Wonderful Day for a Picnic";
        case OPTION_FREEPLAY_MOUNTAINS: return "The Snow Level of Death";
        case OPTION_FREEPLAY_INDUSTRIAL: return "Deathfields";
        case OPTION_FREEPLAY_CASTLE: return "House of Birdbrain";
        }
    }
}

static void draw_description(Menu *menu) {
    char *text = get_description_text(menu);
    Vector2 dimensions = MeasureTextEx(menu->font, text, menu->font_size, 0.0f);
    Vector2 origin = { dimensions.x / 2, dimensions.y / 2 };
    Vector2 position = {
        .x = menu->center_x,
        .y = menu->inner_rectangle.y + menu->line_section_size + menu->instruction_size + (menu->description_size / 2)
    };
    DrawTextPro(menu->font, text, position, origin, 0.0f, menu->font_size, 0.0f, MENU_OPTION_COLOR);
}

static void cursor_movement(Menu *menu) {
    const int half_options = (MENU_OPTION_COUNT / 2);
    if (menu->option_idx < half_options) {
        if (IsKeyPressed(KEY_DOWN) && menu->option_idx < half_options - 1) {
            menu->option_idx++;
        }
        if (IsKeyPressed(KEY_UP) && menu->option_idx > 0) {
            menu->option_idx--;
        }
        if (IsKeyPressed(KEY_RIGHT)) {
            menu->option_idx += half_options;
        }
    } else {
        if (IsKeyPressed(KEY_DOWN) && menu->option_idx < MENU_OPTION_COUNT - 1) {
            menu->option_idx++;
        }
        if (IsKeyPressed(KEY_UP) && menu->option_idx > half_options) {
            menu->option_idx--;
        }
        if (IsKeyPressed(KEY_LEFT)) {
            menu->option_idx -= half_options;
        }
    }
}

static Vector2 get_option_pivot(Menu *menu, int option_idx) {
    int half_options = (MENU_OPTION_COUNT / 2);

    if (option_idx < half_options) {
        // option is in the column to the left
        return (Vector2) {
            .x = menu->inner_rectangle.x + (menu->inner_rectangle.width * 0.1f),
            .y = menu->inner_rectangle.y + (menu->line_size * option_idx)
        };
    }

    // option is in the column to the right
    return (Vector2) {
        .x = menu->center_x + (menu->inner_rectangle.width * 0.1f),
        .y = menu->inner_rectangle.y + (menu->line_size * (option_idx - half_options))
    };
}

static void draw_option_text(Menu *menu, int option_idx) {
    char *text = get_option_text(menu, option_idx);
    Vector2 option_pivot = get_option_pivot(menu, option_idx);
    Color color = (option_idx == menu->option_idx) ? MENU_OPTION_SELECTED_COLOR : MENU_OPTION_COLOR;
    Vector2 position = {option_pivot.x, option_pivot.y + (menu->line_size / 2)};
    draw_text(menu, text, position, color);
}

static void draw_cursor(State *state, float option_idx) {
    Menu *menu = &state->menu;

    const float inner_rectangle_padding = (menu->outer_rectangle.width - menu->inner_rectangle.width) / 2;
    const float x_fract = menu->inner_rectangle.width * 0.01f;
    const float y_fract = menu->inner_rectangle.height * 0.05f;

    Vector2 option_pivot = get_option_pivot(menu, option_idx);
    float left_padding = (menu->inner_rectangle.width * 0.05f);
    float vertical_padding = 0.3f * y_fract;

    { // background
        Vector2 position = {
            .x = option_pivot.x - left_padding,
            .y = option_pivot.y + vertical_padding
        };
        Vector2 size = {
            .x = (menu->inner_rectangle.width / 2) - left_padding - (x_fract * 4),
            .y = menu->line_size - (vertical_padding * 2)
        };
        DrawRectangleV(position, size, MENU_CURSOR_BG_COLOR);
    }

    float horizontal_offset = option_idx >= (MENU_OPTION_COUNT / 2)
        ? state->game_center_x
        : inner_rectangle_padding;

    Vector2 position = {
        .x = menu->inner_rectangle.x + horizontal_offset + x_fract,
        .y = option_pivot.y + (menu->line_size / 2)
    };
    draw_text(menu, ">", position, MENU_FG_COLOR);
}

static void draw_option_cursor(State *state) {
    Menu *menu = &state->menu;
    draw_cursor(state, menu->option_idx);
}

void menu_update_dimensions(State *state) {
    Menu *menu = &state->menu;
    menu->font_size = (MENU_FONT_SIZE / menu->font.baseSize) * state->scale_multiplier * 1.2f;

    menu->outer_rectangle = (Rectangle) {
        state->game_left,
        state->game_top,
        state->game_width,
        state->game_height
    };

    menu->center_x = menu->outer_rectangle.x + (menu->outer_rectangle.width * 0.5f);

    menu->header_size = menu->outer_rectangle.height * 0.1f;

    const float body_padding = menu->outer_rectangle.width * 0.02f;
    menu->inner_rectangle = (Rectangle) {
        menu->outer_rectangle.x + body_padding,
        menu->outer_rectangle.y + menu->header_size,
        menu->outer_rectangle.width - (body_padding * 2),
        menu->outer_rectangle.height - menu->header_size - body_padding,
    };

    menu->line_section_size = menu->inner_rectangle.height * 0.7f;
    menu->line_size = menu->line_section_size / MENU_OPTION_COUNT * 2;

    menu->instruction_size = menu->inner_rectangle.height * 0.15f;
    menu->description_size = menu->inner_rectangle.height * 0.15f;
    menu->line_thickness = state->scale_multiplier;
}

void menu_init(State *state) {
    Menu *menu = &state->menu;
    menu->font = LoadFont(MENU_FONT);
    menu_update_dimensions(state);
}

void menu_cleanup(State *state) {
    Menu *menu = &state->menu;
    UnloadFont(menu->font);
}

void menu_level_setup(State *state) {
    Menu *menu = &state->menu;

    menu->option_idx = 0;
    state->areas_discovered_bits |= (1 << (state->area - 1));
    menu->anim_timer = 0.0f;
    menu->anim_frame = 0;
}

void menu_game_over_setup(State *state) {
    Menu *menu = &state->menu;

    menu->option_idx = 0;
    state->areas_discovered_bits = 1 << (AREA_FOREST - 1);
    menu->anim_timer = 0.0f;
    menu->anim_frame = 0;
    menu->state = MENU_STATE_GAME_OVER;
}

Area menu_update(State *state) {
    Menu *menu = &state->menu;

    bool activation = IsKeyPressed(KEY_ENTER);
    bool back = IsKeyPressed(KEY_BACKSPACE);

    cursor_movement(menu);

    switch (menu->state) {
    default:
    case MENU_STATE_DEFAULT: {
        if (IsKeyPressed(KEY_F10)) {
            state->global_state = GLOBAL_STATE_TERMINAL;
            // TODO: bad code
            terminal_setup(state);
        }
        if (activation) {
            switch (menu->option_idx) {
                default: break;
                case OPTION_PLAY: return AREA_FOREST;
                case OPTION_FREEPLAY: menu->state = MENU_STATE_FREEPLAY; break;
                case OPTION_TERMINAL: {
                    state->global_state = GLOBAL_STATE_TERMINAL;
                    // TODO: bad code
                    terminal_setup(state);
                    break;
                }
            }
            menu->option_idx = 0;
        }
    } break;
    case MENU_STATE_FREEPLAY: {
        if (back) {
            menu->state = MENU_STATE_DEFAULT;
        } else if (activation) {
            switch (menu->option_idx) {
            default: return AREA_NONE;
            case OPTION_FREEPLAY_FOREST: return AREA_FOREST;
            case OPTION_FREEPLAY_MEADOWS: return AREA_MEADOWS;
            case OPTION_FREEPLAY_MOUNTAINS: return AREA_MOUNTAINS;
            case OPTION_FREEPLAY_INDUSTRIAL: return AREA_INDUSTRIAL;
            case OPTION_FREEPLAY_CASTLE: return AREA_CASTLE;
            }
            menu->option_idx = 0;
        }
    } break;
    }
    return AREA_NONE;
}

static void draw_instruction_text(Menu *menu) {
    const float y_fract = menu->instruction_size / 3.0f;
    const float x_fract = (menu->inner_rectangle.width * 0.05f);
    Vector2 position = {
        .x = menu->inner_rectangle.x + x_fract,
        .y = menu->inner_rectangle.y + menu->line_section_size + y_fract
    };

    draw_text(menu, "ESC    : Quit", position, MENU_FG_COLOR);
    position.y += y_fract;
    draw_text(menu, "Arrows : Select Item", position, MENU_FG_COLOR);

    switch (menu->state) {
    case MENU_STATE_DEFAULT:
        position.y -= y_fract;
        position.x = menu->center_x + x_fract;
        draw_text(menu, "F10       : Terminal", position, MENU_FG_COLOR);
        break;
    case MENU_STATE_FREEPLAY:
        position.y -= y_fract;
        position.x = menu->center_x + x_fract;
        draw_text(menu, "Backspace : Go back", position, MENU_FG_COLOR);
        break;
    }
}

static void draw_default_menu_background(State *state) {
    Menu *menu = &state->menu;

    DrawRectangleRec(menu->outer_rectangle, MENU_BG_COLOR);

    char *header = "Bird Computer - Sloppyright (S) 1999-2005 Deathbird Systems";
    Vector2 header_menu = MeasureTextEx(menu->font, header, menu->font_size, 0.0f);
    Vector2 header_origin = { (header_menu.x / 2.0f), (header_menu.y / 2.0f) };
    Vector2 header_position = {
        .x = menu->center_x,
        .y = menu->outer_rectangle.y + (menu->header_size / 2.0f)
    };
    DrawTextPro(menu->font, header, header_position, header_origin, 0.0f, menu->font_size, 0.0f, MENU_FG_COLOR);

    DrawRectangleLinesEx(menu->inner_rectangle, menu->line_thickness, MENU_FG_COLOR);

    { // foreground lines
        Vector2 start = {menu->inner_rectangle.x, menu->inner_rectangle.y};
        Vector2 end = {menu->inner_rectangle.x + menu->inner_rectangle.width, start.y};
        DrawLineEx(start, end, menu->line_thickness, MENU_FG_COLOR);
        start.x = menu->center_x;
        end.x = start.x;
        end.y = start.y + menu->line_section_size;
        DrawLineEx(start, end, menu->line_thickness, MENU_FG_COLOR);
        start.y = end.y;
        start.x = menu->inner_rectangle.x;
        end.x = menu->inner_rectangle.x + menu->inner_rectangle.width;
        DrawLineEx(start, end, menu->line_thickness, MENU_FG_COLOR);
        start.y += menu->instruction_size;
        end.y += menu->instruction_size;
        DrawLineEx(start, end, menu->line_thickness, MENU_FG_COLOR);
    }

    draw_instruction_text(menu);
}

void menu_render(State *state) {
    Menu *menu = &state->menu;

    switch (menu->state) {
    default: {
        draw_default_menu_background(state);
        draw_option_cursor(state);
        for (int option_idx = 0; option_idx < MENU_OPTION_COUNT; option_idx++) {
            draw_option_text(menu, option_idx);
        }
        draw_description(menu);
    } break;
    case MENU_STATE_GAME_OVER: {
        char *text = "You are a horrible person";
        Vector2 text_menu = MeasureTextEx(menu->font, text, menu->font_size, 0.0f);
        Vector2 origin = {0};
        Vector2 position = {
            .x = menu->outer_rectangle.x - (text_menu.x / 2),
            .y = menu->outer_rectangle.y - (text_menu.y / 2)
        };
        DrawTextPro(menu->font, text, position, origin, 0.0f, menu->font_size, 0.0f, WHITE);
    } break;
    }
}

