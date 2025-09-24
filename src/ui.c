void ui_render() {
    Vector2 position = { .x = state->game_left, .y = state->game_top };
    Vector2 text_dimensions;

    {
        const char *buffer = TextFormat(" Level: %i ", state->level_idx);
        text_dimensions = MeasureTextEx(state->font, buffer, state->font_size, 0);
        DrawRectangleV(position, text_dimensions, LEVEL_IDX_TEXT_BG_COLOR);
        DrawTextPro(state->font, buffer, position, (Vector2) { 0, 0 }, 0, state->font_size, 0, LEVEL_IDX_TEXT_COLOR);
    }

    if (!player_has_infinite_lives()) {
        Rectangle lives_rec = {
            .y = position.y + text_dimensions.y,
            .width = text_dimensions.x / DEFAULT_LIVES,
            .height = text_dimensions.y,
        };
        for (int i = 0; i < state->player.lives; i++) {
            lives_rec.x = position.x + (lives_rec.width * i);
            Color color;
            switch (i) {
                case 0: color = (Color){255,0,0,255}; break;
                case 1: color = (Color){255,128,0,255}; break;
                case 2: color = (Color){255,255,0,255}; break;
                default: color = (Color){0,255,0,255}; break;
            }
            DrawRectangleRec(lives_rec, color);
            DrawRectangleLinesEx(lives_rec, text_dimensions.y / 8, (Color){0,0,0,255});
        }
    }

    float fuel_bar_width = state->game_width * 0.4f;
    Rectangle fuel_bar_rec = {
        .x = state->game_center_x - (fuel_bar_width * 0.5f),
        .y = state->game_top,
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
        if (bird_boss_exists()) {
            bar_text = state->bosses_name;
            fill_full = 0.0f;
            fill_current = 0.0f;
            for (int i = 0; i < BOSS_CAPACITY; i++) {
                if (state->bosses[i].bird == NULL) {
                    continue;
                }
                fill_full += state->bosses[i].full_health;
                if (state->bosses[i].bird->state == BIRD_STATE_DEAD) {
                    continue;
                }
                fill_current += state->bosses[i].bird->alive.health;
            }
            fill_color = (Color){255,0,0,255};
        } else {
            bar_text = TextFormat("%i / %i", state->portal_fuel, state->current_level.required_fuel);
            fill_full = state->current_level.required_fuel;
            fill_current = state->portal_fuel;
            fill_color = (Color){0,255,0,255};
        }
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
        MULTIPLIER_TEXT_COLOR
    );

    if (state->bird_multiplier_timer > 0.0f) {
        const char *string = TextFormat("%ix-Multiplier ", state->bird_multiplier_display);
        Vector2 multiplier_text_dimensions = MeasureTextEx(state->font, string, state->font_size, 0);
        Vector2 multiplier_text_position = {
            .x = state->game_right - multiplier_text_dimensions.x,
            .y = state->game_top
        };
        DrawRectangleV(multiplier_text_position, multiplier_text_dimensions, MULTIPLIER_TEXT_BG_COLOR);
        DrawTextPro(state->font, string, multiplier_text_position, (Vector2){0}, 0, state->font_size, 0, MULTIPLIER_TEXT_COLOR);
    }
}

