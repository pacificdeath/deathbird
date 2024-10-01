#include <stdio.h>
#include "raylib.h"
#include "main.h"

static bool should_display_multiplier(State *state) {
    return state->player.bird_multiplier_timer < PLAYER_MULTIPLIER_DISPLAY_TIME;
}

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
    } else {
        return;
    }
    for (int i = last_place - 1; i >= 0; i--) {
        if (multiplier > player->highest_multipliers[i]) {
            player->highest_multipliers[i + 1] = player->highest_multipliers[i];
            player->highest_multipliers[i] = multiplier;
        }
    }
    player->level_score += (multiplier * multiplier) - multiplier;
    player->bird_multiplier_display = multiplier;
    player->bird_multiplier_timer = 0.0f;
}

void player_level_setup(State *state) {
    state->player.damage = 10;
    state->player.state = PLAYER_STATE_GROUNDED;
    state->player.position.x = 0.0f;
    state->player.position.y = GROUND_Y;
    state->player.level_score = 0;
    for (int i = 0; i < BIRD_TYPES_TOTAL; i++) {
        state->player.obliterated_birds[i] = 0;
    }
    state->player.bird_multiplier = 0;
    state->player.bird_multiplier_timer = PLAYER_MULTIPLIER_DISPLAY_TIME;
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
    } break;
    case KEY_RIGHT: {
        player->position.x += PLAYER_HORIZONTAL_SPEED * state->delta_time;
    } break;
    }
    if (player->state == PLAYER_STATE_GROUNDED) {
        player->position.y = GROUND_Y;
        if (up_arrow_hold) {
            player->state = PLAYER_STATE_UP;
        } else {
            player->rotation += PLAYER_ROTATION_SPEED * state->delta_time;
        }
    } else {
        int dir = player->state == PLAYER_STATE_UP ? 1 : -1;
        player->position.y += dir * PLAYER_VERTICAL_SPEED * state->delta_time;
        player->rotation += PLAYER_ROTATION_SPEED_FAST * state->delta_time;
        if (player->position.y < GROUND_Y) {
            player->position.y = GROUND_Y;
            handle_score(state);
            player->state = PLAYER_STATE_GROUNDED;
        } else if (player->position.y > CEILING_Y) {
            handle_score(state);
            player->state = PLAYER_STATE_DOWN;
        }
        for (int i = 0; i < BIRD_CAPACITY; i++) {
            if (birds[i].state != BIRD_STATE_ALIVE) {
                continue;
            }
            float x_distance = fabs(player->position.x - birds[i].position.x);
            float y_distance = fabs(player->position.y - birds[i].position.y);
            if (x_distance < birds[i].collision_radius && y_distance < birds[i].collision_radius) {
                birds[i].health -= player->damage;
                if (birds[i].health <= 0) {
                    state->player.level_score++;
                    state->player.bird_multiplier++;
                    player->obliterated_birds[birds[i].type]++;
                    birds[i].state = BIRD_STATE_DYING;
                    Vector2 direction = vec2_normalized(
                        birds[i].position.x - player->position.x,
                        birds[i].position.y - player->position.y
                    );
                    birds[i].death_master_velocity.x = direction.x * BIRD_DEATH_VELOCITY_MULTIPLIER;
                    birds[i].death_master_velocity.y = -direction.y * BIRD_DEATH_VELOCITY_MULTIPLIER;
                }
                if (player->state == PLAYER_STATE_UP) {
                    if (!up_arrow_hold) {
                        player->position.y = birds[i].position.y - birds[i].collision_radius;
                        player->state = PLAYER_STATE_DOWN;
                    }
                } else {
                    if (!down_arrow_hold) {
                        player->position.y = birds[i].position.y + birds[i].collision_radius;
                        player->state = PLAYER_STATE_UP;
                    }
                }
            }
        }
    }
    if (should_display_multiplier(state)) {
        state->player.bird_multiplier_timer += state->delta_time;
    }
}

bool player_ready_for_exit(State *state) {
    return state->player.bird_multiplier == 0 && !should_display_multiplier(state);
}

void player_render(State *state) {
    char buffer[16];
    sprintf(buffer, " Round: %i Score: %i ", state->current_round, state->player.level_score);
    int font_size = (BIRD_COMPUTER_FONT_SIZE / state->bird_computer.font.baseSize) * state->scale_multiplier;
    Vector2 score_text_dimensions = MeasureTextEx(state->bird_computer.font, buffer, font_size, 0);
    Vector2 position = { .x = state->game_left, .y = state->game_top };
    DrawRectangleV(position, score_text_dimensions, (Color) { 0, 0, 0, 128 });
    DrawTextPro(state->bird_computer.font, buffer, position, (Vector2) { 0, 0 }, 0, font_size, 0, WHITE);
    if (should_display_multiplier(state)) {
        sprintf(buffer, " %ix-Multiplier ", state->player.bird_multiplier_display);
        Vector2 multiplier_text_dimensions = MeasureTextEx(state->bird_computer.font, buffer, font_size, 0);
        position.x = state->game_left + score_text_dimensions.x;
        DrawRectangleV(position, multiplier_text_dimensions, (Color) { 255, 0, 0, 128 });
        DrawTextPro(state->bird_computer.font, buffer, position, (Vector2) { 0, 0 }, 0, font_size, 0, WHITE);
    }
    tex_atlas_draw(state, TEX_PLAYER_2, state->player.position, state->player.rotation, OPAQUE);
}