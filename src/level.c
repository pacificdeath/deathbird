#define MIN_BIRD_FREQUENCY 0.1f
#define MAX_BIRD_FREQUENCY 0.2f
#define LEVELS_BEFORE_MIN_FREQUENCY 16
#define LEVEL_IDX_TEXT_BG_COLOR ((Color) { 0, 0, 0, 255 })
#define LEVEL_IDX_TEXT_COLOR ((Color) { 255, 255, 255, 255 })
#define MULTIPLIER_TEXT_BG_COLOR ((Color) { 255, 0, 0, 255 })
#define MULTIPLIER_TEXT_COLOR ((Color) { 255, 255, 255, 255 })

int get_loop_amount() {
    return 1 + (state->level_idx / AREA_TOTAL);
}

bool level_target_reached() {
    return state->portal_fuel >= state->current_level.required_fuel;
}

Vector2 scroll_sprite_size() {
    return (Vector2) {
        .x = 2.0f / GAME_WIDTH_RATIO,
        .y = 2.0f / GAME_HEIGHT_RATIO
    };
}

Vector2 scroll_sprite_offset(Vector2 sprite_size) {
    return (Vector2) {
        .x = -1 + (sprite_size.x / 2),
        .y = -1 + (sprite_size.y / 2)
    };
}

int setup_sprite_scrollers_for_current_level(State *state) {
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

        sprite_scrollers[2].sprite = SPRITE_MEADOW1;
        sprite_scrollers[2].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[2].horizontal_speed = 0.10f;
        sprite_scrollers[2].vertical_sprites = 1;
        sprite_scrollers[2].vertical_speed = 0.0f;
        sprite_scrollers[2].opacity = 255;

        sprite_scrollers[3].sprite = SPRITE_MEADOW2;
        sprite_scrollers[3].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[3].horizontal_speed = 0.15f;
        sprite_scrollers[3].vertical_sprites = 1;
        sprite_scrollers[3].vertical_speed = 0.0f;
        sprite_scrollers[3].opacity = 255;

        sprite_scrollers[4].sprite = SPRITE_MEADOW3;
        sprite_scrollers[4].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[4].horizontal_speed = 0.20f;
        sprite_scrollers[4].vertical_sprites = 1;
        sprite_scrollers[4].vertical_speed = 0.0f;
        sprite_scrollers[4].opacity = 255;

        sprite_scrollers[5].sprite = SPRITE_FENCE;
        sprite_scrollers[5].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[5].horizontal_speed = 0.25f;
        sprite_scrollers[5].vertical_sprites = 1;
        sprite_scrollers[5].vertical_speed = 0.0f;
        sprite_scrollers[5].opacity = 255;

        sprite_scroller_amount = 6;
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
    case AREA_GREEN: {
        sprite_scrollers[0].sprite = SPRITE_GREEN_SKY;
        sprite_scrollers[0].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[0].horizontal_speed = -0.03f;
        sprite_scrollers[0].vertical_sprites = GAME_HEIGHT_RATIO;
        sprite_scrollers[0].vertical_speed = 0.0f;
        sprite_scrollers[0].opacity = 255;

        sprite_scrollers[1].sprite = SPRITE_PURPLE_CLOUDS;
        sprite_scrollers[1].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[1].horizontal_speed = 0.03f;
        sprite_scrollers[1].vertical_sprites = 1;
        sprite_scrollers[1].vertical_speed = 0.0f;
        sprite_scrollers[1].opacity = 255;

        sprite_scrollers[2].sprite = SPRITE_INDUSTRIAL_BUILDINGS;
        sprite_scrollers[2].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[2].horizontal_speed = 0.05f;
        sprite_scrollers[2].vertical_sprites = 1;
        sprite_scrollers[2].vertical_speed = 0.0f;
        sprite_scrollers[2].opacity = 255;

        sprite_scrollers[3].sprite = SPRITE_FIELD1;
        sprite_scrollers[3].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[3].horizontal_speed = 0.10f;
        sprite_scrollers[3].vertical_sprites = 1;
        sprite_scrollers[3].vertical_speed = 0.0f;
        sprite_scrollers[3].opacity = 255;

        sprite_scrollers[4].sprite = SPRITE_FIELD2;
        sprite_scrollers[4].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[4].horizontal_speed = 0.15f;
        sprite_scrollers[4].vertical_sprites = 1;
        sprite_scrollers[4].vertical_speed = 0.0f;
        sprite_scrollers[4].opacity = 255;

        sprite_scrollers[5].sprite = SPRITE_FIELD3;
        sprite_scrollers[5].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[5].horizontal_speed = 0.20f;
        sprite_scrollers[5].vertical_sprites = 1;
        sprite_scrollers[5].vertical_speed = 0.0f;
        sprite_scrollers[5].opacity = 255;

        sprite_scrollers[6].sprite = SPRITE_INDUSTRIAL_FENCE;
        sprite_scrollers[6].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[6].horizontal_speed = 0.25f;
        sprite_scrollers[6].vertical_sprites = 1;
        sprite_scrollers[6].vertical_speed = 0.0f;
        sprite_scrollers[6].opacity = 255;

        sprite_scrollers[7].sprite = SPRITE_RAIN;
        sprite_scrollers[7].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[7].horizontal_speed = 0.25f;
        sprite_scrollers[7].vertical_sprites = GAME_HEIGHT_RATIO + 1;
        sprite_scrollers[7].vertical_speed = -2.0f;
        sprite_scrollers[7].opacity = 96;

        sprite_scroller_amount = 8;
    } break;
    case AREA_VOID: {
        sprite_scroller_amount = 0;
    } break;
    case AREA_INDUSTRIAL: {
        sprite_scrollers[0].sprite = SPRITE_FOG3,
        sprite_scrollers[0].horizontal_sprites = GAME_WIDTH_RATIO + 1,
        sprite_scrollers[0].horizontal_speed = 0.01f,
        sprite_scrollers[0].vertical_sprites = GAME_HEIGHT_RATIO,
        sprite_scrollers[0].vertical_speed = 0.0f;
        sprite_scrollers[0].opacity = 32;

        sprite_scrollers[1].sprite = SPRITE_FOG3,
        sprite_scrollers[1].horizontal_sprites = GAME_WIDTH_RATIO + 1,
        sprite_scrollers[1].horizontal_speed = 0.02f,
        sprite_scrollers[1].vertical_sprites = GAME_HEIGHT_RATIO,
        sprite_scrollers[1].vertical_speed = 0.0f;
        sprite_scrollers[1].opacity = 64;

        sprite_scrollers[2].sprite = SPRITE_FOG3,
        sprite_scrollers[2].horizontal_sprites = GAME_WIDTH_RATIO + 1,
        sprite_scrollers[2].horizontal_speed = 0.03f,
        sprite_scrollers[2].vertical_sprites = GAME_HEIGHT_RATIO,
        sprite_scrollers[2].vertical_speed = 0.0f;
        sprite_scrollers[2].opacity = 128;

        sprite_scrollers[3].sprite = SPRITE_FOG2;
        sprite_scrollers[3].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[3].horizontal_speed = 0.05f;
        sprite_scrollers[3].vertical_sprites = 1;
        sprite_scrollers[3].vertical_speed = 0.0f;
        sprite_scrollers[3].opacity = 255;

        sprite_scrollers[4].sprite = SPRITE_FOG1;
        sprite_scrollers[4].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[4].horizontal_speed = 0.25f;
        sprite_scrollers[4].vertical_sprites = 1;
        sprite_scrollers[4].vertical_speed = 0.0f;
        sprite_scrollers[4].opacity = 255;

        sprite_scroller_amount = 5;
    } break;
    case AREA_CASTLE: {
        sprite_scrollers[0].sprite = SPRITE_CASTLE_SKY;
        sprite_scrollers[0].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[0].horizontal_speed = -0.03f;
        sprite_scrollers[0].vertical_sprites = GAME_HEIGHT_RATIO;
        sprite_scrollers[0].vertical_speed = 0.0f;
        sprite_scrollers[0].opacity = 255;

        sprite_scrollers[1].sprite = SPRITE_DARK_HILLS;
        sprite_scrollers[1].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[1].horizontal_speed = 0.03f;
        sprite_scrollers[1].vertical_sprites = 1;
        sprite_scrollers[1].vertical_speed = 0.0f;
        sprite_scrollers[1].opacity = 255;

        sprite_scrollers[2].sprite = SPRITE_DARK_SILHOUETTES;
        sprite_scrollers[2].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[2].horizontal_speed = 0.15f;
        sprite_scrollers[2].vertical_sprites = 1;
        sprite_scrollers[2].vertical_speed = 0.0f;
        sprite_scrollers[2].opacity = 255;

        sprite_scrollers[3].sprite = SPRITE_DARK_BRICKS;
        sprite_scrollers[3].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[3].horizontal_speed = 0.75f;
        sprite_scrollers[3].vertical_sprites = 1;
        sprite_scrollers[3].vertical_speed = 0.0f;
        sprite_scrollers[3].opacity = 255;

        sprite_scroller_amount = 4;
    } break;
    case AREA_SKY: {
        sprite_scrollers[0].sprite = SPRITE_DARK_SKY;
        sprite_scrollers[0].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[0].horizontal_speed = -0.03f;
        sprite_scrollers[0].vertical_sprites = GAME_HEIGHT_RATIO;
        sprite_scrollers[0].vertical_speed = 0.0f;
        sprite_scrollers[0].opacity = 255;

        sprite_scrollers[1].sprite = SPRITE_SKY_CLOUD8;
        sprite_scrollers[1].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[1].horizontal_speed = 0.0524288f;
        sprite_scrollers[1].vertical_sprites = 1;
        sprite_scrollers[1].vertical_speed = 0.0f;
        sprite_scrollers[1].opacity = 255;

        sprite_scrollers[2].sprite = SPRITE_SKY_CLOUD7;
        sprite_scrollers[2].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[2].horizontal_speed = 0.065536f;
        sprite_scrollers[2].vertical_sprites = 1;
        sprite_scrollers[2].vertical_speed = 0.0f;
        sprite_scrollers[2].opacity = 255;

        sprite_scrollers[3].sprite = SPRITE_SKY_CLOUD6;
        sprite_scrollers[3].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[3].horizontal_speed = 0.08192f;
        sprite_scrollers[3].vertical_sprites = 1;
        sprite_scrollers[3].vertical_speed = 0.0f;
        sprite_scrollers[3].opacity = 255;

        sprite_scrollers[4].sprite = SPRITE_SKY_CLOUD5;
        sprite_scrollers[4].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[4].horizontal_speed = 0.1024f;
        sprite_scrollers[4].vertical_sprites = 1;
        sprite_scrollers[4].vertical_speed = 0.0f;
        sprite_scrollers[4].opacity = 255;

        sprite_scrollers[5].sprite = SPRITE_SKY_CLOUD4;
        sprite_scrollers[5].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[5].horizontal_speed = 0.128f;
        sprite_scrollers[5].vertical_sprites = 1;
        sprite_scrollers[5].vertical_speed = 0.0f;
        sprite_scrollers[5].opacity = 255;

        sprite_scrollers[6].sprite = SPRITE_SKY_CLOUD3;
        sprite_scrollers[6].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[6].horizontal_speed = 0.16f;
        sprite_scrollers[6].vertical_sprites = 1;
        sprite_scrollers[6].vertical_speed = 0.0f;
        sprite_scrollers[6].opacity = 255;

        sprite_scrollers[7].sprite = SPRITE_SKY_CLOUD2;
        sprite_scrollers[7].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[7].horizontal_speed = 0.2f;
        sprite_scrollers[7].vertical_sprites = 1;
        sprite_scrollers[7].vertical_speed = 0.0f;
        sprite_scrollers[7].opacity = 255;

        sprite_scrollers[8].sprite = SPRITE_SKY_CLOUD1;
        sprite_scrollers[8].horizontal_sprites = GAME_WIDTH_RATIO + 1;
        sprite_scrollers[8].horizontal_speed = 0.25f;
        sprite_scrollers[8].vertical_sprites = 1;
        sprite_scrollers[8].vertical_speed = 0.0f;
        sprite_scrollers[8].opacity = 255;

        sprite_scroller_amount = 9;
    } break;
    }
    for (int i = 0; i < sprite_scroller_amount; i++) {
        ASSERT(i < SPRITE_SCROLLER_CAP);
        sprite_scrollers[i].scroll.x = 0.0f;
        sprite_scrollers[i].scroll.y = 0.0f;
    }
    return sprite_scroller_amount;
}

Level generate_level_data(State *state) {
    Level level = {0};

    if (state->level_idx >= LEVELS_BEFORE_MIN_FREQUENCY) {
        level.bird_frequency = MIN_BIRD_FREQUENCY;
    } else {
        float min_max_frequency_diff = MAX_BIRD_FREQUENCY - MIN_BIRD_FREQUENCY;
        float bird_frequency_loss_per_level = min_max_frequency_diff / LEVELS_BEFORE_MIN_FREQUENCY;
        float bird_frequency_loss = bird_frequency_loss_per_level * state->level_idx;
        level.bird_frequency = MAX_BIRD_FREQUENCY - bird_frequency_loss;
        level.required_fuel = 100 + (state->level_idx * 5);
    }

    return level;
}

void level_set(int level_idx) {
    state->level_idx = level_idx;
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
                Vector2 scale = { 1.01f, 1.01f };
                atlas_draw(state, sprite_scrollers[i].sprite, position, 0.0f, scale, color);
            }
        }
    }
}

