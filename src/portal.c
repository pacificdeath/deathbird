#include "raylib.h"
#include "rlgl.h"
#include "main.h"

// between 0.0f -> 1.0f
static void update_smooth_disappearance(Portal *p) {
    float l = p->linear_disappearance;
    p->smooth_disappearance = 0.5f * sin((l * PI) - (PI * 0.5f)) + 0.5f;
}

Vector2 portal_position(State *state) {
    return (Vector2) {0};
}

void portal_init(State *state) {
    state->portal.linear_disappearance = 1.0f;
    state->portal.smooth_disappearance = 1.0f;
    Image image = GenImageColor(1, 1, WHITE);
    state->portal.texture = LoadTextureFromImage(image);
    SetShapesTexture(state->portal.texture, (Rectangle){ 0.0f, 0.0f, 1.0f, 1.0f });
    state->portal.shader = LoadShader((void *)0, "./src/shaders/portal.fs");
}

void portal_cleanup(State *state) {
    UnloadTexture(state->portal.texture);
    UnloadShader(state->portal.shader);
}

void portal_setup(State *state, Portal_Bits flags) {
    Portal *p = &state->portal;
    p->color.r = has_flag(flags, PORTAL_BIT_RED) * 255;
    p->color.g = has_flag(flags, PORTAL_BIT_GREEN) * 255;
    p->color.b = has_flag(flags, PORTAL_BIT_BLUE) * 255;
    p->color.a = 1.0f;
    p->timer = 0.0f;
    p->item_idx = 0;
}

bool portal_appear(State *state) {
    bool completed = false;
    Portal *p = &state->portal;
    p->linear_disappearance -= PORTAL_APPEAR_SPEED * state->delta_time;
    if (p->linear_disappearance < 0.0f) {
        p->linear_disappearance = 0.0f;
        completed = true;
    }
    update_smooth_disappearance(p);
    return completed;
}

bool portal_disappear(State *state) {
    bool completed = false;
    Portal *p = &state->portal;
    p->linear_disappearance += PORTAL_DISAPPEAR_SPEED * state->delta_time;
    if (p->linear_disappearance > 1.0f) {
        p->linear_disappearance = 1.0f;
        completed = true;
    }
    update_smooth_disappearance(p);
    return completed;
}

bool portal_exhale(State *state) {
    Portal *p = &state->portal;
    bool completed = false;
    if (p->timer <= 0.0f) {
        p->timer = PORTAL_EXHALE_RATE;
        switch (p->item_idx) {
        case 0: {
            state->player.state = PLAYER_STATE_PORTAL;
        }
        }
        p->item_idx++;
    }
    if (state->player.state != PLAYER_STATE_PORTAL) {
        completed = true;
    }
    return completed;
}

void portal_render(State *state) {
    Portal *p = &state->portal;
    BeginShaderMode(p->shader);

    { // Resolution
        int loc = GetShaderLocation(p->shader, "resolution");
        Vector2 resolution = {state->game_width, state->game_height };
        void *val = &resolution;
        SetShaderValue(p->shader, loc, val, SHADER_UNIFORM_VEC2);
    }

    { // Time
        int loc = GetShaderLocation(p->shader, "time");
        float time = GetTime();
        void *val = &time;
        SetShaderValue(p->shader, loc, val, SHADER_UNIFORM_FLOAT);
    }

    { // Reverse
        int loc = GetShaderLocation(p->shader, "reverse");
        int reverse = has_flag(p->flags, PORTAL_BIT_REVERSED);
        void *val = &reverse;
        SetShaderValue(p->shader, loc, val, SHADER_UNIFORM_INT);
    }

    { // Disappearance
        int loc = GetShaderLocation(p->shader, "disappearance");
        float disappearance = p->smooth_disappearance;
        void *val = &disappearance;
        SetShaderValue(p->shader, loc, val, SHADER_UNIFORM_FLOAT);
    }

    Vector2 portal_radius = {
        .x = state->game_width * PORTAL_RADIUS,
        .y = state->game_height * PORTAL_RADIUS
    };
    Rectangle rec = {
        .x = state->game_center_x - portal_radius.x,
        .y = state->game_center_y - portal_radius.y,
        .width = portal_radius.x * 2,
        .height = portal_radius.y * 2,
    };
    Vector2 origin = { 0.5f, 0.5f };
    DrawRectanglePro(rec, origin, 0.0f, p->color);

    EndShaderMode();
}
