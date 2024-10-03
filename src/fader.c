#include "raylib.h"
#include "main.h"

void fade_in(State *state) {
    state->fader.state = FADER_STATE_IN;
    state->fader.fade_level -= FADER_SPEED * state->delta_time;
    if (state->fader.fade_level <= FADER_MIN) {
        state->fader.fade_level = FADER_MIN;
        state->fader.state = FADER_STATE_IN_COMPLETE;
    }
}

void fade_out(State *state) {
    state->fader.state = FADER_STATE_OUT;
    state->fader.fade_level += FADER_SPEED * state->delta_time;
    if (state->fader.fade_level >= FADER_MAX) {
        state->fader.fade_level = FADER_MAX;
        state->fader.state = FADER_STATE_OUT_COMPLETE;
    }
}

void fader_render(State *state) {
    Color color = { 0, 0, 0, state->fader.fade_level };
    DrawRectangle(state->game_left, state->game_top, state->game_width, state->game_height, color);
}
