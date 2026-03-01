void ui_render() {
    Vector2 text_dimensions;

    text_dimensions = MeasureTextEx(state->font, "What", state->font_size, 0);

    float fuel_bar_width = (float)state->game_width * 0.4f;
    Rectangle fuel_bar_rec = {
        .x = (float)state->game_center_x - (fuel_bar_width * 0.5f),
        .y = (float)state->game_top,
        .width = fuel_bar_width,
        .height = text_dimensions.y,
    };
    const char *bar_text;

    {
        DrawRectangleRec(fuel_bar_rec, (Color){128,128,128,128});
        Rectangle fill_rec = {
            .x = fuel_bar_rec.x,
            .y = fuel_bar_rec.y,
            .height = fuel_bar_rec.height,
        };
        float fill_full;
        float fill_current;
        Color fill_color;
        bar_text = TextFormat("%i / %i (%s)", state->score, SCORE_TARGET, state->the_time);
        fill_full = SCORE_TARGET;
        fill_current = (float)state->score;
        fill_color = (Color){0,255,0,255};
        float fill = (fill_full > 0.0f)
            ? (100.0f / fill_full) * fill_current
            : 0.0f;
        fill_rec.width = (fuel_bar_rec.width / 100.0f) * fill;
        DrawRectangleRec(fill_rec, fill_color);
    }

    text_dimensions = MeasureTextEx(state->font, bar_text, state->font_size, 0);
    DrawTextPro(
        state->font,
        bar_text,
        (Vector2){
            fuel_bar_rec.x + (fuel_bar_rec.width/2) - (text_dimensions.x/2),
            fuel_bar_rec.y + (fuel_bar_rec.height/2) - (text_dimensions.y/2)
        },
        (Vector2){0},
        0,
        state->font_size,
        0,
        (Color){255,255,255,255}
    );
}

