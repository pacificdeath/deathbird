#include "raylib.h"
#include "stdio.h"
#include "main.h"

static void draw_text(
    State *state,
    Bird_Computer_Dimensions *dimensions,
    Bird_Computer_Text_Type text_type,
    char *text,
    int line_idx
) {
    const Vector2 text_dimensions = MeasureTextEx(state->bird_computer.font, text, dimensions->font_size, 0.0f);
    Vector2 text_origin;
    Vector2 position;
    switch (text_type) {
    case BIRD_COMPUTER_TEXT_TYPE_HEADER: {
        text_origin = (Vector2){ 0, -(dimensions->header_size / 2.0f) + (text_dimensions.y / 2.0f) };
        position.x = state->game_left + (state->game_width - text_dimensions.x) / 2;
        position.y = state->game_top;
    } break;
    case BIRD_COMPUTER_TEXT_TYPE_OPTION:
    case BIRD_COMPUTER_TEXT_TYPE_SUB_OPTION: {
        text_origin = (Vector2){ 0, -(dimensions->line_size / 2.0f) + (text_dimensions.y / 2.0f) };
        position.y = state->game_top + dimensions->header_size + (dimensions->line_size * line_idx);
        if (text_type == BIRD_COMPUTER_TEXT_TYPE_OPTION) {
            position.x = state->game_left + (dimensions->x_fract * 2);
        } else {
            position.x = state->game_center_x + (dimensions->x_fract * 2);
        }
    } break;
    }
    DrawTextPro(
        state->bird_computer.font,
        text,
        position,
        text_origin,
        0.0f,
        dimensions->font_size,
        0.0f,
        BIRD_COMPUTER_TEXT_COLOR
    );
}

void bird_computer_init(State *state) {
    Bird_Computer *bird_computer = &state->bird_computer;
    bird_computer->font = LoadFont(BIRD_COMPUTER_FONT);
    bird_computer->option_line_count = 7;
}

void bird_computer_cleanup(State *state) {
    UnloadFont(state->bird_computer.font);
}

void bird_computer_update(State *state) {
    Bird_Computer *bird_computer = &state->bird_computer;
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
        case BIRD_COMPUTER_OPTION_CONTINUE: {
            state->bird_computer.option_line = 0;
            state->game_state = GAME_STATE_NEXT_LEVEL;
        } break;
        }
    }
}

void bird_computer_render(State *state) {
    Bird_Computer *bc = &state->bird_computer;
    const float y_fract = state->game_height / 16;
    const float line_section_size = y_fract * 11;
    Bird_Computer_Dimensions dimensions = {
        .font_size = (BIRD_COMPUTER_FONT_SIZE / bc->font.baseSize) * state->scale_multiplier,
        .x_fract = state->game_width / 32,
        .y_fract = y_fract,
        .header_size = y_fract * 2,
        .line_section_size = line_section_size,
        .line_size = line_section_size / BIRD_COMPUTER_OPTIONS,
        .footer_size = y_fract * 3,
    };

    // background
    DrawRectangle(state->game_left, state->game_top, state->game_width, state->game_height, BIRD_COMPUTER_BG_COLOR);
    Rectangle rec = {
        .x = state->game_left,
        .y = dimensions.header_size,
        .width = state->game_width / 2,
        .height = dimensions.line_section_size
    };
    DrawRectangleLinesEx(rec, state->scale_multiplier, BIRD_COMPUTER_FG_COLOR);
    rec.x = state->game_left + (state->game_width / 2);
    DrawRectangleLinesEx(rec, state->scale_multiplier, BIRD_COMPUTER_FG_COLOR);

    // header
    draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_HEADER, "Welcome to the bird computer", 0);

    // options
    int line_idx = 0;
    switch (state->game_level)
    {
    case GAME_LEVEL_FOREST: {
        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Death forest results", 0);
        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Continue to Death field", 1);
    } break;
    case GAME_LEVEL_FIELD: {
        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Death field results", 0);
        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Continue to Death mountain", 1);
    } break;
    case GAME_LEVEL_SNOW: {
        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Death mountain results", 0);
        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Continue to Death forest", 1);
    } break;
    }
    draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Nothing", 2);
    draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Nothing", 3);
    draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Nothing", 4);
    draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Nothing", 5);
    draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Nothing", 6);

    // sub options
    switch (bc->option_line) {
    case 0: {
        char buffer[32];
        sprintf(buffer, "Birds obliterated: %i/%i", state->player.obliterated_birds, state->level_bird_amount);
        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_SUB_OPTION, buffer, 0);
    } break;
    case 1: {

    } break;
    case 2: {

    } break;
    case 3: {

    } break;
    case 4: {

    } break;
    case 5: {

    } break;
    case 6: {

    } break;
    }

    // cursor
    Vector2 position = {
        state->game_left + dimensions.x_fract,
        state->game_top
            + dimensions.header_size
            + (dimensions.line_size * bc->option_line)
            + (dimensions.line_size / 2)
    };
    tex_atlas_draw_raw(state, TEX_BIRD_EYE, position, 0.0f);
}