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

void setup_area(int area) {
    state->area = area;
    state->next_area = area;
    switch (area) {
        default: break;
        case AREA_FOREST:
            state->scrollers[0].sprite = SPRITE_NIGHT_SKY;
            state->scrollers[0].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[0].horizontal_speed = -0.01f;
            state->scrollers[0].vertical_sprites = (int)GAME_HEIGHT_RATIO;
            state->scrollers[0].vertical_speed = 0.0f;
            state->scrollers[0].opacity = 255;

            state->scrollers[1].sprite = SPRITE_OCEAN,
            state->scrollers[1].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1,
            state->scrollers[1].horizontal_speed = 0.01f,
            state->scrollers[1].vertical_sprites = 1,
            state->scrollers[1].vertical_speed = 0.0f;
            state->scrollers[1].opacity = 255;

            state->scrollers[2].sprite = SPRITE_HILLS;
            state->scrollers[2].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[2].horizontal_speed = 0.05f;
            state->scrollers[2].vertical_sprites = 1;
            state->scrollers[2].vertical_speed = 0.0f;
            state->scrollers[2].opacity = 255;

            state->scrollers[3].sprite = SPRITE_TREES;
            state->scrollers[3].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[3].horizontal_speed = 0.25f;
            state->scrollers[3].vertical_sprites = 1;
            state->scrollers[3].vertical_speed = 0.0f;
            state->scrollers[3].opacity = 255;

            state->scroller_count = 4;
            break;
        case AREA_MEADOWS:
            state->scrollers[0].sprite = SPRITE_DAY_SKY;
            state->scrollers[0].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[0].horizontal_speed = -0.03f;
            state->scrollers[0].vertical_sprites = (int)GAME_HEIGHT_RATIO;
            state->scrollers[0].vertical_speed = 0.0f;
            state->scrollers[0].opacity = 255;

            state->scrollers[1].sprite = SPRITE_CLOUDS;
            state->scrollers[1].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[1].horizontal_speed = 0.03f;
            state->scrollers[1].vertical_sprites = 1;
            state->scrollers[1].vertical_speed = 0.0f;
            state->scrollers[1].opacity = 255;

            state->scrollers[2].sprite = SPRITE_MEADOW1;
            state->scrollers[2].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[2].horizontal_speed = 0.10f;
            state->scrollers[2].vertical_sprites = 1;
            state->scrollers[2].vertical_speed = 0.0f;
            state->scrollers[2].opacity = 255;

            state->scrollers[3].sprite = SPRITE_MEADOW2;
            state->scrollers[3].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[3].horizontal_speed = 0.15f;
            state->scrollers[3].vertical_sprites = 1;
            state->scrollers[3].vertical_speed = 0.0f;
            state->scrollers[3].opacity = 255;

            state->scrollers[4].sprite = SPRITE_MEADOW3;
            state->scrollers[4].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[4].horizontal_speed = 0.20f;
            state->scrollers[4].vertical_sprites = 1;
            state->scrollers[4].vertical_speed = 0.0f;
            state->scrollers[4].opacity = 255;

            state->scrollers[5].sprite = SPRITE_FENCE;
            state->scrollers[5].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[5].horizontal_speed = 0.25f;
            state->scrollers[5].vertical_sprites = 1;
            state->scrollers[5].vertical_speed = 0.0f;
            state->scrollers[5].opacity = 255;

            state->scroller_count = 6;
            break;
        case AREA_MOUNTAINS:
            state->scrollers[0].sprite = SPRITE_WINTER_SKY;
            state->scrollers[0].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[0].horizontal_speed = -0.03f;
            state->scrollers[0].vertical_sprites = (int)GAME_HEIGHT_RATIO;
            state->scrollers[0].vertical_speed = 0.0f;
            state->scrollers[0].opacity = 255;

            state->scrollers[1].sprite = SPRITE_MOUNTAINS;
            state->scrollers[1].horizontal_sprites = 4;
            state->scrollers[1].horizontal_speed = 0.03f;
            state->scrollers[1].vertical_sprites = 1;
            state->scrollers[1].vertical_speed = 0.0f;
            state->scrollers[1].opacity = 255;

            state->scrollers[2].sprite = SPRITE_WINTER_HILLS;
            state->scrollers[2].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[2].horizontal_speed = 0.15f;
            state->scrollers[2].vertical_sprites = 1;
            state->scrollers[2].vertical_speed = 0.0f;
            state->scrollers[2].opacity = 255;

            state->scrollers[3].sprite = SPRITE_WINTER_TREES;
            state->scrollers[3].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[3].horizontal_speed = 0.75f;
            state->scrollers[3].vertical_sprites = 1;
            state->scrollers[3].vertical_speed = 0.0f;
            state->scrollers[3].opacity = 255;

            state->scrollers[4].sprite = SPRITE_SNOW;
            state->scrollers[4].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[4].horizontal_speed = -0.45f;
            state->scrollers[4].vertical_sprites = (int)GAME_HEIGHT_RATIO + 1;
            state->scrollers[4].vertical_speed = -0.75f;
            state->scrollers[4].opacity = 192;

            state->scroller_count = 5;
            break;
        case AREA_GREEN:
            state->scrollers[0].sprite = SPRITE_GREEN_SKY;
            state->scrollers[0].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[0].horizontal_speed = -0.03f;
            state->scrollers[0].vertical_sprites = (int)GAME_HEIGHT_RATIO;
            state->scrollers[0].vertical_speed = 0.0f;
            state->scrollers[0].opacity = 255;

            state->scrollers[1].sprite = SPRITE_PURPLE_CLOUDS;
            state->scrollers[1].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[1].horizontal_speed = 0.03f;
            state->scrollers[1].vertical_sprites = 1;
            state->scrollers[1].vertical_speed = 0.0f;
            state->scrollers[1].opacity = 255;

            state->scrollers[2].sprite = SPRITE_INDUSTRIAL_BUILDINGS;
            state->scrollers[2].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[2].horizontal_speed = 0.05f;
            state->scrollers[2].vertical_sprites = 1;
            state->scrollers[2].vertical_speed = 0.0f;
            state->scrollers[2].opacity = 255;

            state->scrollers[3].sprite = SPRITE_FIELD1;
            state->scrollers[3].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[3].horizontal_speed = 0.10f;
            state->scrollers[3].vertical_sprites = 1;
            state->scrollers[3].vertical_speed = 0.0f;
            state->scrollers[3].opacity = 255;

            state->scrollers[4].sprite = SPRITE_FIELD2;
            state->scrollers[4].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[4].horizontal_speed = 0.15f;
            state->scrollers[4].vertical_sprites = 1;
            state->scrollers[4].vertical_speed = 0.0f;
            state->scrollers[4].opacity = 255;

            state->scrollers[5].sprite = SPRITE_FIELD3;
            state->scrollers[5].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[5].horizontal_speed = 0.20f;
            state->scrollers[5].vertical_sprites = 1;
            state->scrollers[5].vertical_speed = 0.0f;
            state->scrollers[5].opacity = 255;

            state->scrollers[6].sprite = SPRITE_INDUSTRIAL_FENCE;
            state->scrollers[6].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[6].horizontal_speed = 0.25f;
            state->scrollers[6].vertical_sprites = 1;
            state->scrollers[6].vertical_speed = 0.0f;
            state->scrollers[6].opacity = 255;

            state->scrollers[7].sprite = SPRITE_RAIN;
            state->scrollers[7].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[7].horizontal_speed = 0.25f;
            state->scrollers[7].vertical_sprites = (int)GAME_HEIGHT_RATIO + 1;
            state->scrollers[7].vertical_speed = -2.0f;
            state->scrollers[7].opacity = 96;

            state->scroller_count = 8;
            break;
        case AREA_VOID:
            state->scroller_count = 0;
            break;
        case AREA_INDUSTRIAL:
            state->scrollers[0].sprite = SPRITE_FOG3,
            state->scrollers[0].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1,
            state->scrollers[0].horizontal_speed = 0.01f,
            state->scrollers[0].vertical_sprites = (int)GAME_HEIGHT_RATIO,
            state->scrollers[0].vertical_speed = 0.0f;
            state->scrollers[0].opacity = 32;

            state->scrollers[1].sprite = SPRITE_FOG3,
            state->scrollers[1].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1,
            state->scrollers[1].horizontal_speed = 0.02f,
            state->scrollers[1].vertical_sprites = (int)GAME_HEIGHT_RATIO,
            state->scrollers[1].vertical_speed = 0.0f;
            state->scrollers[1].opacity = 64;

            state->scrollers[2].sprite = SPRITE_FOG3,
            state->scrollers[2].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1,
            state->scrollers[2].horizontal_speed = 0.03f,
            state->scrollers[2].vertical_sprites = (int)GAME_HEIGHT_RATIO,
            state->scrollers[2].vertical_speed = 0.0f;
            state->scrollers[2].opacity = 128;

            state->scrollers[3].sprite = SPRITE_FOG2;
            state->scrollers[3].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[3].horizontal_speed = 0.05f;
            state->scrollers[3].vertical_sprites = 1;
            state->scrollers[3].vertical_speed = 0.0f;
            state->scrollers[3].opacity = 255;

            state->scrollers[4].sprite = SPRITE_FOG1;
            state->scrollers[4].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[4].horizontal_speed = 0.25f;
            state->scrollers[4].vertical_sprites = 1;
            state->scrollers[4].vertical_speed = 0.0f;
            state->scrollers[4].opacity = 255;

            state->scroller_count = 5;
            break;
        case AREA_CASTLE:
            state->scrollers[0].sprite = SPRITE_CASTLE_SKY;
            state->scrollers[0].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[0].horizontal_speed = -0.03f;
            state->scrollers[0].vertical_sprites = (int)GAME_HEIGHT_RATIO;
            state->scrollers[0].vertical_speed = 0.0f;
            state->scrollers[0].opacity = 255;

            state->scrollers[1].sprite = SPRITE_DARK_HILLS;
            state->scrollers[1].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[1].horizontal_speed = 0.03f;
            state->scrollers[1].vertical_sprites = 1;
            state->scrollers[1].vertical_speed = 0.0f;
            state->scrollers[1].opacity = 255;

            state->scrollers[2].sprite = SPRITE_DARK_SILHOUETTES;
            state->scrollers[2].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[2].horizontal_speed = 0.15f;
            state->scrollers[2].vertical_sprites = 1;
            state->scrollers[2].vertical_speed = 0.0f;
            state->scrollers[2].opacity = 255;

            state->scrollers[3].sprite = SPRITE_DARK_BRICKS;
            state->scrollers[3].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[3].horizontal_speed = 0.75f;
            state->scrollers[3].vertical_sprites = 1;
            state->scrollers[3].vertical_speed = 0.0f;
            state->scrollers[3].opacity = 255;

            state->scroller_count = 4;
            break;
        case AREA_SKY:
            state->scrollers[0].sprite = SPRITE_DARK_SKY;
            state->scrollers[0].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[0].horizontal_speed = -0.03f;
            state->scrollers[0].vertical_sprites = (int)GAME_HEIGHT_RATIO;
            state->scrollers[0].vertical_speed = 0.0f;
            state->scrollers[0].opacity = 255;

            state->scrollers[1].sprite = SPRITE_SKY_CLOUD8;
            state->scrollers[1].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[1].horizontal_speed = 0.0524288f;
            state->scrollers[1].vertical_sprites = 1;
            state->scrollers[1].vertical_speed = 0.0f;
            state->scrollers[1].opacity = 255;

            state->scrollers[2].sprite = SPRITE_SKY_CLOUD7;
            state->scrollers[2].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[2].horizontal_speed = 0.065536f;
            state->scrollers[2].vertical_sprites = 1;
            state->scrollers[2].vertical_speed = 0.0f;
            state->scrollers[2].opacity = 255;

            state->scrollers[3].sprite = SPRITE_SKY_CLOUD6;
            state->scrollers[3].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[3].horizontal_speed = 0.08192f;
            state->scrollers[3].vertical_sprites = 1;
            state->scrollers[3].vertical_speed = 0.0f;
            state->scrollers[3].opacity = 255;

            state->scrollers[4].sprite = SPRITE_SKY_CLOUD5;
            state->scrollers[4].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[4].horizontal_speed = 0.1024f;
            state->scrollers[4].vertical_sprites = 1;
            state->scrollers[4].vertical_speed = 0.0f;
            state->scrollers[4].opacity = 255;

            state->scrollers[5].sprite = SPRITE_SKY_CLOUD4;
            state->scrollers[5].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[5].horizontal_speed = 0.128f;
            state->scrollers[5].vertical_sprites = 1;
            state->scrollers[5].vertical_speed = 0.0f;
            state->scrollers[5].opacity = 255;

            state->scrollers[6].sprite = SPRITE_SKY_CLOUD3;
            state->scrollers[6].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[6].horizontal_speed = 0.16f;
            state->scrollers[6].vertical_sprites = 1;
            state->scrollers[6].vertical_speed = 0.0f;
            state->scrollers[6].opacity = 255;

            state->scrollers[7].sprite = SPRITE_SKY_CLOUD2;
            state->scrollers[7].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[7].horizontal_speed = 0.2f;
            state->scrollers[7].vertical_sprites = 1;
            state->scrollers[7].vertical_speed = 0.0f;
            state->scrollers[7].opacity = 255;

            state->scrollers[8].sprite = SPRITE_SKY_CLOUD1;
            state->scrollers[8].horizontal_sprites = (int)GAME_WIDTH_RATIO + 1;
            state->scrollers[8].horizontal_speed = 0.25f;
            state->scrollers[8].vertical_sprites = 1;
            state->scrollers[8].vertical_speed = 0.0f;
            state->scrollers[8].opacity = 255;

            state->scroller_count = 9;
            break;
    }

    for (int i = 0; i < state->scroller_count; i++) {
        ASSERT(i < SPRITE_SCROLLER_CAP);
        state->scrollers[i].scroll.x = 0.0f;
        state->scrollers[i].scroll.y = 0.0f;
    }
}

void world_init() {
    setup_area(AREA_FOREST);
    state->fader_level = 1;
    state->world_transition_animation = WORLD_TRANSITION_TIME;
}

void world_update() {
    Vector2 sprite_size = scroll_sprite_size();
    for (int i = 0; i < state->scroller_count; i++) {
        state->scrollers[i].scroll.x += (state->scrollers[i].horizontal_speed * state->delta_time);
        if (state->scrollers[i].horizontal_speed > 0.0f && state->scrollers[i].scroll.x > 0.0f) {
            state->scrollers[i].scroll.x -= sprite_size.x;
        } else if (state->scrollers[i].horizontal_speed < 0.0f && state->scrollers[i].scroll.x < -sprite_size.x) {
            state->scrollers[i].scroll.x += sprite_size.x;
        }
        state->scrollers[i].scroll.y += (state->scrollers[i].vertical_speed * state->delta_time);
        if (state->scrollers[i].vertical_speed > 0.0f && state->scrollers[i].scroll.y > 0.0f) {
            state->scrollers[i].scroll.y -= sprite_size.y;
        } else if (state->scrollers[i].vertical_speed < 0.0f && state->scrollers[i].scroll.y < -sprite_size.y) {
            state->scrollers[i].scroll.y += sprite_size.y;
        }
    }

    if (state->area != state->next_area) {
        state->world_transition_animation += GetFrameTime();
        if (state->world_transition_animation > WORLD_TRANSITION_TIME) {
            state->world_transition_animation = WORLD_TRANSITION_TIME;
            state->fader_level = 1;
            setup_area(state->next_area);
        } else {
            state->fader_level = state->world_transition_animation / WORLD_TRANSITION_TIME;
        }
    } else if (state->fader_level > 0) {
        state->world_transition_animation -= GetFrameTime();
        if (state->world_transition_animation < 0) {
            state->world_transition_animation = 0;
            state->fader_level = 0;
        } else {
            state->fader_level = state->world_transition_animation / WORLD_TRANSITION_TIME;
        }
    }
}

void world_render() {
    Vector2 sprite_size = scroll_sprite_size();
    Vector2 sprite_offset = scroll_sprite_offset(sprite_size);
    for (int i = 0; i < state->scroller_count; i++) {
        for (int x = 0; x < state->scrollers[i].horizontal_sprites; x++) {
            for (int y = 0; y < state->scrollers[i].vertical_sprites; y++) {
                Vector2 position = {
                    sprite_offset.x + state->scrollers[i].scroll.x + (sprite_size.x * (float)x),
                    sprite_offset.y + state->scrollers[i].scroll.y + (sprite_size.y * (float)y)
                };
                Color color = { 255, 255, 255, state->scrollers[i].opacity };
                Vector2 scale = { 1.01f, 1.01f };
                atlas_draw(state->scrollers[i].sprite, position, 0.0f, scale, color);
            }
        }
    }

    if (state->fader_level > 0.0f) {
        Color color = { 0, 0, 0, (unsigned char)(state->fader_level * 255) };
        DrawRectangle(state->game_left, state->game_top, state->game_width, state->game_height, color);
    }
}

