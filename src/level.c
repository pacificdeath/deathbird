#define MIN_BIRD_FREQUENCY 0.05f
#define MAX_BIRD_FREQUENCY 0.2f
#define LEVELS_BEFORE_MIN_FREQUENCY 31
#define LEVEL_IDX_TEXT_BG_COLOR ((Color) { 0, 0, 0, 255 })
#define LEVEL_IDX_TEXT_COLOR ((Color) { 255, 255, 255, 255 })
#define MULTIPLIER_TEXT_BG_COLOR ((Color) { 255, 0, 0, 255 })
#define MULTIPLIER_TEXT_COLOR ((Color) { 255, 255, 255, 255 })

static Vector2 scroll_sprite_size() {
    return (Vector2) {
        .x = 2.0f / GAME_WIDTH_RATIO,
        .y = 2.0f / GAME_HEIGHT_RATIO
    };
}

static Vector2 scroll_sprite_offset(Vector2 sprite_size) {
    return (Vector2) {
        .x = -1 + (sprite_size.x / 2),
        .y = -1 + (sprite_size.y / 2)
    };
}

static int setup_sprite_scrollers_for_current_level(State *state) {
    SpriteScroller *sprite_scrollers = state->sprite_scrollers;
    int sprite_scroller_amount = 0;
    switch (state->area) {
    default: break;
    case AREA_FOREST: {
        sprite_scrollers[0].sprite = SPRITE_NIGHT_SKY;
        sprite_scrollers[0].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[0].horizontal_speed = -0.01f;
        sprite_scrollers[0].vertical_sprites = GAME_HEIGHT_RATIO;
        sprite_scrollers[0].vertical_speed = 0.0f;
        sprite_scrollers[0].opacity = 255;

        sprite_scrollers[1].sprite = SPRITE_OCEAN,
        sprite_scrollers[1].horizontal_sprites = GAME_WIDTH_RATIO + 1,
        sprite_scrollers[1].horizontal_speed = 0.01f,
        sprite_scrollers[1].vertical_sprites = 1,
        sprite_scrollers[1].vertical_speed = 0.0f;
        sprite_scrollers[1].opacity = 255;

        sprite_scrollers[2].sprite = SPRITE_HILLS;
        sprite_scrollers[2].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[2].horizontal_speed = 0.05f;
        sprite_scrollers[2].vertical_sprites = 1;
        sprite_scrollers[2].vertical_speed = 0.0f;
        sprite_scrollers[2].opacity = 255;

        sprite_scrollers[3].sprite = SPRITE_TREES;
        sprite_scrollers[3].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[3].horizontal_speed = 0.25f;
        sprite_scrollers[3].vertical_sprites = 1;
        sprite_scrollers[3].vertical_speed = 0.0f;
        sprite_scrollers[3].opacity = 255;

        sprite_scroller_amount = 4;
    } break;
    case AREA_MEADOWS: {
        sprite_scrollers[0].sprite = SPRITE_DAY_SKY;
        sprite_scrollers[0].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[0].horizontal_speed = -0.03f;
        sprite_scrollers[0].vertical_sprites = GAME_HEIGHT_RATIO;
        sprite_scrollers[0].vertical_speed = 0.0f;
        sprite_scrollers[0].opacity = 255;

        sprite_scrollers[1].sprite = SPRITE_CLOUDS;
        sprite_scrollers[1].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[1].horizontal_speed = 0.03f;
        sprite_scrollers[1].vertical_sprites = 1;
        sprite_scrollers[1].vertical_speed = 0.0f;
        sprite_scrollers[1].opacity = 255;

        sprite_scrollers[2].sprite = SPRITE_MEADOWS;
        sprite_scrollers[2].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[2].horizontal_speed = 0.15f;
        sprite_scrollers[2].vertical_sprites = 1;
        sprite_scrollers[2].vertical_speed = 0.0f;
        sprite_scrollers[2].opacity = 255;

        sprite_scrollers[3].sprite = SPRITE_FENCE;
        sprite_scrollers[3].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[3].horizontal_speed = 0.75f;
        sprite_scrollers[3].vertical_sprites = 1;
        sprite_scrollers[3].vertical_speed = 0.0f;
        sprite_scrollers[3].opacity = 255;

        sprite_scroller_amount = 4;
    } break;
    case AREA_MOUNTAINS: {
        sprite_scrollers[0].sprite = SPRITE_WINTER_SKY;
        sprite_scrollers[0].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[0].horizontal_speed = -0.03f;
        sprite_scrollers[0].vertical_sprites = GAME_HEIGHT_RATIO;
        sprite_scrollers[0].vertical_speed = 0.0f;
        sprite_scrollers[0].opacity = 255;

        sprite_scrollers[1].sprite = SPRITE_MOUNTAINS;
        sprite_scrollers[1].horizontal_sprites = 4;
        sprite_scrollers[1].horizontal_speed = 0.03f;
        sprite_scrollers[1].vertical_sprites = 1;
        sprite_scrollers[1].vertical_speed = 0.0f;
        sprite_scrollers[1].opacity = 255;

        sprite_scrollers[2].sprite = SPRITE_WINTER_HILLS;
        sprite_scrollers[2].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[2].horizontal_speed = 0.15f;
        sprite_scrollers[2].vertical_sprites = 1;
        sprite_scrollers[2].vertical_speed = 0.0f;
        sprite_scrollers[2].opacity = 255;

        sprite_scrollers[3].sprite = SPRITE_WINTER_TREES;
        sprite_scrollers[3].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[3].horizontal_speed = 0.75f;
        sprite_scrollers[3].vertical_sprites = 1;
        sprite_scrollers[3].vertical_speed = 0.0f;
        sprite_scrollers[3].opacity = 255;

        sprite_scrollers[4].sprite = SPRITE_SNOW;
        sprite_scrollers[4].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[4].horizontal_speed = -0.45f;
        sprite_scrollers[4].vertical_sprites = GAME_HEIGHT_RATIO + 1;
        sprite_scrollers[4].vertical_speed = -0.75f;
        sprite_scrollers[4].opacity = 192;

        sprite_scroller_amount = 5;
    } break;
    case AREA_INDUSTRIAL: {
        sprite_scrollers[0].sprite = SPRITE_GREEN_SKY;
        sprite_scrollers[0].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[0].horizontal_speed = -0.03f;
        sprite_scrollers[0].vertical_sprites = GAME_HEIGHT_RATIO;
        sprite_scrollers[0].vertical_speed = 0.0f;
        sprite_scrollers[0].opacity = 255;

        sprite_scrollers[1].sprite = SPRITE_GREY_CLOUDS;
        sprite_scrollers[1].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[1].horizontal_speed = 0.03f;
        sprite_scrollers[1].vertical_sprites = 1;
        sprite_scrollers[1].vertical_speed = 0.0f;
        sprite_scrollers[1].opacity = 255;

        sprite_scrollers[2].sprite = SPRITE_INDUSTRIAL_FIELD;
        sprite_scrollers[2].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[2].horizontal_speed = 0.15f;
        sprite_scrollers[2].vertical_sprites = 1;
        sprite_scrollers[2].vertical_speed = 0.0f;
        sprite_scrollers[2].opacity = 255;

        sprite_scrollers[3].sprite = SPRITE_INDUSTRIAL_FENCE;
        sprite_scrollers[3].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[3].horizontal_speed = 0.75f;
        sprite_scrollers[3].vertical_sprites = 1;
        sprite_scrollers[3].vertical_speed = 0.0f;
        sprite_scrollers[3].opacity = 255;

        sprite_scrollers[4].sprite = SPRITE_RAIN;
        sprite_scrollers[4].horizontal_sprites = GAME_WIDTH_RATIO;
        sprite_scrollers[4].horizontal_speed = 0.0f;
        sprite_scrollers[4].vertical_sprites = GAME_HEIGHT_RATIO + 1;
        sprite_scrollers[4].vertical_speed = -2.0f;
        sprite_scrollers[4].opacity = 64;

        sprite_scroller_amount = 5;
    } break;
    case AREA_CASTLE: {
        sprite_scrollers[0].sprite = SPRITE_CASTLE_SKY;
        sprite_scrollers[0].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[0].horizontal_speed = -0.03f;
        sprite_scrollers[0].vertical_sprites = GAME_HEIGHT_RATIO;
        sprite_scrollers[0].vertical_speed = 0.0f;
        sprite_scrollers[0].opacity = 255;

        sprite_scrollers[1].sprite = SPRITE_CASTLE_MOUNTAINS;
        sprite_scrollers[1].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[1].horizontal_speed = 0.03f;
        sprite_scrollers[1].vertical_sprites = 1;
        sprite_scrollers[1].vertical_speed = 0.0f;
        sprite_scrollers[1].opacity = 255;

        sprite_scrollers[2].sprite = SPRITE_CASTLE_SILHOUETTES;
        sprite_scrollers[2].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[2].horizontal_speed = 0.15f;
        sprite_scrollers[2].vertical_sprites = 1;
        sprite_scrollers[2].vertical_speed = 0.0f;
        sprite_scrollers[2].opacity = 255;

        sprite_scrollers[3].sprite = SPRITE_CASTLE_BRICKS;
        sprite_scrollers[3].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[3].horizontal_speed = 0.75f;
        sprite_scrollers[3].vertical_sprites = 1;
        sprite_scrollers[3].vertical_speed = 0.0f;
        sprite_scrollers[3].opacity = 255;

        sprite_scroller_amount = 4;
    } break;
    }
    for (int i = 0; i < sprite_scroller_amount; i++) {
        ASSERT(i < SPRITE_SCROLLER_CAP);
        sprite_scrollers[i].scroll.x = 0.0f;
        sprite_scrollers[i].scroll.y = 0.0f;
    }
    return sprite_scroller_amount;
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
        level.required_fuel = 20 + (state->level_idx * 5);
    }

    return level;
}

void level_load_0(State *state) {
    state->level_idx = 0;
    state->current_level = generate_level_data(state);
    state->current_level.sprite_scroller_amount = setup_sprite_scrollers_for_current_level(state);
}

void level_load_next(State *state) {
    state->level_idx++;
    state->current_level = generate_level_data(state);
    state->current_level.sprite_scroller_amount = setup_sprite_scrollers_for_current_level(state);
}

void level_update(State *state) {
    SpriteScroller *sprite_scrollers = state->sprite_scrollers;
    Vector2 sprite_size = scroll_sprite_size();
    for (int i = 0; i < state->current_level.sprite_scroller_amount; i++) {
        sprite_scrollers[i].scroll.x += (sprite_scrollers[i].horizontal_speed * state->delta_time);
        if (sprite_scrollers[i].horizontal_speed > 0.0f && sprite_scrollers[i].scroll.x > 0.0f) {
            sprite_scrollers[i].scroll.x -= sprite_size.x;
        } else if (sprite_scrollers[i].horizontal_speed < 0.0f && sprite_scrollers[i].scroll.x < -sprite_size.x) {
            sprite_scrollers[i].scroll.x += sprite_size.x;
        }
        sprite_scrollers[i].scroll.y += (sprite_scrollers[i].vertical_speed * state->delta_time);
        if (sprite_scrollers[i].vertical_speed > 0.0f && sprite_scrollers[i].scroll.y > 0.0f) {
            sprite_scrollers[i].scroll.y -= sprite_size.y;
        } else if (sprite_scrollers[i].vertical_speed < 0.0f && sprite_scrollers[i].scroll.y < -sprite_size.y) {
            sprite_scrollers[i].scroll.y += sprite_size.y;
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

Vector2 text_box(State *state, const char *buffer, Vector2 position, Color bg_color, Color fg_color) {
    int font_size = calc_font_size(state);
    Vector2 text_dimensions = MeasureTextEx(state->menu.font, buffer, font_size, 0);
    DrawRectangleV(position, text_dimensions, bg_color);
    DrawTextPro(state->menu.font, buffer, position, (Vector2) { 0, 0 }, 0, font_size, 0, fg_color);
    return text_dimensions;
}

void level_render(State *state) {
    SpriteScroller *sprite_scrollers = state->sprite_scrollers;
    Vector2 sprite_size = scroll_sprite_size();
    Vector2 sprite_offset = scroll_sprite_offset(sprite_size);
    for (int i = 0; i < state->current_level.sprite_scroller_amount; i++) {
        for (int x = 0; x < sprite_scrollers[i].horizontal_sprites; x++) {
            for (int y = 0; y < sprite_scrollers[i].vertical_sprites; y++) {
                Vector2 position = {
                    sprite_offset.x + sprite_scrollers[i].scroll.x + (sprite_size.x * x),
                    sprite_offset.y + sprite_scrollers[i].scroll.y + (sprite_size.y * y)
                };
                Color color = {255,255,255,sprite_scrollers[i].opacity};
                atlas_draw(state, sprite_scrollers[i].sprite, position, 0.0f, 1.01f, color);
            }
        }
    }
    Vector2 position = { .x = state->game_left, .y = state->game_top };
    Vector2 text_dimensions = text_box(state, TextFormat(" Level: %i ", state->level_idx), position, LEVEL_IDX_TEXT_BG_COLOR, LEVEL_IDX_TEXT_COLOR);

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

    const char *string = TextFormat("%i / %i", state->portal_fuel, state->current_level.required_fuel);
    text_dimensions = MeasureTextEx(state->menu.font, string, font_size, 0);
    DrawTextPro(
        state->menu.font,
        string,
        (Vector2){
            fuel_bar_rec.x + (fuel_bar_rec.width/2) - (text_dimensions.x/2),
            fuel_bar_rec.y + (fuel_bar_rec.height/2) - (text_dimensions.y/2)
        },
        (Vector2){0},
        0,
        font_size,
        0,
        MULTIPLIER_TEXT_COLOR
    );

    if (state->bird_multiplier_timer > 0.0f) {
        string = TextFormat("%ix-Multiplier ", state->bird_multiplier_display);
        Vector2 multiplier_text_dimensions = MeasureTextEx(state->menu.font, string, font_size, 0);
        Vector2 multiplier_text_position = {
            .x = state->game_right - multiplier_text_dimensions.x,
            .y = state->game_top
        };
        DrawRectangleV(multiplier_text_position, multiplier_text_dimensions, MULTIPLIER_TEXT_BG_COLOR);
        DrawTextPro(state->menu.font, string, multiplier_text_position, (Vector2){0}, 0, font_size, 0, MULTIPLIER_TEXT_COLOR);
    }
}

