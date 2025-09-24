void game_over_text(State *state) {
    DrawRectangle(state->game_left, state->game_top, state->game_width, state->game_height, (Color){0,0,0,255});
    char *text = "You are a horrible person";
    Vector2 text_dimensions = MeasureTextEx(state->font, text, state->font_size, 0.0f);
    Vector2 origin = {0};
    Vector2 position = {
        .x = state->game_center_x - (text_dimensions.x / 2),
        .y = state->game_center_y - (text_dimensions.y / 2)
    };
    DrawTextPro(state->font, text, position, origin, 0.0f, state->font_size, 0.0f, (Color){255,255,255,255});
}
