#include "raylib.h"
#include "main.h"

#define FADER_MAX 255
#define FADER_MIN 0
#define FADER_SPEED 500.0f

void fade_in(State *state) {
    state->fader_state = FADER_STATE_IN;
    state->fader_level -= FADER_SPEED * state->delta_time;
    if (state->fader_level <= FADER_MIN) {
        state->fader_level = FADER_MIN;
        state->fader_state = FADER_STATE_IN_COMPLETE;
    }
}

void fade_out(State *state) {
    state->fader_state = FADER_STATE_OUT;
    state->fader_level += FADER_SPEED * state->delta_time;
    if (state->fader_level >= FADER_MAX) {
        state->fader_level = FADER_MAX;
        state->fader_state = FADER_STATE_OUT_COMPLETE;
    }
}

void fader_render(State *state) {
    Color color = { 0, 0, 0, state->fader_level };
    DrawRectangle(state->game_left, state->game_top, state->game_width, state->game_height, color);
}
