#include <stdio.h>
#include "raylib.h"
#include "main.h"

static void handle_score(State *state) {
    Player *player = &state->player;
    player->state = PLAYER_STATE_GROUNDED;
    int multiplier = player->bird_multiplier;
    player->bird_multiplier = 0;
    if (multiplier < PLAYER_SMALLEST_VALID_MULTIPLIER) {
        return;
    }
    int last_place = BIRD_COMPUTER_LINE_COUNT - 1;
    if (multiplier > player->highest_multipliers[last_place]) {
        player->highest_multipliers[last_place] = multiplier;
        for (int i = last_place - 1; i >= 0; i--) {
            if (multiplier > player->highest_multipliers[i]) {
                player->highest_multipliers[i + 1] = player->highest_multipliers[i];
                player->highest_multipliers[i] = multiplier;
            }
        }
    }
    player->level_score += (multiplier * multiplier);
    player->bird_multiplier_display = multiplier;
    player->bird_multiplier_timer = PLAYER_MULTIPLIER_DISPLAY_TIME;
}

void player_level_setup(State *state) {
    state->player.damage = 10;
    state->player.state = PLAYER_STATE_GROUNDED;
    state->player.position.x = 0.0f;
    state->player.position.y = GAME_GROUND_Y;
    state->player.level_score = 0;
    for (int i = 0; i < BIRD_TYPES_TOTAL; i++) {
        state->player.obliterated_birds[i] = 0;
    }
    state->player.bird_multiplier = 0;
    state->player.bird_multiplier_timer = 0.0f;
    for (int i = 0; i < BIRD_COMPUTER_LINE_COUNT - 1; i++) {
        state->player.highest_multipliers[i] = 0;
    }
}

void player_update(State *state) {
    bool down_arrow_hold = IsKeyDown(KEY_DOWN);
    bool up_arrow_hold = IsKeyDown(KEY_UP);
    Player *player = &state->player;
    Bird *birds = state->birds;
    if (IsKeyPressed(KEY_LEFT)) {
        player->current_input_key = KEY_LEFT;
    } else if (!IsKeyDown(KEY_LEFT) && player->current_input_key == KEY_LEFT) {
        player->current_input_key = 0;
    }
    if (IsKeyPressed(KEY_RIGHT)) {
        player->current_input_key = KEY_RIGHT;
    } else if (!IsKeyDown(KEY_RIGHT) && player->current_input_key == KEY_RIGHT) {
        player->current_input_key = 0;
    }
    switch (player->current_input_key) {
    case KEY_LEFT: {
        player->position.x -= PLAYER_HORIZONTAL_SPEED * state->delta_time;
        player->rotation -= PLAYER_ROTATION_SPEED_GROUND_MOVEMENT * state->delta_time;
    } break;
    case KEY_RIGHT: {
        player->position.x += PLAYER_HORIZONTAL_SPEED * state->delta_time;
        player->rotation += PLAYER_ROTATION_SPEED_GROUND_MOVEMENT * state->delta_time;
    } break;
    }
    if (player->state == PLAYER_STATE_GROUNDED) {
        player->position.y = GAME_GROUND_Y;
        player->rotation += PLAYER_ROTATION_SPEED_GROUND * state->delta_time;
        if (up_arrow_hold) {
            player->state = PLAYER_STATE_UP;
        }
    } else {
        int dir = player->state == PLAYER_STATE_UP ? 1 : -1;
        player->position.y += dir * PLAYER_VERTICAL_SPEED * state->delta_time;
        player->rotation += PLAYER_ROTATION_SPEED_AIR * state->delta_time;
        if (player->position.y < GAME_GROUND_Y) {
            player->position.y = GAME_GROUND_Y;
            handle_score(state);
            player->state = PLAYER_STATE_GROUNDED;
        } else if (player->position.y > GAME_CEILING_Y) {
            handle_score(state);
            player->state = PLAYER_STATE_DOWN;
        }
        Bird *bird;
        bool bird_hit = false;
        float closest = 100.0f;
        for (int i = 0; i < BIRD_CAPACITY; i++) {
            if (birds[i].state != BIRD_STATE_ALIVE) {
                continue;
            }
            float x_distance = fabs(player->position.x - birds[i].position.x);
            float y_distance = fabs(player->position.y - birds[i].position.y);
            if (x_distance < birds[i].alive.collision_radius && y_distance < birds[i].alive.collision_radius) {
                if (y_distance < closest) {
                    bird = &birds[i];
                    bird_hit = true;
                    closest = y_distance;
                }
            }
        }
        if (bird_hit) {
            bool bird_was_destroyed = bird_try_destroy(state, bird, player->position);
            if (player->state == PLAYER_STATE_UP) {
                if (!up_arrow_hold || !bird_was_destroyed) {
                    player->position.y = bird->position.y - bird->alive.collision_radius;
                    player->state = PLAYER_STATE_DOWN;
                }
            } else {
                if (!down_arrow_hold || !bird_was_destroyed) {
                    player->position.y = bird->position.y + bird->alive.collision_radius;
                    player->state = PLAYER_STATE_UP;
                }
            }
        }
    }
    if (state->player.bird_multiplier_timer > 0.0f) {
        state->player.bird_multiplier_timer -= state->delta_time;
    }
}

bool player_ready_for_exit(State *state) {
    return state->player.bird_multiplier == 0 && state->player.bird_multiplier_timer <= 0.0f;
}

void player_render(State *state) {
    tex_atlas_draw(state, TEX_PLAYER_2, state->player.position, state->player.rotation, OPAQUE);
}