#include <math.h>
#include "raylib.h"
#include "main.h"

static void set_bird_available(State *state, Bird *bird) {
    bird->state = BIRD_STATE_AVAILABLE;
    state->birds_in_use--;
    state->current_level_data.passed_birds++;
}

void bird_init(State *state) {
    state->bird_damage_shader = LoadShader(0, "./src/shaders/bird_damage.fs");
}

void bird_cleanup(State *state) {
    UnloadShader(state->bird_damage_shader);
}

void bird_level_setup(State *state) {
    for (int i = 0; i < BIRD_CAPACITY; i++) {
        state->birds[i].state = BIRD_STATE_AVAILABLE;
    }
    state->birds_in_use = 0;
}

void birds_update(State *state) {
    Bird *birds = state->birds;
    if (state->current_level_data.requested_birds < state->current_level_data.total_birds) {
        if (state->current_level_data.bird_timer < state->current_level_data.bird_frequency) {
            state->current_level_data.bird_timer += state->delta_time;
        } else {
            state->current_level_data.bird_timer = 0.0f;
            state->current_level_data.requested_birds++;
        }
    }
    for (int i = 0; i < BIRD_CAPACITY; i++) {
        switch (birds[i].state) {
        case BIRD_STATE_AVAILABLE: {
            bool we_need_this_bird = state->current_level_data.spawned_birds < state->current_level_data.requested_birds;
            if (!we_need_this_bird) {
                break;
            }
            state->birds_in_use++;
            state->current_level_data.spawned_birds++;
            birds[i].alive = (Bird_Alive){0};
            birds[i].position.x = BIRD_RESET_RIGHT;
            birds[i].position.y = randf(-BIRD_VERTICAL_FREEDOM, BIRD_VERTICAL_FREEDOM);
            birds[i].alive.move_speed = randf(BIRD_MIN_MOVE_SPEED, BIRD_MAX_MOVE_SPEED);
            birds[i].alive.damage_timer = 0.0f;
            switch (state->current_level_data.environment) {
            default:
            case LEVEL_ENVIRONMENT_NONE: {
                birds[i].type = BIRD_TYPE_GIANT;
                birds[i].alive.health = 255;
                birds[i].alive.collision_radius = 0.0f;
            } break;
            case LEVEL_ENVIRONMENT_FOREST: {
                if (state->current_level_data.spawned_birds % 10 == 0) {
                    birds[i].type = BIRD_TYPE_GIANT;
                    birds[i].alive.health = 2;
                    birds[i].alive.collision_radius = 0.2f;
                } else {
                    birds[i].type = BIRD_TYPE_WHITE;
                    birds[i].alive.health = 1;
                    birds[i].alive.collision_radius = 0.1f;
                }
            } break;
            case LEVEL_ENVIRONMENT_MEADOWS: {
                birds[i].type = BIRD_TYPE_YELLOW;
                birds[i].alive.health = 1;
                birds[i].alive.collision_radius = 0.1f;
            } break;
            case LEVEL_ENVIRONMENT_MOUNTAINS: {
                birds[i].type = BIRD_TYPE_BROWN;
                birds[i].alive.health = 1;
                birds[i].alive.collision_radius = 0.1f;
            } break;
            }
            birds[i].state = BIRD_STATE_ALIVE;
        } break;
        case BIRD_STATE_ALIVE: {
            Bird_Alive *alive_bird = &birds[i].alive;
            birds[i].position.x -= alive_bird->move_speed * state->delta_time;
            if (birds[i].position.x < BIRD_RESET_LEFT) {
                set_bird_available(state, &birds[i]);
            } else {
                alive_bird->anim_time += state->delta_time;
                if (alive_bird->anim_time > BIRD_ANIM_SPEED) {
                    alive_bird->anim_time = 0.0f;
                    alive_bird->current_tex = (alive_bird->current_tex + 1) % BIRD_ANIM_TEX_AMOUNT;
                }
            }
            if (alive_bird->damage_timer > 0.0f) {
                alive_bird->damage_timer -= state->delta_time;
            }
        } break;
        case BIRD_STATE_DEAD: {
            Bird_Dead *dead_bird = &birds[i].dead;
            bool all_off_screen = true;
            float out_of_bounds = 0.2;
            for (int j = 0; j < BIRD_DEATH_PARTS; j++) {
                if (
                    dead_bird->death_velocities[j].y > BIRD_DEATH_GROUND_BOUNCE_VELOCITY_THRESHOLD &&
                    dead_bird->death_positions[j].y < GAME_GROUND_Y
                ) {
                    dead_bird->death_velocities[j].y *= BIRD_DEATH_GROUND_BOUNCE_VELOCITY;
                } else if (
                    dead_bird->death_positions[j].x < (-1.0f - out_of_bounds) ||
                    dead_bird->death_positions[j].x > (1.0f + out_of_bounds) ||
                    dead_bird->death_positions[j].y < (-1.0f - out_of_bounds)
                    /*  ignore the case when the part is above the screen because it is
                        funny if the bloody bodypart comes back down */
                ) {
                    continue;
                }
                all_off_screen = false;
                dead_bird->death_velocities[j].y += BIRD_GRAVITY * state->delta_time;
                dead_bird->death_positions[j].x += dead_bird->death_velocities[j].x * state->delta_time;
                dead_bird->death_positions[j].y -= dead_bird->death_velocities[j].y * state->delta_time;
                dead_bird->death_rotations[j] += dead_bird->death_angular_velocities[j] * state->delta_time;
            }
            if (dead_bird->blood_idx <= BIRD_BLOOD_TEX_AMOUNT) {
                dead_bird->anim_time += state->delta_time;
                if (dead_bird->anim_time > BIRD_ANIM_SPEED) {
                    dead_bird->anim_time = 0.0f;
                    dead_bird->blood_idx++;
                }
            }
            if (all_off_screen && dead_bird->blood_idx >= BIRD_BLOOD_TEX_AMOUNT) {
                set_bird_available(state, &birds[i]);
            }
        } break;
        }
    }
}

bool bird_try_destroy(State *state, Bird *bird, Vector2 from) {
    Bird_Alive *alive_bird = &bird->alive;
    alive_bird->health--;
    if (alive_bird->health <= 0) {
        state->player.level_score++;
        state->player.bird_multiplier++;
        state->player.obliterated_birds[bird->type]++;
        Vector2 master_velocity = vec2_normalized(
            bird->position.x - from.x,
            bird->position.y - from.y
        );
        master_velocity.x *= BIRD_DEATH_VELOCITY_MULTIPLIER;
        master_velocity.y *= -BIRD_DEATH_VELOCITY_MULTIPLIER;
        bird->dead = (Bird_Dead){0};
        Bird_Dead *dead_bird = &bird->dead;
        for (int j = 0; j < BIRD_DEATH_PARTS; j++) {
            dead_bird->death_positions[j] = bird->position;
            Vector2 velocity = {
                master_velocity.x + randf(-BIRD_DEATH_MAX_RANDOM_VELOCITY, BIRD_DEATH_MAX_RANDOM_VELOCITY),
                master_velocity.y + randf(-BIRD_DEATH_MAX_RANDOM_VELOCITY, BIRD_DEATH_MAX_RANDOM_VELOCITY)
            };
            dead_bird->death_velocities[j] = velocity;
            float angular_velocity = randf(-BIRD_DEATH_MAX_RANDOM_ANGULAR_VELOCITY, BIRD_DEATH_MAX_RANDOM_ANGULAR_VELOCITY);
            dead_bird->death_angular_velocities[j] = angular_velocity;
            dead_bird->death_rotations[j] = 0.0f;
        }
        bird->state = BIRD_STATE_DEAD;
        int sound_idx = GetRandomValue(0, DEATH_SOUND_AMOUNT - 1);
        PlaySound(state->sounds_death_splats[sound_idx]);
        if (bird->type == BIRD_TYPE_GIANT) {
            for (int i = 0; i < BIRD_CAPACITY; i++) {
                if (state->birds[i].state != BIRD_STATE_ALIVE) {
                    continue;
                }
                float x_distance = fabs(bird->position.x - state->birds[i].position.x) * GAME_WIDTH_RATIO;
                float y_distance = fabs(bird->position.y - state->birds[i].position.y) * GAME_HEIGHT_RATIO;
                float hypotenuse = sqrtf(x_distance*x_distance + y_distance*y_distance);
                if (hypotenuse < BIRD_GIANT_SPLASH_RADIUS) {
                    bird_try_destroy(state, &state->birds[i], bird->position);
                }
            }
        }
        return true;
    } else {
        alive_bird->damage_timer = BIRD_DAMAGE_TIME;
        return false;
    }
}

bool birds_ready_for_exit(State *state) {
    return state->current_level_data.passed_birds == state->current_level_data.total_birds;
}

void birds_render(State *state) {
    Bird *birds = state->birds;
    for (int i = 0; i < BIRD_CAPACITY; i++) {
        switch (birds[i].state) {
        case BIRD_STATE_AVAILABLE:
            break;
        case BIRD_STATE_ALIVE: {
            Bird_Alive *alive_bird = &birds[i].alive;
            Tex bird_tex;
            switch (birds[i].type) {
            default:
            case BIRD_TYPE_WHITE: bird_tex = TEX_WHITE_BIRD_1 + alive_bird->current_tex; break;
            case BIRD_TYPE_GIANT: bird_tex = TEX_GIANT_BIRD_1 + alive_bird->current_tex; break;
            case BIRD_TYPE_YELLOW: bird_tex = TEX_YELLOW_BIRD_1 + alive_bird->current_tex; break;
            case BIRD_TYPE_BROWN: bird_tex = TEX_BROWN_BIRD_1 + alive_bird->current_tex; break;
            }
            if (alive_bird->damage_timer > 0.0f) {
                BeginShaderMode(state->bird_damage_shader);
                tex_atlas_draw(state, bird_tex, birds[i].position, 0.0f, OPAQUE);
                EndShaderMode();
            } else {
                tex_atlas_draw(state, bird_tex, birds[i].position, 0.0f, OPAQUE);
            }
        } break;
        case BIRD_STATE_DEAD: {
            Bird_Dead *dead_bird = &birds[i].dead;
            if (dead_bird->blood_idx < BIRD_BLOOD_TEX_AMOUNT) {
                Tex blood_tex = TEX_BIRD_BLOOD_1 + dead_bird->blood_idx;
                tex_atlas_draw(state, blood_tex, birds[i].position, 0.0f, OPAQUE);
            }
            Tex textures[BIRD_DEATH_PARTS];
            // in order of rendering
            for (int j = 0; j < BIRD_DEATH_GORE_PARTS; j += BIRD_GORE_TEX_AMOUNT) {
                for (int k = 0; k < BIRD_GORE_TEX_AMOUNT; k++) {
                    textures[j + k] = TEX_BIRD_GORE_1 + k;
                }
            }
            switch (birds[i].type)
            {
            default:
            case BIRD_TYPE_WHITE: {
                textures[BIRD_DEATH_PARTS - 6] = TEX_WHITE_BIRD_WING;
                textures[BIRD_DEATH_PARTS - 5] = TEX_WHITE_BIRD_BODY;
                textures[BIRD_DEATH_PARTS - 4] = TEX_WHITE_BIRD_WING;
                textures[BIRD_DEATH_PARTS - 3] = TEX_BIRD_EYE;
                textures[BIRD_DEATH_PARTS - 2] = TEX_WHITE_BIRD_HEAD;
                textures[BIRD_DEATH_PARTS - 1] = TEX_BIRD_EYE;
            } break;
            case BIRD_TYPE_GIANT: {
                textures[BIRD_DEATH_PARTS - 6] = TEX_GIANT_BIRD_WING;
                textures[BIRD_DEATH_PARTS - 5] = TEX_GIANT_BIRD_BODY;
                textures[BIRD_DEATH_PARTS - 4] = TEX_GIANT_BIRD_WING;
                textures[BIRD_DEATH_PARTS - 3] = TEX_BIRD_EYE;
                textures[BIRD_DEATH_PARTS - 2] = TEX_GIANT_BIRD_HEAD;
                textures[BIRD_DEATH_PARTS - 1] = TEX_BIRD_EYE;
            } break;
            case BIRD_TYPE_YELLOW: {
                textures[BIRD_DEATH_PARTS - 6] = TEX_YELLOW_BIRD_WING;
                textures[BIRD_DEATH_PARTS - 5] = TEX_YELLOW_BIRD_BODY;
                textures[BIRD_DEATH_PARTS - 4] = TEX_YELLOW_BIRD_WING;
                textures[BIRD_DEATH_PARTS - 3] = TEX_BIRD_EYE;
                textures[BIRD_DEATH_PARTS - 2] = TEX_YELLOW_BIRD_HEAD;
                textures[BIRD_DEATH_PARTS - 1] = TEX_BIRD_EYE;
            } break;
            case BIRD_TYPE_BROWN: {
                textures[BIRD_DEATH_PARTS - 6] = TEX_BROWN_BIRD_WING;
                textures[BIRD_DEATH_PARTS - 5] = TEX_BROWN_BIRD_BODY;
                textures[BIRD_DEATH_PARTS - 4] = TEX_BROWN_BIRD_WING;
                textures[BIRD_DEATH_PARTS - 3] = TEX_BIRD_EYE;
                textures[BIRD_DEATH_PARTS - 2] = TEX_BROWN_BIRD_HEAD;
                textures[BIRD_DEATH_PARTS - 1] = TEX_BIRD_EYE;
            } break;
            }
            for (int j = 0; j < BIRD_DEATH_PARTS; j++) {
                Tex tex = textures[j];
                float rotation = dead_bird->death_rotations[j];
                tex_atlas_draw(state, tex, dead_bird->death_positions[j], rotation, OPAQUE);
            }
        } break;
        }
    }
}