#define FADER_MAX 255.0f
#define FADER_MIN 0.0f
#define FADER_SPEED 100.0f

bool fade_in(State *state) {
    state->fader_level -= FADER_SPEED * state->delta_time;
    if (state->fader_level <= FADER_MIN) {
        state->fader_level = FADER_MIN;
        return true;
    }
    return false;
}

bool fade_out(State *state) {
    state->fader_level += FADER_SPEED * state->delta_time;
    if (state->fader_level >= FADER_MAX) {
        state->fader_level = FADER_MAX;
        return true;
    }
    return false;
}

void fader_render(State *state) {
    Color color = { 0, 0, 0, state->fader_level };
    DrawRectangle(state->game_left, state->game_top, state->game_width, state->game_height, color);
}
