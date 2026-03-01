void atlas_destination_rectangle(Rectangle *rec, Vector2 position, Vector2 scale) {
    rec->x = (float)state->game_center_x + (position.x * ((float)state->game_width / 2.0f));
    rec->y = (float)state->game_center_y - (position.y * ((float)state->game_height / 2.0f));
    rec->width = scale.x;
    rec->height = scale.y;
}

Vector2 sprite_render_scale(int sprite, Vector2 scale) {
    ASSERT(sprite < SPRITE_COUNT);
    return (Vector2) {
        sprite_rectangles[sprite].width * state->scale_multiplier * scale.x,
        sprite_rectangles[sprite].height * state->scale_multiplier * scale.y,
    };
}

void atlas_init() {
    state->atlas = LoadTexture(ATLAS_PNG_PATH);
    SetTextureWrap(state->atlas, TEXTURE_WRAP_CLAMP);
}

void atlas_cleanup() {
    UnloadTexture(state->atlas);
}

void atlas_draw(int sprite, Vector2 position, float rotation, Vector2 scale, Color color) {
    ASSERT(sprite < SPRITE_COUNT);

    Rectangle source = sprite_rectangles[sprite];

    if (scale.x < 0) {
        source.width = -source.width;
        scale.x = -scale.x;
    }
    if (scale.y < 0) {
        source.height = -source.height;
        scale.y = -scale.y;
    }

    Vector2 render_scale = sprite_render_scale(sprite, scale);
    Rectangle dest;
    atlas_destination_rectangle(&dest, position, render_scale);

    Vector2 origin = { render_scale.x / 2.0f, render_scale.y / 2.0f };

    DrawTexturePro(state->atlas, source, dest, origin, rotation, color);
}

