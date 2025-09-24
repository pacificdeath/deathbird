#define INFINITE_LIVES_LEVEL 1
#define PLAYER_MAX_LIVES 4
#define DEFAULT_LIVES 4
#define MAX_LEFT -0.9f
#define MAX_RIGHT 0.9f
#define ROTATION_SPEED_GROUND -450.0f
#define ROTATION_SPEED_GROUND_MOVEMENT 1200.0f
#define ROTATION_SPEED_PORTAL 1000.0f
#define ROTATION_SPEED_AIR 2500.0f
#define MULTIPLIER_DISPLAY_TIME 1.0f
#define PLAYER_ACCELERATION_MULTIPLIER 20.0f

static inline bool player_has_infinite_lives() {
    return state->level_idx == INFINITE_LIVES_LEVEL;
}

static inline bool player_key_pressed(int key) {
    if (has_flag(state->flags, FLAG_CONSOLE)) return false;
    return IsKeyPressed(key);
}

static inline bool player_key_down(int key) {
    if (has_flag(state->flags, FLAG_CONSOLE)) return false;
    return IsKeyDown(key);
}

static void handle_input(State *state) {
    Player *player = &state->player;

    if (player_key_pressed(KEY_LEFT)) {
        player->current_input = KEY_LEFT;
    } else if (!player_key_down(KEY_LEFT) && player->current_input == KEY_LEFT) {
        player->current_input = 0;
    }
    if (player_key_pressed(KEY_RIGHT)) {
        player->current_input = KEY_RIGHT;
    } else if (!player_key_down(KEY_RIGHT) && player->current_input == KEY_RIGHT) {
        player->current_input = 0;
    }
    switch (player->current_input) {
        case KEY_LEFT:
            if (player->horizontal_acceleration > 0.0f) {
                player->horizontal_acceleration = 0.0f;
            }
            player->horizontal_acceleration -= state->delta_time * PLAYER_ACCELERATION_MULTIPLIER;
            if (player->horizontal_acceleration < -1.0f) {
                player->horizontal_acceleration = -1.0f;
            }
            player->rotation -= ROTATION_SPEED_GROUND_MOVEMENT * state->delta_time;
            break;
        case KEY_RIGHT:
            if (player->horizontal_acceleration < 0.0f) {
                player->horizontal_acceleration = 0.0f;
            }
            player->horizontal_acceleration += state->delta_time * PLAYER_ACCELERATION_MULTIPLIER;
            if (player->horizontal_acceleration > 1.0f) {
                player->horizontal_acceleration = 1.0f;
            }
            player->rotation += ROTATION_SPEED_GROUND_MOVEMENT * state->delta_time;
            break;
        default:
            player->horizontal_acceleration = 0.0f;
    }

    player->spinner_position.x += (player->horizontal_speed * player->horizontal_acceleration * state->delta_time);
    if (player->spinner_position.x < MAX_LEFT) {
        player->spinner_position.x = MAX_LEFT;
    }
    if (player->spinner_position.x > MAX_RIGHT) {
        player->spinner_position.x = MAX_RIGHT;
    }
}

static void handle_score(State *state) {
    Player *player = &state->player;
    if (!player_has_infinite_lives()) {
        player->lives--;
    }

    player->state = PLAYER_STATE_GROUNDED;
    int multiplier = state->bird_multiplier;
    state->bird_multiplier = 0;
    if (multiplier == 0) {
        return;
    }
    int last_place = BIRD_HIGHEST_MULTIPLIER_CAPACITY - 1;
    if (multiplier > state->bird_highest_multipliers[last_place]) {
        state->bird_highest_multipliers[last_place] = multiplier;
        for (int i = last_place - 1; i >= 0; i--) {
            if (multiplier > state->bird_highest_multipliers[i]) {
                state->bird_highest_multipliers[i + 1] = state->bird_highest_multipliers[i];
                state->bird_highest_multipliers[i] = multiplier;
            }
        }
    }
    state->level_score += (multiplier * multiplier);
    state->bird_multiplier_display = multiplier;
    state->bird_multiplier_timer = MULTIPLIER_DISPLAY_TIME;
    if (!level_target_reached()) {
        state->portal_fuel = 0;
    }
}

void player_init(Player *p) {
    p->state = PLAYER_STATE_GROUNDED;
    p->vertical_speed = 1.0f;
    p->horizontal_speed = 1.6f;
    p->may_not_interact_with_this_bird_idx = -1;
    p->lives = DEFAULT_LIVES;
}

void player_level_setup(State *state) {
    Player *player = &state->player;

    state->level_score = 0;
    for (int i = 0; i < BIRD_TYPE_TOTAL; i++) {
        state->birds_destroyed[i] = 0;
    }
    state->bird_multiplier = 0;
    state->bird_multiplier_timer = 0.0f;
    for (int i = 0; i < BIRD_HIGHEST_MULTIPLIER_CAPACITY - 1; i++) {
        state->bird_highest_multipliers[i] = 0;
    }
    player->last_turnaround_y = -1.0f;
    player->may_not_interact_with_this_bird_idx = -1;
}

void player_add_life() {
    if (state->player.lives < PLAYER_MAX_LIVES) {
        state->player.lives++;
    }
}

static void player_turn_around(State *state) {
    switch (state->player.state) {
    case PLAYER_STATE_UP: state->player.state = PLAYER_STATE_DOWN; break;
    case PLAYER_STATE_DOWN: state->player.state = PLAYER_STATE_UP; break;
    default: break;
    }
    state->player.last_turnaround_y = state->player.spinner_position.y;
}

void player_update(State *state) {
    Player *player = &state->player;
    Bird *birds = state->birds;

    bool console_active = has_flag(state->flags, FLAG_CONSOLE);
    bool down_arrow_hold = console_active ? false : player_key_down(KEY_DOWN);
    bool up_arrow_hold = console_active ? false : player_key_down(KEY_UP);

    switch (player->state) {
        case PLAYER_STATE_NONE:
        case PLAYER_STATE_INSIDE_PORTAL:
            break;
        case PLAYER_STATE_EXHALED_BY_PORTAL: {
            Vector2 portal_position = portal_get_position(state);
            float portal_distance = vec2_distance(portal_position, player->spinner_position);
            float base_speed = 0.1f;
            player->spinner_position.y -= (base_speed + (portal_distance / PORTAL_RADIUS)) * state->delta_time;
            player->rotation -= ROTATION_SPEED_PORTAL * state->delta_time;
            if (player->spinner_position.y <= GAME_GROUND_Y) {
                player->state = PLAYER_STATE_GROUNDED;
            }
        } break;
        case PLAYER_STATE_LOST_LIFE: {
            handle_input(state);
            player->spinner_position.y = GAME_GROUND_Y;
            player->rotation += ROTATION_SPEED_GROUND * state->delta_time;
            player->reset_flicker_timer += state->delta_time;
            if (player->reset_flicker_timer > 0.1f) {
                player->reset_flicker_count++;
                player->reset_flicker_timer = 0.0f;
                if (has_flag(player->flags, PLAYER_FLAG_HIDDEN)) {
                    player->flags &= ~PLAYER_FLAG_HIDDEN;
                } else {
                    player->flags |= PLAYER_FLAG_HIDDEN;
                }
            }
            if (player->reset_flicker_count == 6) {
                player->reset_flicker_count = 0;
                player->state = PLAYER_STATE_GROUNDED;
            }
        } break;
        case PLAYER_STATE_GROUNDED: {
            handle_input(state);
            player->spinner_position.y = GAME_GROUND_Y;
            player->rotation += ROTATION_SPEED_GROUND * state->delta_time;
            if (up_arrow_hold && state->global_state == GLOBAL_STATE_GAME) {
                player->state = PLAYER_STATE_UP;
            }
        } break;
        case PLAYER_STATE_RETURN_TO_GROUND: {
            handle_input(state);
            if (player->spinner_position.y > GAME_GROUND_Y) {
                player->spinner_position.y -= (player->vertical_speed * state->delta_time);
            }
            if (player->spinner_position.y <= GAME_GROUND_Y) {
                player->spinner_position.y = GAME_GROUND_Y;
                player->state = PLAYER_STATE_GROUNDED;
            }
            player->rotation += ROTATION_SPEED_AIR * state->delta_time;
        } break;
        case PLAYER_STATE_UP:
        case PLAYER_STATE_DOWN: {
            handle_input(state);
            int dir = player->state == PLAYER_STATE_UP ? 1 : -1;
            player->spinner_position.y += dir * player->vertical_speed * state->delta_time;
            player->rotation += ROTATION_SPEED_AIR * state->delta_time;
            const float oob = 0.5f;
            const float oob_top = 1 + oob;
            const float oob_bottom = - 1 - oob;
            if (player->spinner_position.y < oob_bottom ||
                player->spinner_position.y > oob_top
            ) {
                player->spinner_position.x = 0.0f;
                player->spinner_position.y = GAME_GROUND_Y;
                player->last_turnaround_y = -1.0f;
                player->may_not_interact_with_this_bird_idx = -1;
                handle_score(state);
                player->state = PLAYER_STATE_LOST_LIFE;
            }

            int bird_hit_idx;
            bool was_bird_hit = false;

            float closest;
            if (player->state == PLAYER_STATE_UP) {
                closest = 100.0f;
            } else { // PLAYER_STATE_DOWN
                closest = -100.0f;
            }

            for (int i = 0; i < BIRD_CAPACITY; i++) {
                if (birds[i].state != BIRD_STATE_ALIVE) {
                    continue;
                }

                if (i == player->may_not_interact_with_this_bird_idx) {
                    continue;
                }

                if (player->state == PLAYER_STATE_UP) {
                    if (birds[i].position.y < player->last_turnaround_y) {
                        continue;
                    }
                } else {
                    if (birds[i].position.y > player->last_turnaround_y) {
                        continue;
                    }
                }

                float x_distance = birds[i].position.x - player->spinner_position.x;
                x_distance = fabs(x_distance);
                bool x_inside_bird_bounds = x_distance < birds[i].alive.collision_bounds.x;
                if (!x_inside_bird_bounds) {
                    continue;
                }

                float y_distance = birds[i].position.y - player->spinner_position.y;
                float y_distance_abs = fabs(y_distance);
                bool y_inside_bird_bounds = y_distance_abs < birds[i].alive.collision_bounds.y;
                if (!y_inside_bird_bounds) {
                    continue;
                }

                bool y_closest_bird;
                if (player->state == PLAYER_STATE_UP) {
                    y_closest_bird = y_distance < closest;
                } else { // PLAYER_STATE_DOWN
                    y_closest_bird = y_distance > closest;
                }

                if (!y_inside_bird_bounds || !y_closest_bird) {
                    continue;
                }

                bird_hit_idx = i;
                was_bird_hit = true;
                closest = y_distance_abs;
            }

            if (was_bird_hit) {
                BirdHit hit = apply_damage_to_bird_from_player(&birds[bird_hit_idx]);

                if (has_flag(hit, BIRD_HIT_DESTROYED)) {
                    player->may_not_interact_with_this_bird_idx = -1;
                    if (player->state == PLAYER_STATE_UP) {
                        if (up_arrow_hold) {
                            if (!has_flag(hit, BIRD_HIT_PASS_THROUGH_ALLOWED)) {
                                player_turn_around(state);
                            }
                        } else {
                            if (has_flag(hit, BIRD_HIT_BOUNCE_ALLOWED)) {
                                player_turn_around(state);
                            }
                        }
                    } else {
                        if (down_arrow_hold) {
                            if (!has_flag(hit, BIRD_HIT_PASS_THROUGH_ALLOWED)) {
                                player_turn_around(state);
                            }
                        } else {
                            if (has_flag(hit, BIRD_HIT_BOUNCE_ALLOWED)) {
                                player_turn_around(state);
                            }
                        }
                    }
                } else {
                    player_turn_around(state);
                    player->may_not_interact_with_this_bird_idx = bird_hit_idx;
                }
            }
        } break;
        case PLAYER_STATE_INHALED_BY_PORTAL: {
            Vector2 portal_position = portal_get_position(state);
            Vector2 direction = vec2_direction(player->spinner_position, portal_position);
            direction = vec2_normalized(direction.x, direction.y);
            direction.x *= state->portal_power * state->delta_time;
            direction.y *= state->portal_power * state->delta_time;

            bool x_fully_inhaled = portal_inhale_object(
                &(player->spinner_position.x),
                direction.x,
                portal_position.x
            );
            bool y_fully_inhaled = portal_inhale_object(
                &(player->spinner_position.y),
                direction.y,
                portal_position.y
            );

            player->rotation -= ROTATION_SPEED_PORTAL * state->delta_time;

            if (x_fully_inhaled && y_fully_inhaled) {
                player->state = PLAYER_STATE_INSIDE_PORTAL;
            }
        } break;
    }
    if (state->bird_multiplier_timer > 0.0f) {
        state->bird_multiplier_timer -= state->delta_time;
    }
}

void player_render(State *state) {
    Player *player = &state->player;

    int sprite;
    switch (player->lives) {
        case 1: sprite = SPRITE_PLAYER3; break;
        case 2: sprite = SPRITE_PLAYER2; break;
        case 3: sprite = SPRITE_PLAYER1; break;
        case 4: sprite = SPRITE_PLAYER1; break;
    }

    switch (player->state) {
        case PLAYER_STATE_GROUNDED:
        case PLAYER_STATE_RETURN_TO_GROUND:
        case PLAYER_STATE_UP:
        case PLAYER_STATE_DOWN: {
            atlas_draw(state, sprite, player->spinner_position, player->rotation, SCALE_NORMAL, OPAQUE);
        } break;
        case PLAYER_STATE_LOST_LIFE: {
            if (has_flag(player->flags, PLAYER_FLAG_HIDDEN)) {
                break;
            }
            atlas_draw(state, sprite, player->spinner_position, player->rotation, SCALE_NORMAL, OPAQUE);
        } break;
        case PLAYER_STATE_INHALED_BY_PORTAL:
        case PLAYER_STATE_EXHALED_BY_PORTAL: {
            float ratio = portal_distance_to_center_ratio(state, player->spinner_position);
            float c = ratio * 255.0f;
            Color color = {c,c,c,255};
            atlas_draw(state, SPRITE_PLAYER2, player->spinner_position, player->rotation, SCALE_UNIFORM(ratio), color);
        } break;
        case PLAYER_STATE_NONE:
        case PLAYER_STATE_INSIDE_PORTAL:
            break;
    }
}
