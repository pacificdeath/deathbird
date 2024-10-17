#include "raylib.h"
#include "main.h"
#ifdef DEBUG
#include <stdio.h>
#endif

typedef struct Texture_Offset_Data {
    int x;
    int y;
    Tex start_texture;
    int texture_size;
    int consecutive_textures;
    int reset_x_and_offset_y;
} Texture_Offset_Data;

static void add_texture_offsets(State *state, Texture_Offset_Data *data) {
    #ifdef DEBUG
    printf(
        "Deathbird Atlas: Tex (%3i -> %3i) Size %3i Offset (x %3i y %3i)\n",
        data->start_texture,
        data->start_texture + data->consecutive_textures,
        data->texture_size,
        data->x,
        data->y
    );
    #endif
    for (int i = 0; i < data->consecutive_textures; i++) {
        state->tex_atlas_offsets[data->start_texture + i] = (Tex_Atlas_Offset) {
            .x = data->x,
            .y = data->y,
            .size = data->texture_size
        };
        data->x += data->texture_size + ATLAS_PADDING;
    }
    if (data->reset_x_and_offset_y) {
        data->x = ATLAS_PADDING;
        data->y += data->reset_x_and_offset_y + ATLAS_PADDING;
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
    return result;
}

void tex_atlas_init(State *state) {
    state->tex_atlas = LoadTexture("./textures/atlas.png");

    Texture_Offset_Data data;
    data.x = ATLAS_PADDING;
    data.y = ATLAS_PADDING;

    data.start_texture = TEX_ENV_NIGHT_SKY;
    data.texture_size = 128;
    data.consecutive_textures = 9;
    data.reset_x_and_offset_y = 128;
    add_texture_offsets(state, &data);

    data.start_texture = TEX_ENV_WINTER_SKY;
    data.texture_size = 128;
    data.consecutive_textures = 6;
    data.reset_x_and_offset_y = 128;
    add_texture_offsets(state, &data);

    data.start_texture = TEX_GIANT_BIRD_1;
    data.texture_size = 64;
    data.consecutive_textures = 7;
    data.reset_x_and_offset_y = 64;
    add_texture_offsets(state, &data);

    data.start_texture = TEX_WHITE_BIRD_1;
    data.texture_size = 32;
    data.consecutive_textures = 28;
    data.reset_x_and_offset_y = 32;
    add_texture_offsets(state, &data);

    data.start_texture = TEX_BIRD_BLOOD_1;
    data.texture_size = 32;
    data.consecutive_textures = 2;
    data.reset_x_and_offset_y = 0;
    add_texture_offsets(state, &data);

    data.start_texture = TEX_BIRD_BLOOD_3;
    data.texture_size = 16;
    data.consecutive_textures = 5;
    data.reset_x_and_offset_y = 0;
    add_texture_offsets(state, &data);

    data.start_texture = TEX_BIRD_BLOOD_GREEN_1;
    data.texture_size = 32;
    data.consecutive_textures = 2;
    data.reset_x_and_offset_y = 0;
    add_texture_offsets(state, &data);

    data.start_texture = TEX_BIRD_BLOOD_GREEN_3;
    data.texture_size = 16;
    data.consecutive_textures = 5;
    data.reset_x_and_offset_y = 32;
    add_texture_offsets(state, &data);

    data.start_texture = TEX_PLAYER_1;
    data.texture_size = 32;
    data.consecutive_textures = 3;
    data.reset_x_and_offset_y = 32;
    add_texture_offsets(state, &data);
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

#ifdef DEBUG
void tex_atlas_debug(State *state) {
    static Tex tex = 0;
    if (IsKeyPressed(KEY_RIGHT)) {
        tex = (tex + 1) % TEX_TOTAL;
    } else if (IsKeyPressed(KEY_LEFT)) {
        tex = ((tex + TEX_TOTAL) - 1) % TEX_TOTAL;
    } else if (IsKeyPressed(KEY_UP)) {
        tex = (tex + 10) % TEX_TOTAL;
    } else if (IsKeyPressed(KEY_DOWN)) {
        tex = ((tex + TEX_TOTAL) - 10) % TEX_TOTAL;
    }
    BeginDrawing();
    Vector2 text_position = {state->game_left, state->game_top};
    ClearBackground(BLACK);
    DrawTextEx(state->bird_computer.font, TextFormat("Texture: %i", tex), text_position, 20 * state->scale_multiplier, 0.0f, WHITE);
    tex_atlas_draw(state, tex, (Vector2){0}, 0.0f, 1.0f, WHITE);
    EndDrawing();
}
#endif