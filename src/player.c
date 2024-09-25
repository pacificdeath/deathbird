#include "raylib.h"
#include "main.h"

void player_update(State *state, Player *player, Bird *birds, float delta_time) {
    player->rotation += PLAYER_ROTATION_SPEED * delta_time;
    player->velocity += PLAYER_GRAVITY * delta_time;
    player->position.y -= player->velocity * delta_time;
    if (player->position.y < GROUND_Y) {
        player->position.y = GROUND_Y;
        player->velocity *= -1;
        player->velocity *= PLAYER_GROUND_LOSS;
    }
    if (player->position.y > CEILING_Y) {
        player->position.y = CEILING_Y;
        player->velocity *= -1;
    }
    if (IsKeyPressed(KEY_LEFT)) {
        player->current_key = KEY_LEFT;
    } else if (!IsKeyDown(KEY_LEFT) && player->current_key == KEY_LEFT) {
        player->current_key = 0;
    }
    if (IsKeyPressed(KEY_RIGHT)) {
        player->current_key = KEY_RIGHT;
    } else if (!IsKeyDown(KEY_RIGHT) && player->current_key == KEY_RIGHT) {
        player->current_key = 0;
    }
    switch (player->current_key)
    {
    case KEY_LEFT:
        player->position.x -= PLAYER_HORIZONTAL_SPEED * delta_time;
        break;
    case KEY_RIGHT:
        player->position.x += PLAYER_HORIZONTAL_SPEED * delta_time;
        break;
    }
    for (int i = 0; i < BIRD_AMOUNT; i++) {
        if (birds[i].state != BIRD_STATE_ALIVE) {
            continue;
        }
        float x_distance = fabs(player->position.x - birds[i].position.x);
        float y_distance = fabs(player->position.y - birds[i].position.y);
        if (x_distance < birds[i].collision_radius && y_distance < birds[i].collision_radius) {
            birds[i].health -= player->damage;
            if (birds[i].health <= 0) {
                birds[i].state = BIRD_STATE_DYING;
                Vector2 direction = vec2_normalized(
                    birds[i].position.x - player->position.x,
                    birds[i].position.y - player->position.y
                );
                birds[i].death_master_velocity.x = direction.x * BIRD_DEATH_VELOCITY_MULTIPLIER;
                birds[i].death_master_velocity.y = -direction.y * BIRD_DEATH_VELOCITY_MULTIPLIER;
            }
            if (player->position.y < birds[i].position.y) {
                player->position.y = birds[i].position.y - birds[i].collision_radius;
                player->velocity = PLAYER_BIRD_BOUNCE;
            } else {
                player->position.y = birds[i].position.y + birds[i].collision_radius;
                player->velocity = -PLAYER_BIRD_BOUNCE;
            }
        }
    }
    player->anim_time += delta_time;
    if (player->anim_time > PLAYER_ANIM_SPEED) {
        player->anim_time = 0.0f;
        player->anim_step = (player->anim_step + 1) % PLAYER_ANIM_TEX_AMOUNT;
    }
}

void player_render(State *state, Player *player) {
    tex_atlas_draw(state, TEX_PLAYER_1, player->position, player->rotation, OPAQUE);
}