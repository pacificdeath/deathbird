#include "raylib.h"
#include "main.h"

static void set_bird_available(State *state, Bird *bird) {
    bird->state = BIRD_STATE_AVAILABLE;
    state->birds_in_use--;
    state->level_passed_birds++;
}

void bird_level_setup(State *state) {
    for (int i = 0; i < BIRD_CAPACITY; i++) {
        state->birds[i].state = BIRD_STATE_AVAILABLE;
    }
    state->birds_in_use = 0;
}

void birds_update(State *state) {
    Bird *birds = state->birds;
    if (state->level_requested_birds < state->level_bird_amount) {
        if (state->level_bird_timer < state->level_bird_rate) {
            state->level_bird_timer += state->delta_time;
        } else {
            state->level_bird_timer = 0.0f;
            state->level_requested_birds++;
        }
    }
    for (int i = 0; i < BIRD_CAPACITY; i++) {
        switch (birds[i].state) {
        case BIRD_STATE_AVAILABLE: {
            bool we_need_this_bird = state->level_spawned_birds < state->level_requested_birds;
            if (!we_need_this_bird) {
                break;
            }
            state->birds_in_use++;
            state->level_spawned_birds++;
            birds[i].position.x = BIRD_RESET_RIGHT;
            birds[i].position.y = randf(-BIRD_VERTICAL_FREEDOM, BIRD_VERTICAL_FREEDOM);
            birds[i].move_speed = randf(BIRD_MIN_MOVE_SPEED, BIRD_MAX_MOVE_SPEED);
            switch (state->game_level)
            {
            case GAME_LEVEL_FOREST: {
                birds[i].type = BIRD_TYPE_WHITE;
                birds[i].health = 10;
                birds[i].collision_radius = 0.2f;
            } break;
            case GAME_LEVEL_FIELD: {
                birds[i].type = BIRD_TYPE_YELLOW;
                birds[i].health = 10;
                birds[i].collision_radius = 0.1f;
            } break;
            case GAME_LEVEL_SNOW: {
                birds[i].type = BIRD_TYPE_BROWN;
                birds[i].health = 10;
                birds[i].collision_radius = 0.1f;
            } break;
            }
            birds[i].state = BIRD_STATE_ALIVE;
        } break;
        case BIRD_STATE_ALIVE: {
            birds[i].position.x -= birds[i].move_speed * state->delta_time;
            if (birds[i].position.x < BIRD_RESET_LEFT) {
                set_bird_available(state, &birds[i]);
            } else {
                birds[i].anim_time += state->delta_time;
                if (birds[i].anim_time > BIRD_ANIM_SPEED) {
                    birds[i].anim_time = 0.0f;
                    birds[i].current_tex = (birds[i].current_tex + 1) % BIRD_ANIM_TEX_AMOUNT;
                }
            }
        } break;
        case BIRD_STATE_DYING: {
            for (int j = 0; j < BIRD_DEATH_PARTS; j++) {
                float velocity_x = randf(-BIRD_DEATH_MAX_RANDOM_VELOCITY, BIRD_DEATH_MAX_RANDOM_VELOCITY);
                float velocity_y = randf(-BIRD_DEATH_MAX_RANDOM_VELOCITY, BIRD_DEATH_MAX_RANDOM_VELOCITY);
                birds[i].death_velocities[j].x = birds[i].death_master_velocity.x + velocity_x;
                birds[i].death_velocities[j].y = birds[i].death_master_velocity.y + velocity_y;
                birds[i].death_positions[j] = birds[i].position;
                float angular_velocity = randf(-BIRD_DEATH_MAX_RANDOM_ANGULAR_VELOCITY, BIRD_DEATH_MAX_RANDOM_ANGULAR_VELOCITY);
                birds[i].death_angular_velocities[j] = angular_velocity;
                birds[i].death_rotations[j] = 0.0f;
            }
            birds[i].state = BIRD_STATE_DEAD;
            birds[i].anim_time = 0.0f;
            birds[i].blood_idx = 0;
            int sound_idx = GetRandomValue(0, DEATH_SOUND_AMOUNT - 1);
            PlaySound(state->sounds_death_splats[sound_idx]);
        } break;
        case BIRD_STATE_DEAD: {
            bool all_off_screen = true;
            float out_of_bounds = 0.2;
            for (int j = 0; j < BIRD_DEATH_PARTS; j++) {
                if (
                    birds[i].death_velocities[j].y > BIRD_DEATH_GROUND_BOUNCE_VELOCITY_THRESHOLD &&
                    birds[i].death_positions[j].y < GROUND_Y
                ) {
                    birds[i].death_velocities[j].y *= BIRD_DEATH_GROUND_BOUNCE_VELOCITY;
                } else if (
                    birds[i].death_positions[j].x < (-1.0f - out_of_bounds) ||
                    birds[i].death_positions[j].x > (1.0f + out_of_bounds) ||
                    birds[i].death_positions[j].y < (-1.0f - out_of_bounds)
                    /*  ignore the case when the part is above the screen because it is
                        funny if the bloody bodypart comes back down */
                ) {
                    continue;
                }
                all_off_screen = false;
                birds[i].death_velocities[j].y += BIRD_GRAVITY * state->delta_time;
                birds[i].death_positions[j].x += birds[i].death_velocities[j].x * state->delta_time;
                birds[i].death_positions[j].y -= birds[i].death_velocities[j].y * state->delta_time;
                birds[i].death_rotations[j] += birds[i].death_angular_velocities[j] * state->delta_time;
            }
            if (birds[i].blood_idx <= BIRD_BLOOD_TEX_AMOUNT) {
                birds[i].anim_time += state->delta_time;
                if (birds[i].anim_time > BIRD_ANIM_SPEED) {
                    birds[i].anim_time = 0.0f;
                    birds[i].blood_idx++;
                }
            }
            if (all_off_screen && birds[i].blood_idx >= BIRD_BLOOD_TEX_AMOUNT) {
                set_bird_available(state, &birds[i]);
            }
        } break;
        }
    }
}

bool birds_ready_for_exit(State *state) {
    return state->level_passed_birds == state->level_bird_amount;
}

void birds_render(State *state) {
    Bird *birds = state->birds;
    for (int i = 0; i < BIRD_CAPACITY; i++) {
        switch (birds[i].state) {
        case BIRD_STATE_AVAILABLE:
            break;
        case BIRD_STATE_ALIVE: {
            Tex bird_tex;
            switch (birds[i].type) {
            default:
            case BIRD_TYPE_WHITE: bird_tex = TEX_WHITE_BIRD_1 + birds[i].current_tex; break;
            case BIRD_TYPE_GIANT: bird_tex = TEX_GIANT_BIRD_1 + birds[i].current_tex; break;
            case BIRD_TYPE_YELLOW: bird_tex = TEX_YELLOW_BIRD_1 + birds[i].current_tex; break;
            case BIRD_TYPE_BROWN: bird_tex = TEX_BROWN_BIRD_1 + birds[i].current_tex; break;
            }
            tex_atlas_draw(state, bird_tex, birds[i].position, 0.0f, OPAQUE);
        } break;
        case BIRD_STATE_DYING:
        case BIRD_STATE_DEAD: {
            if (birds[i].blood_idx < BIRD_BLOOD_TEX_AMOUNT) {
                Tex blood_tex = TEX_BIRD_BLOOD_1 + birds[i].blood_idx;
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
                float rotation = birds[i].death_rotations[j];
                tex_atlas_draw(state, tex, birds[i].death_positions[j], rotation, OPAQUE);
            }
        } break;
        }
    }
}