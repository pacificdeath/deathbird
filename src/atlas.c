static void atlas_source_rectangle(State *state, Rectangle *rec, Sprite sprite) {
    ASSERT(sprite < SPRITE_COUNT);
    *rec = sprite_rectangles[sprite];
}

static void atlas_destination_rectangle(State *state, Rectangle *rec, Vector2 position, Vector2 scale) {
    rec->x = state->game_center_x + (position.x * (state->game_width / 2.0f));
    rec->y = state->game_center_y - (position.y * (state->game_height / 2.0f));
    rec->width = scale.x;
    rec->height = scale.y;
}

static Vector2 sprite_render_scale(State *state, Sprite sprite, float multiplier) {
    ASSERT(sprite < SPRITE_COUNT);
    return (Vector2) {
        sprite_rectangles[sprite].width * state->scale_multiplier * multiplier,
        sprite_rectangles[sprite].height * state->scale_multiplier * multiplier,
    };
}

void atlas_init(State *state) {
    state->atlas = LoadTexture(ATLAS_PNG_PATH);
}

void atlas_cleanup(State *state) {
    UnloadTexture(state->atlas);
}

void atlas_draw(State *state, Sprite sprite, Vector2 position, float rotation, float scale, Color color) {
    Rectangle source;
    atlas_source_rectangle(state, &source, sprite);
    Vector2 render_scale = sprite_render_scale(state, sprite, scale);
    Rectangle dest;
    atlas_destination_rectangle(state, &dest, position, render_scale);
    Vector2 origin = { render_scale.x / 2.0f, render_scale.y / 2.0f };
    DrawTexturePro(state->atlas, source, dest, origin, rotation, color);
}

#if DEBUG
void atlas_debug(State *state, bool bird_shader) {
    static Sprite sprite = 0;
    if (IsKeyPressed(KEY_RIGHT)) {
        sprite = (sprite + 1) % SPRITE_COUNT;
    } else if (IsKeyPressed(KEY_LEFT)) {
        sprite = ((sprite + SPRITE_COUNT) - 1) % SPRITE_COUNT;
    } else if (IsKeyPressed(KEY_UP)) {
        sprite = (sprite + 10) % SPRITE_COUNT;
    } else if (IsKeyPressed(KEY_DOWN)) {
        sprite = ((sprite + SPRITE_COUNT) - 10) % SPRITE_COUNT;
    }
    BeginDrawing();
    if (bird_shader) {
        BeginShaderMode(state->bird_shader);
    }
    Vector2 text_position = {state->game_left, state->game_top};
    ClearBackground(BLACK);
    DrawTextEx(state->menu.font, TextFormat("Sprite: %i", sprite), text_position, 20 * state->scale_multiplier, 0.0f, WHITE);
    atlas_draw(state, sprite, (Vector2){0}, 0.0f, 1.0f, WHITE);
    if (bird_shader) {
        EndShaderMode();
    }
    EndDrawing();
}
#endif
