#define MENU_FULL_SIZE 0.8f
#define MENU_BIG_ANIMATION_SPEED 2.0f
#define MENU_SINE_ANIMATION_SPEED 10.0f
#define MENU_SELECT_ANIMATION_SPEED 2.0f

#define MENU_BG_COLOR ((Color){0x20,0x40,0x80,0xf0})
#define MENU_OPTION_COLOR ((Color){0xff,0xff,0xff,0xff})
#define MENU_SELECTED_COLOR ((Color){0x60,0xc0,0x60,0xff})

enum {
    MAIN_MENU_OPTION_JUST_PLAY_THE_GAME,
    MAIN_MENU_OPTION_1,
    MAIN_MENU_OPTION_2,
    MAIN_MENU_OPTION_GO_AWAY_GAME,
    MAIN_MENU_OPTION_COUNT,
};

enum {
    PAUSE_MENU_OPTION_RESUME,
    PAUSE_MENU_OPTION_QUICK_RESTART,
    PAUSE_MENU_OPTION_BACK_TO_MAIN_MENU,
    PAUSE_MENU_OPTION_COUNT,
};

enum {
    GAME_OVER_MENU_OPTION_JUST_RESTART,
    GAME_OVER_MENU_OPTION_ENOUGH_WITH_THIS,
    GAME_OVER_MENU_OPTION_COUNT,
};

bool is_menu_opening() { return state->menu_state == MENU_STATE_OPENING; }
bool is_menu_active() { return state->menu_state == MENU_STATE_ACTIVE; }
bool is_menu_closing() { return state->menu_state == MENU_STATE_CLOSING; }
bool is_menu_inactive() { return state->menu_state == MENU_STATE_INACTIVE; }

float ease_out_back(float x) {
    float c1 = 1.70158;
    float c3 = c1 + 1;
    return 1 + c3 * powf(x - 1, 3) + c1 * powf(x - 1, 2);
}

float ease_out_elastic(float x) {
    float c4 = (2 * PI) / 4;
    if (x <= 0) return 0;
    if (x >= 1) return 1;
    return powf(2, -5 * x) * sinf((x * 10 - 0.75) * c4) + 1;
}

void update_menu_rectangle() {
    state->menu_rectangle.width = GetScreenWidth() * MENU_FULL_SIZE * ease_out_back(state->menu_big_animation);
    state->menu_rectangle.height = GetScreenHeight() * MENU_FULL_SIZE * ease_out_back(state->menu_big_animation);
    state->menu_rectangle.x = (GetScreenWidth() - state->menu_rectangle.width) / 2;
    state->menu_rectangle.y = (GetScreenHeight() - state->menu_rectangle.height) / 2;
}

void initialize_menu() {
    state->menu_cached_global_state = state->global_state;
    state->menu_state = MENU_STATE_OPENING;
    state->menu_selected_idx = 0;
    state->menu_big_animation = 0;
    update_menu_rectangle();
}

bool is_game_paused() {
    return has_flag(state->flags, FLAG_GAME_IS_PAUSED);
}

void toggle_game_pause() {
    if (is_game_paused()) {
        state->menu_state = MENU_STATE_CLOSING;
    } else {
        state->menu_cached_global_state = state->global_state;
        state->menu_state = MENU_STATE_OPENING;
        state->flags |= FLAG_GAME_IS_PAUSED;
    }
}

int get_menu_option_count() {
    if (is_game_paused()) {
        return PAUSE_MENU_OPTION_COUNT;
    }
    switch (state->menu_cached_global_state) {
        case GLOBAL_STATE_DEFAULT:
            return MAIN_MENU_OPTION_COUNT;
        case GLOBAL_STATE_GAME_OVER:
            return GAME_OVER_MENU_OPTION_COUNT;
        default:
            return 0;
    }
}

const char *get_menu_option_name(int idx) {
    if (is_game_paused()) {
        switch (idx) {
            default: ASSERT(false); return NULL;
            case PAUSE_MENU_OPTION_RESUME: return "Resume";
            case PAUSE_MENU_OPTION_QUICK_RESTART: return "Quick restart";
            case PAUSE_MENU_OPTION_BACK_TO_MAIN_MENU: return "Back to main menu";
        }
    }
    switch (state->menu_cached_global_state) {
        default: {
            switch (idx) {
                default: ASSERT(false); return NULL;
                case MAIN_MENU_OPTION_JUST_PLAY_THE_GAME: return "Just play the game!";
                case MAIN_MENU_OPTION_1: return "You are a horrible person";
                case MAIN_MENU_OPTION_2: return "What the hell man?!";
                case MAIN_MENU_OPTION_GO_AWAY_GAME: return "Go away game!";
            }
        }
        case GLOBAL_STATE_GAME_OVER: {
            switch (idx) {
                default: ASSERT(false); return NULL;
                case GAME_OVER_MENU_OPTION_JUST_RESTART: return "Just restart";
                case GAME_OVER_MENU_OPTION_ENOUGH_WITH_THIS: return "Enough with this!";
            }
        }
    }
}

void update_menu_selection_idx(int idx) {
    state->menu_selected_idx = idx;
    state->menu_select_animation = 0;
}

bool update_menu() {
    if (state->global_state != GLOBAL_STATE_DEFAULT && IsKeyPressed(KEY_ESCAPE)) {
        toggle_game_pause();
    }

    if (is_menu_inactive()) {
        return true;
    }

    state->menu_selection_bird_animation += state->delta_time;
    if (state->menu_selection_bird_animation > GAME_ANIM_SPEED) {
        state->menu_selection_bird_animation = 0.0f;
        state->menu_selection_bird_sprite = (state->menu_selection_bird_sprite + 1) % ANIM_SPRITE_AMOUNT;
    }

    if (is_menu_closing()) {
        state->menu_big_animation -= state->delta_time * MENU_BIG_ANIMATION_SPEED;
        if (state->menu_big_animation < 0) {
            state->menu_big_animation = 0;
            state->menu_state = MENU_STATE_INACTIVE;
            state->flags &= ~FLAG_GAME_IS_PAUSED;
            state->global_state = GLOBAL_STATE_GAME;

            update_menu_rectangle();
            return true;
        }

        update_menu_rectangle();
        return false;
    }

    update_menu_rectangle();

    if (!has_flag(state->flags, FLAG_CONSOLE)) {
        if (IsKeyPressed(KEY_UP)) {
            if (state->menu_selected_idx > 0) {
                update_menu_selection_idx(state->menu_selected_idx - 1);
            }
        } else if (IsKeyPressed(KEY_DOWN)) {
            if (state->menu_selected_idx < (get_menu_option_count() - 1)) {
                update_menu_selection_idx(state->menu_selected_idx + 1);
            }
        } else if (IsKeyPressed(KEY_ENTER)) {
            if (is_game_paused()) {
                switch (state->menu_selected_idx) {
                    default: ASSERT(false);
                    case PAUSE_MENU_OPTION_RESUME:
                        toggle_game_pause();
                        break;
                    case PAUSE_MENU_OPTION_QUICK_RESTART:
                        state->flags &= ~FLAG_GAME_IS_PAUSED;
                        travel_to_area(AREA_FOREST, 1);
                        return true;
                    case PAUSE_MENU_OPTION_BACK_TO_MAIN_MENU:
                        travel_to_area(AREA_FOREST, 1);
                        state->global_state = GLOBAL_STATE_DEFAULT;
                        state->flags &= ~FLAG_GAME_IS_PAUSED;
                        initialize_menu();
                        break;
                }
                return false;
            }
            switch (state->menu_cached_global_state) {
                default: {
                    switch (state->menu_selected_idx) {
                        default: ASSERT(false);
                        case MAIN_MENU_OPTION_JUST_PLAY_THE_GAME:
                            state->menu_state = MENU_STATE_CLOSING;
                            break;
                        case MAIN_MENU_OPTION_1:
                            break;
                        case MAIN_MENU_OPTION_2:
                            break;
                        case MAIN_MENU_OPTION_GO_AWAY_GAME:
                            state->flags |= FLAG_DEATHBIRD_BIG_QUIT;
                            return false;
                    }
                } break;
                case GLOBAL_STATE_GAME_OVER: {
                    switch (state->menu_selected_idx) {
                        default: ASSERT(false);
                        case GAME_OVER_MENU_OPTION_JUST_RESTART:
                            return true;
                        case GAME_OVER_MENU_OPTION_ENOUGH_WITH_THIS:
                            state->flags |= FLAG_DEATHBIRD_BIG_QUIT;
                            return false;
                    }
                } break;
            }
        }
    }

    if (is_menu_opening()) {
        state->menu_big_animation += state->delta_time * MENU_BIG_ANIMATION_SPEED;
        if (state->menu_big_animation > 1) {
            state->menu_big_animation = 1;
            state->menu_state = MENU_STATE_ACTIVE;
            state->menu_select_animation = 0;
        }
    }

    state->menu_sine_animation += state->delta_time * MENU_SINE_ANIMATION_SPEED;
    if (state->menu_sine_animation >= PI_X2) {
        state->menu_sine_animation -= PI_X2;
    }

    if (is_menu_active()) {
        state->menu_select_animation += state->delta_time * MENU_SELECT_ANIMATION_SPEED;
        if (state->menu_select_animation > 1.0f) {
            state->menu_select_animation = 1.0f;
        }
    }

    return false;
}

void render_menu() {
    DrawRectangleRounded(state->menu_rectangle, 0.2f, 100, MENU_BG_COLOR);
    DrawRectangleRoundedLines(state->menu_rectangle, 0.2f, 100, MENU_OPTION_COLOR);

    float multiplier = ease_out_back(state->menu_big_animation);

    for (int option_idx = 0; option_idx < get_menu_option_count(); option_idx++) {

        const char *option_name = get_menu_option_name(option_idx);

        bool is_selected = option_idx == state->menu_selected_idx;

        float text_rotation;
        float font_size;
        Color text_color;

        if (is_selected && is_menu_active()) {
            text_rotation = sinf(state->menu_sine_animation) * 5;
            font_size = state->font_size + (state->font_size * ease_out_elastic(state->menu_select_animation));
            text_color = MENU_SELECTED_COLOR;
        } else {
            text_rotation = 0;
            font_size = state->font_size * multiplier;
            text_color = MENU_OPTION_COLOR;
        }

        Vector2 text_dimensions = MeasureTextEx(state->font, option_name, font_size, 0);

        Vector2 text_position;
        text_position.x = state->menu_rectangle.x + (state->menu_rectangle.width / 2);
        text_position.y = state->menu_rectangle.y + (state->menu_rectangle.height / (get_menu_option_count() + 1)) * (option_idx + 1);

        Vector2 text_origin;
        text_origin.x = text_dimensions.x / 2;
        text_origin.y = text_dimensions.y / 2;

        DrawTextPro(state->font, option_name, text_position, text_origin, text_rotation, font_size, 0, text_color);

        if (is_selected) {
            BeginShaderMode(state->bird_shader);

            int bird_sprite = SPRITE_BIRD1 + state->menu_selection_bird_sprite;
            float bird_offset_from_menu_border = state->menu_rectangle.width * 0.1f;
            float bird_offscreen_offset = GetScreenWidth() - (GetScreenWidth() * multiplier);

            Vector2 bird_pixel_position;
            float menu_right_border = state->menu_rectangle.x + state->menu_rectangle.width;
            bird_pixel_position.x = menu_right_border - bird_offset_from_menu_border + bird_offscreen_offset;
            float bird_y_direction = text_position.y - state->menu_selection_bird_position;
            state->menu_selection_bird_position += bird_y_direction * state->delta_time * 10.0f;
            bird_pixel_position.y = state->menu_selection_bird_position;
            Vector2 bird_position = to_game_position(bird_pixel_position);
            Vector2 bird_scale = is_menu_closing() ? SCALE_X_FLIP : SCALE_NORMAL;
            atlas_draw(bird_sprite, bird_position, 0.0f, bird_scale, OPAQUE);

            bird_pixel_position.x = state->menu_rectangle.x + bird_offset_from_menu_border - bird_offscreen_offset;
            bird_position = to_game_position(bird_pixel_position);
            bird_scale = is_menu_closing() ? SCALE_NORMAL : SCALE_X_FLIP;
            atlas_draw(bird_sprite, bird_position, 0.0f, bird_scale, OPAQUE);

            EndShaderMode();
        }
    }
}

