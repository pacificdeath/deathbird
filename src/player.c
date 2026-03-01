#define MAX_LEFT -0.9f
#define MAX_RIGHT 0.9f
#define ROTATION_SPEED_GROUND -450.0f
#define ROTATION_SPEED_GROUND_MOVEMENT 1200.0f
#define ROTATION_SPEED_AIR 2500.0f
#define PLAYER_ACCELERATION_MULTIPLIER 20.0f

bool is_player_vertical_movement_enabled() {
    return !has_flag(state->flags, FLAG_CONSOLE) && state->menu_state == MENU_STATE_INACTIVE;
}

bool player_key_pressed(int key) {
    if (is_player_vertical_movement_enabled()) {
        return IsKeyPressed(key);
    }
    return false;
}

bool player_key_down(int key) {
    if (is_player_vertical_movement_enabled()) {
        return IsKeyDown(key);
    }
    return false;
}

void handle_input() {
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

void player_prepare() {
    Player *player = &state->player;
    player->state = PLAYER_STATE_GROUNDED;
    player->vertical_speed = 1.3f;
    player->horizontal_speed = 1.6f;
    player->may_not_interact_with_this_bird_idx = -1;
    player->last_turnaround_y = -1.0f;
    player->may_not_interact_with_this_bird_idx = -1;
}

void player_turn_around() {
    switch (state->player.state) {
        case PLAYER_STATE_UP: state->player.state = PLAYER_STATE_DOWN; break;
        case PLAYER_STATE_DOWN: state->player.state = PLAYER_STATE_UP; break;
        default: break;
    }
    state->player.last_turnaround_y = state->player.spinner_position.y;
}

void player_update() {
    Player *player = &state->player;
    Bird *birds = state->birds;

    bool console_active = has_flag(state->flags, FLAG_CONSOLE);
    bool down_arrow_hold = console_active ? false : player_key_down(KEY_DOWN);
    bool up_arrow_hold = console_active ? false : player_key_down(KEY_UP);

    switch (player->state) {
        case PLAYER_STATE_NONE:
            break;
        case PLAYER_STATE_GROUNDED: {
            handle_input();
            player->spinner_position.y = GAME_GROUND_Y;
            player->rotation += ROTATION_SPEED_GROUND * state->delta_time;
            if (up_arrow_hold && state->global_state == GLOBAL_STATE_GAME) {
                player->state = PLAYER_STATE_UP;
            }
        } break;
        case PLAYER_STATE_RETURN_TO_GROUND: {
            handle_input();
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
            handle_input();
            float dir = player->state == PLAYER_STATE_UP ? 1.0f : -1.0f;
            player->spinner_position.y += dir * player->vertical_speed * state->delta_time;
            player->rotation += ROTATION_SPEED_AIR * state->delta_time;
            const float oob = 0.5f;
            const float oob_top = 1 + oob;
            const float oob_bottom = - 1 - oob;
            if (player->spinner_position.y < oob_bottom ||
                player->spinner_position.y > oob_top
            ) {
                game_over();
                return;
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
                x_distance = fabsf(x_distance);
                bool x_inside_bird_bounds = x_distance < birds[i].alive.collision_bounds.x;
                if (!x_inside_bird_bounds) {
                    continue;
                }

                float y_distance = birds[i].position.y - player->spinner_position.y;
                float y_distance_abs = fabsf(y_distance);
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
                int hit = (int)apply_damage_to_bird_from_player(&birds[bird_hit_idx]);

                if (has_flag(hit, BIRD_HIT_DESTROYED)) {
                    player->may_not_interact_with_this_bird_idx = -1;
                    if (player->state == PLAYER_STATE_UP) {
                        if (up_arrow_hold) {
                            if (!has_flag(hit, BIRD_HIT_PASS_THROUGH_ALLOWED)) {
                                player_turn_around();
                            }
                        } else {
                            if (has_flag(hit, BIRD_HIT_BOUNCE_ALLOWED)) {
                                player_turn_around();
                            }
                        }
                    } else {
                        if (down_arrow_hold) {
                            if (!has_flag(hit, BIRD_HIT_PASS_THROUGH_ALLOWED)) {
                                player_turn_around();
                            }
                        } else {
                            if (has_flag(hit, BIRD_HIT_BOUNCE_ALLOWED)) {
                                player_turn_around();
                            }
                        }
                    }
                } else {
                    player_turn_around();
                    player->may_not_interact_with_this_bird_idx = bird_hit_idx;
                }
            }
        } break;
    }
}

void player_render() {
    Player *player = &state->player;

    static int sprite = -1;
    if (sprite < 0) {
        switch (GetRandomValue(1,3)) {
            default: ASSERT(false);
            case 1: sprite = SPRITE_PLAYER1; break;
            case 2: sprite = SPRITE_PLAYER2; break;
            case 3: sprite = SPRITE_PLAYER3; break;
        }
    }

    atlas_draw(sprite, player->spinner_position, player->rotation, SCALE_NORMAL, OPAQUE);
}
