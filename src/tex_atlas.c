#include "raylib.h"
#include "main.h"

static void add_texture_offsets(State *state, int *x, int *y, Tex tex, int size, int repeat, bool reset) {
    for (int i = 0; i < repeat; i++) {
        state->tex_atlas_offsets[tex + i] = (Tex_Atlas_Offset) {
            .x = *x,
            .y = *y,
            .size = size
        };
        *x += size + ATLAS_PADDING;
    }
    if (reset) {
        *x = ATLAS_PADDING;
        *y += size + ATLAS_PADDING;
    }
}

static void atlas_source_rectangle(State *state, Rectangle *rec, Tex tex) {
    rec->x = state->tex_atlas_offsets[tex].x;
    rec->y = state->tex_atlas_offsets[tex].y;
    rec->width = state->tex_atlas_offsets[tex].size;
    rec->height = state->tex_atlas_offsets[tex].size;
}

static void atlas_destination_rectangle(State *state, Rectangle *rec, Vector2 position, float scale) {
    rec->x = state->game_center_x + (position.x * (state->game_width / 2.0f));
    rec->y = state->game_center_y - (position.y * (state->game_height / 2.0f));
    rec->width = scale;
    rec->height = scale;
}

static float tex_render_scale(State *state, Tex tex, float multiplier) {
    float size = (float)(state->tex_atlas_offsets[tex].size);
    float result = size * state->scale_multiplier * multiplier;
    /*
    if ((uint)(result * GAME_WIDTH_RATIO) < state->game_width) {
        // as long as GAME_WIDTH_RATIO is odd we might need to add a correction pixel
        result += 1.0f;
    }
    */
    return result;
}

void tex_atlas_init(State *state) {
    state->tex_atlas = LoadTexture("./textures/atlas.png");
    int x = ATLAS_PADDING;
    int y = ATLAS_PADDING;
    add_texture_offsets(state, &x, &y, TEX_ENV_NIGHT_SKY, 128, 8, true);
    add_texture_offsets(state, &x, &y, TEX_ENV_WINTER_SKY, 128, 5, true);
    add_texture_offsets(state, &x, &y, TEX_GIANT_BIRD_1, 64, 7, true);
    add_texture_offsets(state, &x, &y, TEX_WHITE_BIRD_1, 32, 21, true);
    add_texture_offsets(state, &x, &y, TEX_BIRD_BLOOD_1, 32, 2, false);
    add_texture_offsets(state, &x, &y, TEX_BIRD_BLOOD_3, 16, 5, false);
    add_texture_offsets(state, &x, &y, TEX_BIRD_BLOOD_3, 32, 0, true);
    add_texture_offsets(state, &x, &y, TEX_PLAYER_1, 32, 3, true);
}

void tex_atlas_cleanup(State *state) {
    UnloadTexture(state->tex_atlas);
}

void tex_atlas_draw(State *state, Tex tex, Vector2 position, float rotation, float scale, Color color) {
    Rectangle source;
    atlas_source_rectangle(state, &source, tex);
    float render_scale = tex_render_scale(state, tex, scale);
    Rectangle dest;
    atlas_destination_rectangle(state, &dest, position, render_scale);
    Vector2 origin = { render_scale / 2.0f, render_scale / 2.0f };
    DrawTexturePro(state->tex_atlas, source, dest, origin, rotation, color);
}

void tex_atlas_draw_raw(State *state, Tex tex, Vector2 position, float rotation, float scale, Vector2 origin, Color color) {
    Rectangle source;
    atlas_source_rectangle(state, &source, tex);
    int render_scale = tex_render_scale(state, tex, scale);
    Rectangle dest = {position.x, position.y, render_scale, render_scale};
    DrawTexturePro(state->tex_atlas, source, dest, origin, rotation, WHITE);
}
