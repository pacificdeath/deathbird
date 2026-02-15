#define CARTOON_TRANSITION_SPEED 2.0f
#define CARTOON_TRANSITION_FULL_TRANSPARENCY 2.0f
#define CARTOON_TRANSITION_FULL_DARKNESS 0.0f

void cartoon_transition_init() {
    state->cartoon_transition_shader = LoadShader(NULL, "./src/shaders/cartoon_transition.glsl");
    state->cartoon_transition_shader_location_resolution = GetShaderLocation(
        state->cartoon_transition_shader,
        "resolution"
    );
    state->cartoon_transition_shader_location_size = GetShaderLocation(
        state->cartoon_transition_shader,
        "size"
    );
    state->cartoon_transition_size = 0.0f;
}

inline static bool cartoon_transition_is_full_transparency() {
    return state->cartoon_transition_size >= CARTOON_TRANSITION_FULL_TRANSPARENCY;
}

inline static bool cartoon_transition_is_full_darkness() {
    return state->cartoon_transition_size <= CARTOON_TRANSITION_FULL_DARKNESS;
}

void cartoon_transition_update() {
    switch (state->global_state) {
        case GLOBAL_STATE_DEFAULT:
        case GLOBAL_STATE_GAME:
            if (cartoon_transition_is_full_transparency()) {
                state->cartoon_transition_size = CARTOON_TRANSITION_FULL_TRANSPARENCY;
                return;
            }

            state->cartoon_transition_size += state->delta_time * CARTOON_TRANSITION_SPEED;
            break;
        case GLOBAL_STATE_TRANSITION:
            if (cartoon_transition_is_full_darkness()) {
                state->cartoon_transition_size = CARTOON_TRANSITION_FULL_DARKNESS;
                return;
            }

            state->cartoon_transition_size -= state->delta_time * CARTOON_TRANSITION_SPEED;
            break;
    }
    SetShaderValue(
        state->cartoon_transition_shader,
        state->cartoon_transition_shader_location_size,
        &(state->cartoon_transition_size),
        SHADER_UNIFORM_FLOAT
    );

    Vector2 resolution = {
        state->game_width,
        state->game_height
    };
    SetShaderValue(
        state->cartoon_transition_shader,
        state->cartoon_transition_shader_location_resolution,
        &resolution,
        SHADER_UNIFORM_VEC2
    );
}

void cartoon_transition_render() {
    if (cartoon_transition_is_full_transparency()) {
        return;
    }

    BeginShaderMode(state->cartoon_transition_shader);
    DrawRectangle(
        state->game_left,
        state->game_top,
        state->game_width,
        state->game_height,
        WHITE
    );
    EndShaderMode();
}

void cartoon_transition_set_fully_transparent() {
    state->cartoon_transition_size = CARTOON_TRANSITION_FULL_TRANSPARENCY;
}

