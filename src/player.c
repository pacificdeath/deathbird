#include <stdio.h>
#include "raylib.h"
#include "main.h"

static void handle_input(State *state) {
    if (IsKeyPressed(KEY_LEFT)) {
        state->player_current_input = KEY_LEFT;
    } else if (!IsKeyDown(KEY_LEFT) && state->player_current_input == KEY_LEFT) {
        state->player_current_input = 0;
    }
    if (IsKeyPressed(KEY_RIGHT)) {
        state->player_current_input = KEY_RIGHT;
    } else if (!IsKeyDown(KEY_RIGHT) && state->player_current_input == KEY_RIGHT) {
        state->player_current_input = 0;
    }
    switch (state->player_current_input) {
    case KEY_LEFT: {
        state->player_position.x -= PLAYER_HORIZONTAL_SPEED * state->delta_time;
        if (state->player_position.x < PLAYER_MAX_LEFT) {
            state->player_position.x = PLAYER_MAX_LEFT;
        }
        state->player_rotation -= PLAYER_ROTATION_SPEED_GROUND_MOVEMENT * state->delta_time;
    } break;
    case KEY_RIGHT: {
        state->player_position.x += PLAYER_HORIZONTAL_SPEED * state->delta_time;
        if (state->player_position.x > PLAYER_MAX_RIGHT) {
            state->player_position.x = PLAYER_MAX_RIGHT;
        }
        state->player_rotation += PLAYER_ROTATION_SPEED_GROUND_MOVEMENT * state->delta_time;
    } break;
    }
}

static void handle_score(State *state) {
    state->player_state = PLAYER_STATE_GROUNDED;
    int multiplier = state->bird_multiplier;
    state->bird_multiplier = 0;
    if (multiplier == 0) {
        return;
    }
    int last_place = MENU_LINE_COUNT - 1;
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
    state->bird_multiplier_timer = PLAYER_MULTIPLIER_DISPLAY_TIME;
}

void player_level_setup(State *state) {
    state->player_state = PLAYER_STATE_INSIDE_PORTAL;
    state->player_position.x = 0.0f;
    state->player_position.y = 0.0f;
    state->level_score = 0;
    for (int i = 0; i < BIRD_TYPES_TOTAL; i++) {
        state->birds_destroyed[i] = 0;
    }
    state->bird_multiplier = 0;
    state->bird_multiplier_timer = 0.0f;
    for (int i = 0; i < MENU_LINE_COUNT - 1; i++) {
        state->bird_highest_multipliers[i] = 0;
    }
}

void player_update(State *state) {
    Bird *birds = state->birds;
    bool down_arrow_hold = IsKeyDown(KEY_DOWN);
    bool up_arrow_hold = IsKeyDown(KEY_UP);
    switch (state->player_state) {
    case PLAYER_STATE_NONE:
    case PLAYER_STATE_INSIDE_PORTAL:
        break;
    case PLAYER_STATE_EXHALED_BY_PORTAL: {
        Vector2 portal_position = portal_get_position(state);
        float portal_distance = vec2_distance(portal_position, state->player_position);
        float base_speed = 0.1f;
        state->player_position.y -= (base_speed + (portal_distance / PORTAL_RADIUS)) * state->delta_time;
        state->player_rotation -= PLAYER_ROTATION_SPEED_PORTAL * state->delta_time;
        if (state->player_position.y <= GAME_GROUND_Y) {
            state->player_state = PLAYER_STATE_GROUNDED;
        }
    } break;
    case PLAYER_STATE_GROUNDED: {
        handle_input(state);
        state->player_position.y = GAME_GROUND_Y;
        state->player_rotation += PLAYER_ROTATION_SPEED_GROUND * state->delta_time;
        if (up_arrow_hold) {
            state->player_state = PLAYER_STATE_UP;
        }
    } break;
    case PLAYER_STATE_UP:
    case PLAYER_STATE_DOWN: {
        handle_input(state);
        int dir = state->player_state == PLAYER_STATE_UP ? 1 : -1;
        state->player_position.y += dir * PLAYER_VERTICAL_SPEED * state->delta_time;
        state->player_rotation += PLAYER_ROTATION_SPEED_AIR * state->delta_time;
        if (state->player_position.y < GAME_GROUND_Y) {
            state->player_position.y = GAME_GROUND_Y;
            handle_score(state);
            state->player_state = PLAYER_STATE_GROUNDED;
        } else if (state->player_position.y > GAME_CEILING_Y) {
            handle_score(state);
            state->player_state = PLAYER_STATE_DOWN;
        }

        Bird *bird_that_was_hit;
        bool bird_hit = false;

        float closest;
        if (state->player_state == PLAYER_STATE_UP) {
            closest = 100.0f;
        } else { // PLAYER_STATE_DOWN
            closest = -100.0f;
        }

        for (int i = 0; i < BIRD_CAPACITY; i++) {
            if (birds[i].state != BIRD_STATE_ALIVE) {
                continue;
            }

            float x_distance = birds[i].position.x - state->player_position.x;
            x_distance = fabs(x_distance);
            bool x_inside_bird_bounds = x_distance < birds[i].alive.collision_bounds.x;
            if (!x_inside_bird_bounds) {
                continue;
            }

            float y_distance = birds[i].position.y - state->player_position.y;
            float y_distance_abs = fabs(y_distance);
            bool y_inside_bird_bounds = y_distance_abs < birds[i].alive.collision_bounds.y;
            if (!y_inside_bird_bounds) {
                continue;
            }

            bool y_closest_bird;
            if (state->player_state == PLAYER_STATE_UP) {
                y_closest_bird = y_distance < closest;
            } else { // PLAYER_STATE_DOWN
                y_closest_bird = y_distance > closest;
            }

            if (!y_inside_bird_bounds || !y_closest_bird) {
                continue;
            }

            bird_that_was_hit = &birds[i];
            bird_hit = true;
            closest = y_distance_abs;
        }

        if (bird_hit) {
            float pre_destruction_collision_bounds_y = bird_that_was_hit->alive.collision_bounds.y;
            bool bird_was_destroyed = bird_try_destroy(state, bird_that_was_hit, state->player_position);
            if (state->player_state == PLAYER_STATE_UP) {
                if (!up_arrow_hold || !bird_was_destroyed) {
                    state->player_position.y = bird_that_was_hit->position.y - pre_destruction_collision_bounds_y;
                    state->player_state = PLAYER_STATE_DOWN;
                }
            } else {
                if (!down_arrow_hold || !bird_was_destroyed) {
                    state->player_position.y = bird_that_was_hit->position.y + pre_destruction_collision_bounds_y;
                    state->player_state = PLAYER_STATE_UP;
                }
            }
        }
    } break;
    case PLAYER_STATE_INHALED_BY_PORTAL: {
        Vector2 portal_position = portal_get_position(state);
        Vector2 direction = vec2_direction(state->player_position, portal_position);
        direction = vec2_normalized(direction.x, direction.y);
        direction.x *= PORTAL_ATTRACT_MULTIPLIER * state->delta_time;
        direction.y *= PORTAL_ATTRACT_MULTIPLIER * state->delta_time;

        bool x_fully_inhaled = portal_inhale_object(
            &(state->player_position.x),
            direction.x,
            portal_position.x
        );
        bool y_fully_inhaled = portal_inhale_object(
            &(state->player_position.y),
            direction.y,
            portal_position.y
        );

        state->player_rotation -= PLAYER_ROTATION_SPEED_PORTAL * state->delta_time;
        
        if (x_fully_inhaled && y_fully_inhaled) {
            state->player_state = PLAYER_STATE_INSIDE_PORTAL;
        }
    } break;
    }
    if (state->bird_multiplier_timer > 0.0f) {
        state->bird_multiplier_timer -= state->delta_time;
    }
}

void player_render(State *state) {
    switch (state->player_state) {
    default: {
        atlas_draw(
            state,
            TEX_PLAYER_2,
            state->player_position,
            state->player_rotation,
            1.0f,
            OPAQUE
        );
    } break;
    case PLAYER_STATE_INHALED_BY_PORTAL:
    case PLAYER_STATE_EXHALED_BY_PORTAL: {
        float x = portal_distance_to_center_ratio(state, state->player_position);
        float scale = x;
        float c = x * 255.0f;
        Color color = {c,c,c,255};
        atlas_draw(
            state,
            TEX_PLAYER_2,
            state->player_position,
            state->player_rotation,
            scale,
            color
        );
    } break;
    case PLAYER_STATE_NONE:
    case PLAYER_STATE_INSIDE_PORTAL:
        break;
    }
}
