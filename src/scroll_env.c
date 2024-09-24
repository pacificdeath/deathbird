#include "main.h"

void scroll_env_current_state_setup(State *state, Scroll_Env scroll_envs[SCROLL_ENV_AMOUNT] ) {
    Vector2 env_block_size = {
        .x = 2.0f / GAME_WIDTH_RATIO,
        .y = 2.0f / GAME_HEIGHT_RATIO
    };
    for (int i = 0; i < SCROLL_ENV_AMOUNT; i++) {
        scroll_envs[i].scroll.x = 0.0f;
        scroll_envs[i].scroll.y = 0.0f;
        scroll_envs[i].offset = (Vector2) {
            .x = -(env_block_size.x),
            .y = -(env_block_size.y / 2)
        };
    }
    switch (state->game_level) {
    case GAME_LEVEL_FOREST: {
        scroll_envs[0].tex = TEX_ENV_NIGHT_SKY;
        scroll_envs[0].type = SCROLL_TYPE_LEFT;
        scroll_envs[0].vertical_amount = 2;
        scroll_envs[0].horizontal_amount = 4;
        scroll_envs[0].speed = 0.03f;

        scroll_envs[1].tex = TEX_ENV_OCEAN,
        scroll_envs[1].type = SCROLL_TYPE_RIGHT,
        scroll_envs[1].vertical_amount = 1,
        scroll_envs[1].horizontal_amount = 4,
        scroll_envs[1].speed = 0.03f,

        scroll_envs[2].tex = TEX_ENV_HILLS;
        scroll_envs[2].type = SCROLL_TYPE_RIGHT;
        scroll_envs[2].vertical_amount = 1;
        scroll_envs[2].horizontal_amount = 4;
        scroll_envs[2].speed = 0.15f;

        scroll_envs[3].tex = TEX_ENV_TREES;
        scroll_envs[3].type = SCROLL_TYPE_RIGHT;
        scroll_envs[3].vertical_amount = 1;
        scroll_envs[3].horizontal_amount = 4;
        scroll_envs[3].speed = 0.75f;
    } break;
    case GAME_LEVEL_FIELD: {
        scroll_envs[0].tex = TEX_ENV_DAY_SKY;
        scroll_envs[0].type = SCROLL_TYPE_LEFT;
        scroll_envs[0].vertical_amount = 2;
        scroll_envs[0].horizontal_amount = 4;
        scroll_envs[0].speed = 0.03f;

        scroll_envs[1].tex = TEX_ENV_CLOUDS;
        scroll_envs[1].type = SCROLL_TYPE_RIGHT;
        scroll_envs[1].vertical_amount = 1;
        scroll_envs[1].horizontal_amount = 4;
        scroll_envs[1].speed = 0.03f;

        scroll_envs[2].tex = TEX_ENV_FIELD;
        scroll_envs[2].type = SCROLL_TYPE_RIGHT;
        scroll_envs[2].vertical_amount = 1;
        scroll_envs[2].horizontal_amount = 4;
        scroll_envs[2].speed = 0.15f;

        scroll_envs[3].tex = TEX_ENV_FENCE;
        scroll_envs[3].type = SCROLL_TYPE_RIGHT;
        scroll_envs[3].vertical_amount = 1;
        scroll_envs[3].horizontal_amount = 4;
        scroll_envs[3].speed = 0.75f;
    } break;
    }
}

void scroll_env_update(State *state, Scroll_Env *scroll_envs, float delta_time) {
    float loop_threshold;
    for (int i = 0; i < SCROLL_ENV_AMOUNT; i++) {
        switch (scroll_envs[i].type) {
        case SCROLL_TYPE_RIGHT:
        case SCROLL_TYPE_LEFT: {
            loop_threshold = 2.0f / GAME_WIDTH_RATIO;
        } break;
        case SCROLL_TYPE_DOWN:
        case SCROLL_TYPE_UP: {
            loop_threshold = 2.0f / GAME_HEIGHT_RATIO;
        } break;
        }
        switch (scroll_envs[i].type) {
        case SCROLL_TYPE_RIGHT: {
            scroll_envs[i].scroll.x += (scroll_envs[i].speed * delta_time);
            if (scroll_envs[i].scroll.x > loop_threshold) {
                scroll_envs[i].scroll.x -= loop_threshold;
            }
        } break;
        case SCROLL_TYPE_LEFT: {
            scroll_envs[i].scroll.x -= (scroll_envs[i].speed * delta_time);
            if (scroll_envs[i].scroll.x < 0.0f) {
                scroll_envs[i].scroll.x += loop_threshold;
            }
        } break;
        case SCROLL_TYPE_DOWN: {
            scroll_envs[i].scroll.y -= (scroll_envs[i].speed * delta_time);
            if (scroll_envs[i].scroll.y > loop_threshold) {
                scroll_envs[i].scroll.y -= loop_threshold;
            }
        } break;
        case SCROLL_TYPE_UP: {
            scroll_envs[i].scroll.y += (scroll_envs[i].speed * delta_time);
            if (scroll_envs[i].scroll.y < 0.0f) {
                scroll_envs[i].scroll.y += loop_threshold;
            }
        } break;
        }
    }
}

void scroll_env_render(State *state, Scroll_Env *scroll_envs) {
    Vector2 block_size = {
        .x = 2.0f / GAME_WIDTH_RATIO,
        .y = 2.0f / GAME_HEIGHT_RATIO,
    };
    for (int i = 0; i < SCROLL_ENV_AMOUNT; i++) {
        for (int x = 0; x < scroll_envs[i].horizontal_amount; x++) {
            for (int y = 0; y < scroll_envs[i].vertical_amount; y++) {
                Vector2 position = {
                    scroll_envs[i].offset.x + scroll_envs[i].scroll.x + (block_size.x * x),
                    scroll_envs[i].offset.y + scroll_envs[i].scroll.y + (block_size.y * y)
                };
                switch (scroll_envs[i].type) {
                case SCROLL_TYPE_RIGHT:
                case SCROLL_TYPE_LEFT: {
                    position.x -= block_size.x;
                } break;
                case SCROLL_TYPE_DOWN:
                case SCROLL_TYPE_UP: {
                    position.y += block_size.y;
                } break;
                }
                tex_atlas_draw(state, scroll_envs[i].tex, position, 0.0f);
            }
        }
    }
}