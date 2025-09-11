#include "rlgl.h"

// between 0.0f and 1.0f
static void update_smooth_disappearance(State *state) {
    float l = state->portal_linear_disappearance;
    state->portal_smooth_disappearance = 0.5f * sin((l * PI) - (PI * 0.5f)) + 0.5f;
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

PortalBits portal_area_color(Area area) {
    switch (area) {
    case AREA_FOREST: {
        return PORTAL_BIT_GREEN;
    } break;
    case AREA_MEADOWS: {
        return PORTAL_BIT_YELLOW;
    } break;
    case AREA_MOUNTAINS: {
        return (PORTAL_BIT_GREEN | PORTAL_BIT_BLUE);
    } break;
    case AREA_INDUSTRIAL: {
        return PORTAL_BIT_RED;
    } break;
    case AREA_CASTLE: {
        return PORTAL_BIT_BLACK;
    } break;
    default: {
        return PORTAL_BIT_NONE;
    } break;
    }
}

void portal_init(State *state) {
    state->portal_linear_disappearance = 1.0f;
    state->portal_smooth_disappearance = 1.0f;
    Image image = GenImageColor(1, 1, WHITE);
    state->portal_texture = LoadTextureFromImage(image);
    SetShapesTexture(state->portal_texture, (Rectangle){ 0.0f, 0.0f, 1.0f, 1.0f });
    state->portal_shader = LoadShader((void *)0, "./src/shaders/portal.glsl");
}

void portal_cleanup(State *state) {
    UnloadTexture(state->portal_texture);
    UnloadShader(state->portal_shader);
}

void portal_setup(State *state, PortalBits bits) {
    state->portal_bits = bits;
    state->portal_color.r = has_flag(bits, PORTAL_BIT_RED) * 255;
    state->portal_color.g = has_flag(bits, PORTAL_BIT_GREEN) * 255;
    state->portal_color.b = has_flag(bits, PORTAL_BIT_BLUE) * 255;
    state->portal_color.a = 1.0f;
    state->portal_timer = 0.0f;
}

bool portal_appear(State *state) {
    bool completed = false;
    state->portal_linear_disappearance -= PORTAL_APPEAR_SPEED * state->delta_time;
    if (state->portal_linear_disappearance < 0.0f) {
        state->portal_linear_disappearance = 0.0f;
        completed = true;
    }
    update_smooth_disappearance(state);
    return completed;
}

bool portal_disappear(State *state) {
    bool completed = false;
    state->portal_linear_disappearance += PORTAL_DISAPPEAR_SPEED * state->delta_time;
    if (state->portal_linear_disappearance > 1.0f) {
        state->portal_linear_disappearance = 1.0f;
        completed = true;
    }
    update_smooth_disappearance(state);
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
    state->portal_timer -= state->delta_time;
    if (state->portal_timer <= 0.0f) {
        state->portal_timer = PORTAL_EXHALE_RATE;
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
    BeginShaderMode(state->portal_shader);

    { // Resolution
        int loc = GetShaderLocation(state->portal_shader, "resolution");
        Vector2 resolution = {state->game_width, state->game_height };
        void *val = &resolution;
        SetShaderValue(state->portal_shader, loc, val, SHADER_UNIFORM_VEC2);
    }

    { // Time
        int loc = GetShaderLocation(state->portal_shader, "time");
        float time = GetTime();
        void *val = &time;
        SetShaderValue(state->portal_shader, loc, val, SHADER_UNIFORM_FLOAT);
    }

    { // Direction
        int loc = GetShaderLocation(state->portal_shader, "direction");
        float direction = 0.0f;
        if (has_flag(state->portal_bits, PORTAL_BIT_INHALE)) {
            direction = 1.0f;
        } else if (has_flag(state->portal_bits, PORTAL_BIT_EXHALE)) {
            direction = -1.0f;
        }
        void *val = &direction;
        SetShaderValue(state->portal_shader, loc, val, SHADER_UNIFORM_FLOAT);
    }

    { // Disappearance
        int loc = GetShaderLocation(state->portal_shader, "disappearance");
        float disappearance = state->portal_smooth_disappearance;
        void *val = &disappearance;
        SetShaderValue(state->portal_shader, loc, val, SHADER_UNIFORM_FLOAT);
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
    DrawRectanglePro(rec, origin, 0.0f, state->portal_color);

    EndShaderMode();
}
