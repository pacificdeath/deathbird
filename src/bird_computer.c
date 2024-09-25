#include "raylib.h"
#include "stdlib.h"
#include "main.h"

static void populate_option(Bird_Computer_Option *dst, char *src, int length) {
    dst->name_len = length;
    for (int i = 0; i < length; i++) {
        dst->name[i] = src[i];
    }
    dst->name[length] = '\0';
}

void bird_computer_init(Bird_Computer *bird_computer) {
    bird_computer->font = LoadFont(BIRD_COMPUTER_FONT);
    populate_option(&(bird_computer->options[0]), "Continue to the next level", 26);
    populate_option(&(bird_computer->options[1]), "Placeholder", 11);
    populate_option(&(bird_computer->options[2]), "Placeholder", 11);
    populate_option(&(bird_computer->options[3]), "Placeholder", 11);
    populate_option(&(bird_computer->options[4]), "Placeholder", 11);
    populate_option(&(bird_computer->options[5]), "Placeholder", 11);
    populate_option(&(bird_computer->options[6]), "Placeholder", 11);
    bird_computer->option_line_count = 7;
}

void bird_computer_current_state_setup(State *state) {
    switch (state->game_level) {
    case GAME_LEVEL_NONE: {

    } break;
    case GAME_LEVEL_FOREST: {

    } break;
    case GAME_LEVEL_FIELD: {

    } break;
    case GAME_LEVEL_SNOW: {

    } break;
    }
}

void bird_computer_update(State *state, Bird_Computer *bird_computer) {
    if (IsKeyPressed(KEY_DOWN)) {
        if (bird_computer->option_line < (BIRD_COMPUTER_OPTIONS - 1)) {
            bird_computer->option_line++;
        }
    }
    if (IsKeyPressed(KEY_UP)) {
        if (bird_computer->option_line > 0) {
            bird_computer->option_line--;
        }
    }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        switch (bird_computer->option_line) {
        case 0: {
            state->game_state = GAME_STATE_NEXT_LEVEL;
        } break;
        }
    }
}

void bird_computer_render(State *state, Bird_Computer *bird_computer) {
    const float font_size = BIRD_COMPUTER_FONT_SIZE * state->scale_multiplier;
    const Vector2 text_dimensions = MeasureTextEx(bird_computer->font, "Welcome to the bird computer", font_size, 0.0f);
    const float x_fract = state->game_width / 32;
    const float y_fract = state->game_height / 16;
    const float header_size = y_fract * 1.0f;
    const float options_size = y_fract * 11;
    const float footer_offset = y_fract * 4;
    {
        DrawRectangle(state->game_left, state->game_top, state->game_width, state->game_height, BIRD_COMPUTER_BG_COLOR);
        Rectangle rec = {
            .x = state->game_left,
            .y = header_size,
            .width = state->game_width / 2,
            .height = options_size
        };
        DrawRectangleLinesEx(rec, state->scale_multiplier, BIRD_COMPUTER_FG_COLOR);
        rec.x = state->game_left + (state->game_width / 2);
        DrawRectangleLinesEx(rec, state->scale_multiplier, BIRD_COMPUTER_FG_COLOR);
    }
    {
        // header
        Vector2 position = {
            state->game_left + (state->game_width - text_dimensions.x) / 2,
            state->game_top
        };
        Vector2 origin = { 0, 0 };
        DrawTextPro(bird_computer->font, "Welcome to the bird computer", position, origin, 0.0f, font_size, 0, BIRD_COMPUTER_TEXT_COLOR);
    }
    {
        float option_fract = options_size / BIRD_COMPUTER_OPTIONS;
        Vector2 origin = { 0, -(option_fract / 2.0f) + (text_dimensions.y / 2.0f) };
        for (int i = 0; i < BIRD_COMPUTER_OPTIONS; i++) {
            // options
            Vector2 position = {
                state->game_left + (x_fract * 2),
                state->game_top
                    + header_size
                    + (option_fract * i)
            };
            DrawTextPro(bird_computer->font, bird_computer->options[i].name, position, origin, 0.0f, font_size, 0.0f, BIRD_COMPUTER_TEXT_COLOR);
        }
        // cursor
        float fract = options_size / BIRD_COMPUTER_OPTIONS;
        Vector2 position = {
            state->game_left + x_fract,
            state->game_top
                + header_size
                + (option_fract * bird_computer->option_line)
                + (option_fract / 2)
        };
        tex_atlas_draw_raw(state, TEX_BIRD_EYE, position, 0.0f);
    }
}