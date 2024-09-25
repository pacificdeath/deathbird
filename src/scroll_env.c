#include "main.h"

static Vector2 env_tex_size() {
    return (Vector2) {
        .x = 2.0f / GAME_WIDTH_RATIO,
        .y = 2.0f / GAME_HEIGHT_RATIO
    };
}

static Vector2 env_tex_offset(Vector2 tex_size) {
    return (Vector2) {
        .x = -(tex_size.x),
        .y = -(tex_size.y / 2)
    };
}

void scroll_env_current_state_setup(State *state, Scroll_Env scroll_envs[SCROLL_ENV_CAPACITY] ) {
    switch (state->game_level) {
    case GAME_LEVEL_NONE: {
        state->scroll_env_amount = 0;
    } break;
    case GAME_LEVEL_FOREST: {
        state->scroll_env_amount = 4;

        scroll_envs[0].tex = TEX_ENV_NIGHT_SKY;
        scroll_envs[0].horizontal_textures = 4;
        scroll_envs[0].horizontal_speed = -0.03f;
        scroll_envs[0].vertical_textures = 2;
        scroll_envs[0].vertical_speed = 0.0f;
        scroll_envs[0].opacity = OPAQUE;

        scroll_envs[1].tex = TEX_ENV_OCEAN,
        scroll_envs[1].horizontal_textures = 4,
        scroll_envs[1].horizontal_speed = 0.03f,
        scroll_envs[1].vertical_textures = 1,
        scroll_envs[1].vertical_speed = 0.0f;
        scroll_envs[1].opacity = OPAQUE;

        scroll_envs[2].tex = TEX_ENV_HILLS;
        scroll_envs[2].horizontal_textures = 4;
        scroll_envs[2].horizontal_speed = 0.15f;
        scroll_envs[2].vertical_textures = 1;
        scroll_envs[2].vertical_speed = 0.0f;
        scroll_envs[2].opacity = OPAQUE;

        scroll_envs[3].tex = TEX_ENV_TREES;
        scroll_envs[3].horizontal_textures = 4;
        scroll_envs[3].horizontal_speed = 0.75f;
        scroll_envs[3].vertical_textures = 1;
        scroll_envs[3].vertical_speed = 0.0f;
        scroll_envs[3].opacity = OPAQUE;
    } break;
    case GAME_LEVEL_FIELD: {
        state->scroll_env_amount = 4;

        scroll_envs[0].tex = TEX_ENV_DAY_SKY;
        scroll_envs[0].horizontal_textures = 4;
        scroll_envs[0].horizontal_speed = -0.03f;
        scroll_envs[0].vertical_textures = 2;
        scroll_envs[0].vertical_speed = 0.0f;
        scroll_envs[0].opacity = OPAQUE;

        scroll_envs[1].tex = TEX_ENV_CLOUDS;
        scroll_envs[1].horizontal_textures = 4;
        scroll_envs[1].horizontal_speed = 0.03f;
        scroll_envs[1].vertical_textures = 1;
        scroll_envs[1].vertical_speed = 0.0f;
        scroll_envs[1].opacity = OPAQUE;

        scroll_envs[2].tex = TEX_ENV_FIELD;
        scroll_envs[2].horizontal_textures = 4;
        scroll_envs[2].horizontal_speed = 0.15f;
        scroll_envs[2].vertical_textures = 1;
        scroll_envs[2].vertical_speed = 0.0f;
        scroll_envs[2].opacity = OPAQUE;

        scroll_envs[3].tex = TEX_ENV_FENCE;
        scroll_envs[3].horizontal_textures = 4;
        scroll_envs[3].horizontal_speed = 0.75f;
        scroll_envs[3].vertical_textures = 1;
        scroll_envs[3].vertical_speed = 0.0f;
        scroll_envs[3].opacity = OPAQUE;
    } break;
    case GAME_LEVEL_SNOW: {
        state->scroll_env_amount = 5;

        scroll_envs[0].tex = TEX_ENV_WINTER_SKY;
        scroll_envs[0].horizontal_textures = 4;
        scroll_envs[0].horizontal_speed = -0.03f;
        scroll_envs[0].vertical_textures = 2;
        scroll_envs[0].vertical_speed = 0.0f;
        scroll_envs[0].opacity = OPAQUE;

        scroll_envs[1].tex = TEX_ENV_MOUNTAINS;
        scroll_envs[1].horizontal_textures = 4;
        scroll_envs[1].horizontal_speed = 0.03f;
        scroll_envs[1].vertical_textures = 1;
        scroll_envs[1].vertical_speed = 0.0f;
        scroll_envs[1].opacity = OPAQUE;

        scroll_envs[2].tex = TEX_ENV_WINTER_HILLS;
        scroll_envs[2].horizontal_textures = 4;
        scroll_envs[2].horizontal_speed = 0.15f;
        scroll_envs[2].vertical_textures = 1;
        scroll_envs[2].vertical_speed = 0.0f;
        scroll_envs[2].opacity = OPAQUE;

        scroll_envs[3].tex = TEX_ENV_WINTER_TREES;
        scroll_envs[3].horizontal_textures = 4;
        scroll_envs[3].horizontal_speed = 0.75f;
        scroll_envs[3].vertical_textures = 1;
        scroll_envs[3].vertical_speed = 0.0f;
        scroll_envs[3].opacity = OPAQUE;

        scroll_envs[4].tex = TEX_ENV_SNOW;
        scroll_envs[4].horizontal_textures = 4;
        scroll_envs[4].horizontal_speed = -0.45f;
        scroll_envs[4].vertical_textures = 3;
        scroll_envs[4].vertical_speed = -0.75f;
        scroll_envs[4].opacity = 128;
    } break;
    }
    for (int i = 0; i < state->scroll_env_amount; i++) {
        scroll_envs[i].scroll.x = 0.0f;
        scroll_envs[i].scroll.y = 0.0f;
    }
}

void scroll_env_update(State *state, Scroll_Env *scroll_envs, float delta_time) {
    Vector2 tex_size = env_tex_size();
    for (int i = 0; i < state->scroll_env_amount; i++) {
        scroll_envs[i].scroll.x += (scroll_envs[i].horizontal_speed * delta_time);
        if (scroll_envs[i].horizontal_speed > 0.0f && scroll_envs[i].scroll.x > 0.0f) {
            scroll_envs[i].scroll.x -= tex_size.x;
        } else if (scroll_envs[i].horizontal_speed < 0.0f && scroll_envs[i].scroll.x < -tex_size.x) {
            scroll_envs[i].scroll.x += tex_size.x;
        }
        scroll_envs[i].scroll.y += (scroll_envs[i].vertical_speed * delta_time);
        if (scroll_envs[i].vertical_speed > 0.0f && scroll_envs[i].scroll.y > 0.0f) {
            scroll_envs[i].scroll.y -= tex_size.y;
        } else if (scroll_envs[i].vertical_speed < 0.0f && scroll_envs[i].scroll.y < -tex_size.y) {
            scroll_envs[i].scroll.y += tex_size.y;
        }
    }
}

void scroll_env_render(State *state, Scroll_Env *scroll_envs) {
    Vector2 tex_size = {
        .x = 2.0f / GAME_WIDTH_RATIO,
        .y = 2.0f / GAME_HEIGHT_RATIO,
    };
    Vector2 tex_offset = env_tex_offset(tex_size);
    for (int i = 0; i < state->scroll_env_amount; i++) {
        for (int x = 0; x < scroll_envs[i].horizontal_textures; x++) {
            for (int y = 0; y < scroll_envs[i].vertical_textures; y++) {
                Vector2 position = {
                    tex_offset.x + scroll_envs[i].scroll.x + (tex_size.x * x),
                    tex_offset.y + scroll_envs[i].scroll.y + (tex_size.y * y)
                };
                tex_atlas_draw(state, scroll_envs[i].tex, position, 0.0f, scroll_envs[i].opacity);
            }
        }
    }
}