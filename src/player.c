#include "raylib.h"
#include "main.h"

#define MAX_LEFT -0.9f
#define MAX_RIGHT 0.9f
#define ROTATION_SPEED_GROUND -450.0f
#define ROTATION_SPEED_GROUND_MOVEMENT 1200.0f
#define ROTATION_SPEED_PORTAL 1000.0f
#define ROTATION_SPEED_AIR 2499.0f
#define MULTIPLIER_DISPLAY_TIME 1.0f

static void handle_input(State *state) {
    Player *player = &state->player;

    if (IsKeyPressed(KEY_LEFT)) {
        player->current_input = KEY_LEFT;
    } else if (!IsKeyDown(KEY_LEFT) && player->current_input == KEY_LEFT) {
        player->current_input = 0;
    }
    if (IsKeyPressed(KEY_RIGHT)) {
        player->current_input = KEY_RIGHT;
    } else if (!IsKeyDown(KEY_RIGHT) && player->current_input == KEY_RIGHT) {
        player->current_input = 0;
    }
    switch (player->current_input) {
    case KEY_LEFT: {
        player->spinner_position.x -= player->horizontal_speed * state->delta_time;
        if (player->spinner_position.x < MAX_LEFT) {
            player->spinner_position.x = MAX_LEFT;
        }
        player->rotation -= ROTATION_SPEED_GROUND_MOVEMENT * state->delta_time;
    } break;
    case KEY_RIGHT: {
        player->spinner_position.x += player->horizontal_speed * state->delta_time;
        if (player->spinner_position.x > MAX_RIGHT) {
            player->spinner_position.x = MAX_RIGHT;
        }
        player->rotation += ROTATION_SPEED_GROUND_MOVEMENT * state->delta_time;
    } break;
    }
    if (player->state == PLAYER_STATE_GROUNDED) {
        player->home_position = player->spinner_position.x;
    }
}

static void handle_score(State *state) {
    Player *player = &state->player;

    player->state = PLAYER_STATE_GROUNDED;
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
    state->bird_multiplier_timer = MULTIPLIER_DISPLAY_TIME;
}

void player_init(State *state) {
    state->player.vertical_speed = 1.5f;
    state->player.horizontal_speed = 2.0f;
}

void player_level_setup(State *state) {
    Player *player = &state->player;

    player->state = PLAYER_STATE_GROUNDED;
    player->spinner_position.x = 0.0f;
    player->spinner_position.y = 0.0f;
    state->level_score = 0;
    for (int i = 0; i < BIRD_TYPES_TOTAL; i++) {
        state->birds_destroyed[i] = 0;
    }
    state->bird_multiplier = 0;
    state->bird_multiplier_timer = 0.0f;
    for (int i = 0; i < MENU_LINE_COUNT - 1; i++) {
        state->bird_highest_multipliers[i] = 0;
    }
    player->last_turnaround_y = -1.0f;
    player->may_not_interact_with_this_bird_idx = -1;
}

void player_update(State *state) {
    Player *player = &state->player;
    Bird *birds = state->birds;

    bool down_arrow_hold = IsKeyDown(KEY_DOWN);
    bool up_arrow_hold = IsKeyDown(KEY_UP);
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
    case PLAYER_STATE_GROUNDED: {
        handle_input(state);
        player->spinner_position.y = GAME_GROUND_Y;
        player->rotation += ROTATION_SPEED_GROUND * state->delta_time;
        if (up_arrow_hold) {
            player->state = PLAYER_STATE_UP;
        }
    } break;
    case PLAYER_STATE_UP:
    case PLAYER_STATE_DOWN: {
        handle_input(state);
        int dir = player->state == PLAYER_STATE_UP ? 1 : -1;
        player->spinner_position.y += dir * player->vertical_speed * state->delta_time;
        player->rotation += ROTATION_SPEED_AIR * state->delta_time;
        if (player->spinner_position.y < GAME_GROUND_Y) {
            player->spinner_position.y = GAME_GROUND_Y;
            player->last_turnaround_y = -1.0f;
            player->may_not_interact_with_this_bird_idx = -1;
            handle_score(state);
            player->state = PLAYER_STATE_GROUNDED;
        } else if (player->spinner_position.y > GAME_CEILING_Y) {
            player->last_turnaround_y = 1.0f;
            player->may_not_interact_with_this_bird_idx = -1;
            handle_score(state);
            player->state = PLAYER_STATE_DOWN;
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
            bool bird_was_destroyed = bird_try_destroy_by_player(state, &birds[bird_hit_idx]);
            if (bird_was_destroyed) {
                player->may_not_interact_with_this_bird_idx = -1;
            } else {
                player->may_not_interact_with_this_bird_idx = bird_hit_idx;
            }
            if (player->state == PLAYER_STATE_UP) {
                if (!up_arrow_hold || !bird_was_destroyed) {
                    player->state = PLAYER_STATE_DOWN;
                    player->last_turnaround_y = player->spinner_position.y;
                }
            } else {
                if (!down_arrow_hold || !bird_was_destroyed) {
                    player->state = PLAYER_STATE_UP;
                    player->last_turnaround_y = player->spinner_position.y;
                }
            }
        }
    } break;
    case PLAYER_STATE_INHALED_BY_PORTAL: {
        Vector2 portal_position = portal_get_position(state);
        Vector2 direction = vec2_direction(player->spinner_position, portal_position);
        direction = vec2_normalized(direction.x, direction.y);
        direction.x *= PORTAL_ATTRACT_MULTIPLIER * state->delta_time;
        direction.y *= PORTAL_ATTRACT_MULTIPLIER * state->delta_time;

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

    switch (player->state) {
    default: {
        atlas_draw(
            state,
            TEX_PLAYER_2,
            player->spinner_position,
            player->rotation,
            1.0f,
            OPAQUE
        );
    } break;
    case PLAYER_STATE_INHALED_BY_PORTAL:
    case PLAYER_STATE_EXHALED_BY_PORTAL: {
        float x = portal_distance_to_center_ratio(state, player->spinner_position);
        float scale = x;
        float c = x * 255.0f;
        Color color = {c,c,c,255};
        atlas_draw(
            state,
            TEX_PLAYER_2,
            player->spinner_position,
            player->rotation,
            scale,
            color
        );
    } break;
    case PLAYER_STATE_NONE:
    case PLAYER_STATE_INSIDE_PORTAL:
        break;
    }

    Vector2 home_position = { player->home_position, -0.9f };
    atlas_draw(state, TEX_GIANT_BIRD_1, home_position, 0, 1.0f, WHITE);
}
