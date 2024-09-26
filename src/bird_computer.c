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
    if (line_idx < 0 || line_idx >= BIRD_COMPUTER_LINE_COUNT) {
        return;
    }
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
    bird_computer->option_count = 7;
}

void bird_computer_cleanup(State *state) {
    UnloadFont(state->bird_computer.font);
}

void bird_computer_level_setup(State *state) {
    state->bird_computer.option_idx = 0;
    state->bird_computer.option_area_offset = 0;
    state->bird_computer.sub_option_idx = 0;
    state->bird_computer.sub_option_area_offset = 0;
}

void bird_computer_update(State *state) {
    Bird_Computer *bird_computer = &state->bird_computer;
    if (IsKeyPressed(KEY_DOWN)) {
        bird_computer->sub_option_idx = 0;
        if (bird_computer->option_idx < BIRD_COMPUTER_LINE_COUNT - 1) {
            bird_computer->option_idx++;
        } else if (bird_computer->option_area_offset < bird_computer->option_count - BIRD_COMPUTER_LINE_COUNT) {
            bird_computer->option_area_offset++;
        }
    }
    if (IsKeyPressed(KEY_UP)) {
        bird_computer->sub_option_idx = 0;
        if (bird_computer->option_idx > 0) {
            bird_computer->option_idx--;
        } else if (bird_computer->option_area_offset > 0) {
            bird_computer->option_area_offset--;
        }
    }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        switch (bird_computer->option_idx) {
        case BIRD_COMPUTER_OPTION_RESULTS: {
            bird_computer->option_idx = -1;
        } break;
        case BIRD_COMPUTER_OPTION_CONTINUE: {
            state->game_state = GAME_STATE_NEXT_LEVEL;
        } break;
        }
    }
}

void bird_computer_render(State *state) {
    Bird_Computer *bird_computer = &state->bird_computer;
    const float y_fract = state->game_height / 16;
    const float line_section_size = y_fract * 11;
    Bird_Computer_Dimensions dimensions = {
        .font_size = (BIRD_COMPUTER_FONT_SIZE / bird_computer->font.baseSize) * state->scale_multiplier,
        .x_fract = state->game_width / 32,
        .y_fract = y_fract,
        .header_size = y_fract * 2,
        .line_section_size = line_section_size,
        .line_size = line_section_size / BIRD_COMPUTER_LINE_COUNT,
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

    if (bird_computer->option_count > BIRD_COMPUTER_LINE_COUNT) {
        // scroll bar
        float full_size = dimensions.line_section_size - dimensions.y_fract;
        Rectangle rec = {
            .x = state->game_center_x - (dimensions.x_fract),
            .y = dimensions.header_size + (y_fract / 2),
            .width = dimensions.x_fract / 2.0f,
            .height = full_size
        };
        DrawRectangleLinesEx(rec, state->scale_multiplier, BIRD_COMPUTER_FG_COLOR);
        float scroll_bar_fract = full_size / bird_computer->option_count;
        rec.x += state->scale_multiplier;
        rec.y += (bird_computer->option_area_offset * scroll_bar_fract) + state->scale_multiplier;
        float scale_multiplier_x2 = state->scale_multiplier * 2;
        rec.width -= scale_multiplier_x2;
        rec.height = (BIRD_COMPUTER_LINE_COUNT * scroll_bar_fract) - scale_multiplier_x2;
        DrawRectangleRec(rec, BIRD_COMPUTER_FG_COLOR);
    }

    // header
    draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_HEADER, "Welcome to the bird computer", 0);

    // options
    {
        int option_idx = -bird_computer->option_area_offset;
        switch (state->game_level)
        {
        case GAME_LEVEL_FOREST: {
            draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Death forest results", option_idx);
            option_idx++;
            draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Continue to Death field", option_idx);
        } break;
        case GAME_LEVEL_FIELD: {
            draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Death field results", option_idx);
            option_idx++;
            draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Continue to Death mountain", option_idx);
        } break;
        case GAME_LEVEL_SNOW: {
            draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Death mountain results", option_idx);
            option_idx++;
            draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Continue to Death forest", option_idx);
        } break;
        }
        option_idx++;
        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Nothing", option_idx);
        option_idx++;
        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Nothing", option_idx);
        option_idx++;
        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Nothing", option_idx);
        option_idx++;
        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Nothing", option_idx);
        option_idx++;
        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Nothing", option_idx);
        option_idx++;
        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Nothing", option_idx);
        option_idx++;
        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Nothing", option_idx);
        option_idx++;
        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Nothing", option_idx);
        option_idx++;
        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Nothing", option_idx);
        option_idx++;
        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Finale!", option_idx);
        bird_computer->option_count = option_idx + 1 + bird_computer->option_area_offset;
    }

    // sub options
    char buffer[32];
    int line_idx = 0;
    switch (bird_computer->option_idx + bird_computer->option_area_offset) {
    case BIRD_COMPUTER_OPTION_RESULTS: {
        sprintf(buffer, "Level score: %i", state->player.level_score);
        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_SUB_OPTION, buffer, line_idx);
        line_idx++;
        sprintf(buffer, "Birds obliterated: %i/%i", state->player.obliterated_birds, state->level_bird_amount);
        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_SUB_OPTION, buffer, line_idx);
        int player_max_multiplier_idx = PLAYER_MULTIPLIER_MAX - 1;
        for (int i = 0; i < player_max_multiplier_idx - 1; i++) {
            if (state->player.multiplier_amounts[i] == 0) {
                continue;
            }
            line_idx++;
            sprintf(buffer, "%ix Multipliers: %i", i + PLAYER_SMALLEST_VALID_MULTIPLIER, state->player.multiplier_amounts[i]);
            draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_SUB_OPTION, buffer, line_idx);
        }
        if (state->player.multiplier_amounts[player_max_multiplier_idx] > 0) {
            line_idx++;
            sprintf(buffer, "Obliterations: %i", state->player.multiplier_amounts[player_max_multiplier_idx]);
            draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_SUB_OPTION, buffer, line_idx);
        }
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
    Vector2 position;
    switch (bird_computer->state) {
    case BIRD_COMPUTER_STATE_OPTIONS: {
        position.x = state->game_left + dimensions.x_fract;
        position.y = state->game_top
            + dimensions.header_size
            + (dimensions.line_size * bird_computer->option_idx)
            + (dimensions.line_size / 2);
    } break;
    case BIRD_COMPUTER_STATE_SUB_OPTIONS: {
        position.x = state->game_center_x + dimensions.x_fract;
        position.y = state->game_top
            + dimensions.header_size
            + (dimensions.line_size * bird_computer->option_idx)
            + (dimensions.line_size / 2);
    } break;
    }
    tex_atlas_draw_raw(state, TEX_BIRD_EYE, position, 0.0f);
}