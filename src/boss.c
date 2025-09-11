#include "main.h"

#define BOSS_SPEED 10.0f
#define BOSS_DEFAULT_HEIGHT 0.5f

enum Boss_State {
    BOSS_ENTER,
    BOSS_NORMAL,
};

void boss_init(State *state) {
    state->boss.position = (Vector2){0.0f,10.0f};
    state->boss.state = BOSS_ENTER;
}

void boss_update(State *state) {
    Boss *boss = &state->boss;
    switch (boss->state) {
        case BOSS_ENTER: {
            if (boss->position.y < BOSS_DEFAULT_HEIGHT) {
                boss->position.y = BOSS_DEFAULT_HEIGHT;
                boss->state = BOSS_NORMAL;
            }
            boss->position.y -= BOSS_SPEED * state->delta_time;
        } break;
        case BOSS_NORMAL: {
        } break;
        default: ASSERT(false);
    }
}

void boss_render(State *state) {
    Boss *boss = &state->boss;
    atlas_draw(state, 0, boss->position, 0.0f, 1.0f, OPAQUE);
}

