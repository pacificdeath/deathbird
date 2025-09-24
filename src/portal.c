// between 0.0f and 1.0f
static void update_smooth_disappearance(State *state) {
    float l = state->portal_linear_disappearance;
    state->portal_smooth_disappearance = 0.5f * sin((l * PI) - (PI * 0.5f)) + 0.5f;
}

static inline bool portal_has_appeared() {
    return state->portal_linear_disappearance <= 0.0f;
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

void portal_setup(State *state, Color color) {
    state->portal_color = color;
    state->portal_timer = 0.0f;
    state->portal_power = 0.0f;
}

void portal_appear(State *state) {
    state->portal_linear_disappearance -= PORTAL_APPEAR_SPEED * state->delta_time;
    if (state->portal_linear_disappearance < 0.0f) {
        state->portal_linear_disappearance = 0.0f;
    }
    update_smooth_disappearance(state);
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
    state->portal_power += state->delta_time;

    bool all_inhaled = true;
    if (state->player.state == PLAYER_STATE_INHALED_BY_PORTAL) {
        all_inhaled = false;
    }
    for (int i = 0; i < BIRD_CAPACITY; i++) {
        if (state->birds[i].state == BIRD_STATE_INHALED_BY_PORTAL) {
            all_inhaled = false;
        }
    }

    if (all_inhaled) {
        return portal_disappear(state);
    } else {
        portal_appear(state);
    }

    return false;
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
    bool all_exhaled = false;
    state->portal_timer -= state->delta_time;

    Bird *bird_to_exhale = NULL;
    bool exhale_player = false;

    if (state->portal_timer <= 0.0f) {
        for (int i = 0; i < BIRD_CAPACITY; i++) {
            if (state->birds[i].state == BIRD_STATE_INSIDE_PORTAL) {
                bird_to_exhale = &state->birds[i];
                goto portal_found_bird_to_exhale;
            }
        }
        if (state->player.state == PLAYER_STATE_INSIDE_PORTAL) {
            exhale_player = true;
            goto portal_found_bird_to_exhale;
        }
        all_exhaled = true;
    }

    portal_found_bird_to_exhale:

    if (bird_to_exhale != NULL || exhale_player) {
        if (!portal_has_appeared()) {
            portal_appear(state);
            return false;
        }
        state->portal_timer = PORTAL_EXHALE_RATE;
        if (bird_to_exhale != NULL) {
            bird_to_exhale->state = BIRD_STATE_EXHALED_BY_PORTAL;
        }
        if (exhale_player) {
            state->player.state = PLAYER_STATE_EXHALED_BY_PORTAL;
        }
        return false;
    }

    if (all_exhaled) {
        return portal_disappear(state);
    }
    return false;
}

void portal_render(State *state, int render_mode) {
    if (state->portal_linear_disappearance >= 1.0f) {
        return;
    }

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
        switch (render_mode) {
            default: ASSERT(false);
            case PORTAL_RENDER_MODE_INHALE: direction = 1.0f; break;
            case PORTAL_RENDER_MODE_EXHALE: direction = -1.0f; break;
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
