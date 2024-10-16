#include "raylib.h"
#include "rlgl.h"
#include "main.h"

// between 0.0f and 1.0f
static void update_smooth_disappearance(Portal *p) {
    float l = p->linear_disappearance;
    p->smooth_disappearance = 0.5f * sin((l * PI) - (PI * 0.5f)) + 0.5f;
}

Vector2 portal_get_position(State *state) {
    return (Vector2){0};
}

// this returns a value between 0 and 1
// 0 means the position is exactly in the portal center
// 1 means the position is at the portal horizon or further away
float portal_distance_to_center_ratio(State *state, Vector2 position) {
    Vector2 portal_position = portal_get_position(state);
    float portal_distance = vec2_distance(portal_position, position);
    float portal_radius = PORTAL_RADIUS * GAME_HEIGHT_RATIO;
    if (portal_distance < portal_radius) {
        return portal_distance / portal_radius;
    }
    return 1.0f;
}

Portal_Bits portal_env_color(Level_Environment env) {
    switch (env) {
    case LEVEL_ENVIRONMENT_FOREST: {
        return PORTAL_BIT_GREEN;
    } break;
    case LEVEL_ENVIRONMENT_MEADOWS: {
        return PORTAL_BIT_YELLOW;
    } break;
    case LEVEL_ENVIRONMENT_MOUNTAINS: {
        return PORTAL_BIT_WHITE;
    } break;
    default: {
        return PORTAL_BIT_NONE;
    } break;
    }
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
    p->flags = flags;
    p->color.r = has_flag(flags, PORTAL_BIT_RED) * 255;
    p->color.g = has_flag(flags, PORTAL_BIT_GREEN) * 255;
    p->color.b = has_flag(flags, PORTAL_BIT_BLUE) * 255;
    p->color.a = 1.0f;
    p->timer = 0.0f;
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

bool portal_inhale(State *state) {
    if (state->player.state == PLAYER_STATE_INHALED_BY_PORTAL) {
        return false;
    }
    for (int i = 0; i < BIRD_CAPACITY; i++) {
        if (state->birds[i].state == BIRD_STATE_INHALED_BY_PORTAL) {
            return false;
        }
    }
    return true;
}

// move object towards portal center
// returns whether or not the object has reached the center
bool portal_inhale_object(float *obj_position, float obj_step, float portal_position) {
    bool x_at_portal = *obj_position == portal_position;

    if (x_at_portal) {
        return true;
    }

    float mov = *obj_position + obj_step;

    bool overstep_a = *obj_position < portal_position && mov > portal_position;
    bool overstep_b = *obj_position > portal_position && mov < portal_position;

    if (overstep_a || overstep_b) {
        *obj_position = portal_position;
        return true;
    } else {
        *obj_position = mov;
        return false;
    }
}

bool portal_exhale(State *state) {
    Portal *p = &state->portal;
    p->timer -= state->delta_time;
    if (p->timer <= 0.0f) {
        p->timer = PORTAL_EXHALE_RATE;
        bool all_objects_out = true;

        for (int i = 0; i < BIRD_CAPACITY; i++) {
            switch (state->birds[i].state) {
            case BIRD_STATE_INSIDE_PORTAL: {
                state->birds[i].state = BIRD_STATE_EXHALED_BY_PORTAL;
                return false;
            } break;
            case BIRD_STATE_EXHALED_BY_PORTAL: {
                all_objects_out = false;
            } break;
            default: break;
            }
        }
        switch (state->player.state) {
        case PLAYER_STATE_INSIDE_PORTAL: {
            state->player.state = PLAYER_STATE_EXHALED_BY_PORTAL;
            return false;
        } break;
        case PLAYER_STATE_EXHALED_BY_PORTAL: {
            all_objects_out = false;
        } break;
        default: break;
        }
        return all_objects_out;
    }
    return false;
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

    { // Direction
        int loc = GetShaderLocation(p->shader, "direction");
        float direction = 0.0f;
        if (has_flag(p->flags, PORTAL_BIT_INHALE)) {
            direction = 1.0f;
        } else if (has_flag(p->flags, PORTAL_BIT_EXHALE)) {
            direction = -1.0f;
        }
        void *val = &direction;
        SetShaderValue(p->shader, loc, val, SHADER_UNIFORM_FLOAT);
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
