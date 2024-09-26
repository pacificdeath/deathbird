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

void tex_atlas_init(State *state) {
    state->tex_atlas = LoadTexture("./textures/atlas2.png");
    int x = ATLAS_PADDING;
    int y = ATLAS_PADDING;
    add_texture_offsets(state, &x, &y, TEX_ENV_NIGHT_SKY, 128, 8, true);
    add_texture_offsets(state, &x, &y, TEX_ENV_WINTER_SKY, 128, 5, true);
    add_texture_offsets(state, &x, &y, TEX_GIANT_BIRD_1, 64, 7, true);
    add_texture_offsets(state, &x, &y, TEX_WHITE_BIRD_1, 32, 21, true);
    add_texture_offsets(state, &x, &y, TEX_BIRD_BLOOD_1, 32, 2, false);
    add_texture_offsets(state, &x, &y, TEX_BIRD_BLOOD_3, 16, 5, false);
    add_texture_offsets(state, &x, &y, TEX_BIRD_BLOOD_3, 32, 0, true);
    add_texture_offsets(state, &x, &y, TEX_PLAYER_1, 32, 1, true);
}

void tex_atlas_cleanup(State *state) {
    UnloadTexture(state->tex_atlas);
}

void tex_atlas_draw(State *state, Tex tex, Vector2 position, float rotation, unsigned char opacity) {
    Rectangle source = {
        .x = state->tex_atlas_offsets[tex].x,
        .y = state->tex_atlas_offsets[tex].y,
        .width = state->tex_atlas_offsets[tex].size,
        .height = state->tex_atlas_offsets[tex].size,
    };
    int render_scale = state->tex_atlas_offsets[tex].size * state->scale_multiplier;
    Rectangle dest = {
        .x = state->game_center_x + (position.x * (state->game_width / 2)),
        .y = state->game_center_y - (position.y * (state->game_height / 2)),
        .width = render_scale,
        .height = render_scale,
    };
    Vector2 origin = { render_scale / 2.0f, render_scale / 2.0f };
    DrawTexturePro(state->tex_atlas, source, dest, origin, rotation, (Color){ 255, 255, 255, opacity});
}

void tex_atlas_draw_raw(State *state, Tex tex, Vector2 position, float rotation) {
    Rectangle source = {
        .x = state->tex_atlas_offsets[tex].x,
        .y = state->tex_atlas_offsets[tex].y,
        .width = state->tex_atlas_offsets[tex].size,
        .height = state->tex_atlas_offsets[tex].size,
    };
    int render_scale = state->tex_atlas_offsets[tex].size * state->scale_multiplier;
    Rectangle dest = {
        .x = position.x,
        .y = position.y,
        .width = render_scale,
        .height = render_scale,
    };
    Vector2 origin = { render_scale / 2.0f, render_scale / 2.0f };
    DrawTexturePro(state->tex_atlas, source, dest, origin, rotation, WHITE);
}
