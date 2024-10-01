#include "raylib.h"
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

static int scroll_env_level_setup(State *state, Level_Environment environment) {
    Scroll_Env *scroll_envs = state->scroll_envs;
    int scroll_env_amount = 0;
    switch (environment) {
    case LEVEL_ENVIRONMENT_NONE: break;
    case LEVEL_ENVIRONMENT_FOREST: {
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

        scroll_env_amount = 4;
    } break;
    case LEVEL_ENVIRONMENT_MEADOWS: {
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

        scroll_envs[2].tex = TEX_ENV_MEADOWS;
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

        scroll_env_amount = 4;
    } break;
    case LEVEL_ENVIRONMENT_MOUNTAINS: {
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

        scroll_env_amount = 5;
    } break;
    }
    for (int i = 0; i < scroll_env_amount; i++) {
        scroll_envs[i].scroll.x = 0.0f;
        scroll_envs[i].scroll.y = 0.0f;
    }
    return scroll_env_amount;
}

void level_setup_next(State *state) {
    Level_Environment environment;
    switch (state->current_level_data.environment) {
    case LEVEL_ENVIRONMENT_NONE: {
        environment = LEVEL_ENVIRONMENT_FOREST;
    } break;
    case LEVEL_ENVIRONMENT_FOREST: {
        environment = LEVEL_ENVIRONMENT_MEADOWS;
    } break;
    case LEVEL_ENVIRONMENT_MEADOWS: {
        environment = LEVEL_ENVIRONMENT_MOUNTAINS;
    } break;
    case LEVEL_ENVIRONMENT_MOUNTAINS: {
        environment = LEVEL_ENVIRONMENT_FOREST;
    } break;
    }
    int next_round = state->current_round + 1;

    state->next_level_data.environment = environment;
    state->next_level_data.scroll_env_amount = scroll_env_level_setup(state, environment);

    int extra_birds = next_round * LEVEL_BIRD_AMOUNT_MULTIPLIER;
    int extra_birds_random = GetRandomValue(LEVEL_MIN_RANDOM_BIRD_AMOUNT, LEVEL_MAX_RANDOM_BIRD_AMOUNT);
    state->next_level_data.total_birds = LEVEL_BASE_BIRD_TOTAL + extra_birds + extra_birds_random;

    if (next_round >= LEVELS_BEFORE_MIN_FREQUENCY) {
        state->next_level_data.bird_frequency = LEVEL_MIN_BIRD_FREQUENCY;
    } else {
        float min_max_frequency_diff = LEVEL_MAX_BIRD_FREQUENCY - LEVEL_MIN_BIRD_FREQUENCY;
        float bird_frequency_loss_per_level = min_max_frequency_diff / LEVELS_BEFORE_MIN_FREQUENCY;
        float bird_frequency_loss = bird_frequency_loss_per_level * next_round;
        state->next_level_data.bird_frequency = LEVEL_MAX_BIRD_FREQUENCY - bird_frequency_loss;
    }
    float extra_bird_frequency = randf(LEVEL_MIN_RANDOM_BIRD_FREQUENCY, LEVEL_MAX_RANDOM_BIRD_FREQUENCY);
    state->next_level_data.bird_frequency += extra_bird_frequency;

    state->next_level_data.requested_birds = 0;
    state->next_level_data.spawned_birds = 0;
    state->next_level_data.passed_birds = 0;
}

void level_go_to_next(State *state) {
    state->current_round++;
    state->current_level_data = state->next_level_data;
}

void level_update(State *state) {
    Scroll_Env *scroll_envs = state->scroll_envs;
    Vector2 tex_size = env_tex_size();
    for (int i = 0; i < state->current_level_data.scroll_env_amount; i++) {
        scroll_envs[i].scroll.x += (scroll_envs[i].horizontal_speed * state->delta_time);
        if (scroll_envs[i].horizontal_speed > 0.0f && scroll_envs[i].scroll.x > 0.0f) {
            scroll_envs[i].scroll.x -= tex_size.x;
        } else if (scroll_envs[i].horizontal_speed < 0.0f && scroll_envs[i].scroll.x < -tex_size.x) {
            scroll_envs[i].scroll.x += tex_size.x;
        }
        scroll_envs[i].scroll.y += (scroll_envs[i].vertical_speed * state->delta_time);
        if (scroll_envs[i].vertical_speed > 0.0f && scroll_envs[i].scroll.y > 0.0f) {
            scroll_envs[i].scroll.y -= tex_size.y;
        } else if (scroll_envs[i].vertical_speed < 0.0f && scroll_envs[i].scroll.y < -tex_size.y) {
            scroll_envs[i].scroll.y += tex_size.y;
        }
    }
}

void level_render(State *state) {
    Scroll_Env *scroll_envs = state->scroll_envs;
    Vector2 tex_size = {
        .x = 2.0f / GAME_WIDTH_RATIO,
        .y = 2.0f / GAME_HEIGHT_RATIO,
    };
    Vector2 tex_offset = env_tex_offset(tex_size);
    for (int i = 0; i < state->current_level_data.scroll_env_amount; i++) {
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
