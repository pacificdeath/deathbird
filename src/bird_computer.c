#include <stdio.h>
#include "raylib.h"
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

static void draw_scroll_bar(
    State *state,
    Bird_Computer_Dimensions *dimensions,
    Bird_Computer_Text_Type type,
    int item_count,
    int item_area_offset
) {
    float full_size = dimensions->line_section_size - dimensions->y_fract;
    Rectangle rec = {
        .y = dimensions->header_size + (dimensions->y_fract / 2),
        .width = dimensions->x_fract / 2.0f,
        .height = full_size
    };
    switch (type) {
    case BIRD_COMPUTER_TEXT_TYPE_HEADER: break;
    case BIRD_COMPUTER_TEXT_TYPE_OPTION: {
        rec.x = state->game_center_x - (dimensions->x_fract);
    } break;
    case BIRD_COMPUTER_TEXT_TYPE_SUB_OPTION: {
        rec.x = state->game_right - (dimensions->x_fract);
    } break;
    }
    DrawRectangleLinesEx(rec, state->scale_multiplier, BIRD_COMPUTER_FG_COLOR);
    float scroll_bar_fract = full_size / item_count;
    rec.x += state->scale_multiplier;
    rec.y += (item_area_offset * scroll_bar_fract) + state->scale_multiplier;
    float scale_multiplier_x2 = state->scale_multiplier * 2;
    rec.width -= scale_multiplier_x2;
    rec.height = (BIRD_COMPUTER_LINE_COUNT * scroll_bar_fract) - scale_multiplier_x2;
    DrawRectangleRec(rec, BIRD_COMPUTER_FG_COLOR);
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
    Bird_Computer *bird_computer = &state->bird_computer;
    bird_computer->option_idx = 0;
    bird_computer->option_area_offset = 0;
    bird_computer->sub_option_idx = 0;
    bird_computer->sub_option_area_offset = 0;
    if (state->current_round == 0) {
        bird_computer->option_idx_results = -1;
        bird_computer->option_idx_continue = 0;
        bird_computer->option_idx_shop = -1;
        bird_computer->option_count = 1;
    } else {
        bird_computer->option_idx_results = 0;
        bird_computer->option_idx_continue = 1;
        bird_computer->option_idx_shop = 2;
        bird_computer->option_count = 3;
    }
}

void bird_computer_update(State *state) {
    Bird_Computer *bird_computer = &state->bird_computer;
    int *idx;
    int *area_offset;
    int *count;
    switch (bird_computer->state) {
    case BIRD_COMPUTER_STATE_OPTIONS: {
        idx = &bird_computer->option_idx;
        area_offset = &bird_computer->option_area_offset;
        count = &bird_computer->option_count;
    } break;
    case BIRD_COMPUTER_STATE_SUB_OPTIONS: {
        idx = &bird_computer->sub_option_idx;
        area_offset = &bird_computer->sub_option_area_offset;
        count = &bird_computer->sub_option_count;
    } break;
    }
    if (IsKeyPressed(KEY_DOWN) && *idx < (*count - 1)) {
        if (*idx < BIRD_COMPUTER_LINE_COUNT - 1) {
            (*idx)++;
        } else if (*area_offset < (*count - BIRD_COMPUTER_LINE_COUNT)) {
            (*area_offset)++;
        }
    }
    if (IsKeyPressed(KEY_UP)) {
        if (*idx > 0) {
            (*idx)--;
        } else if (*area_offset > 0) {
            (*area_offset)--;
        }
    }
    switch (bird_computer->state) {
    case BIRD_COMPUTER_STATE_OPTIONS: {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            int option_idx_with_offset = bird_computer->option_idx + bird_computer->option_area_offset;
            if (option_idx_with_offset == bird_computer->option_idx_results) {
                bird_computer->state = BIRD_COMPUTER_STATE_SUB_OPTIONS;
            } else if (option_idx_with_offset == bird_computer->option_idx_continue) {
                state->game_state = GAME_STATE_NEXT_LEVEL;
            }
        }
    } break;
    case BIRD_COMPUTER_STATE_SUB_OPTIONS: {
        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_BACKSPACE)) {
            bird_computer->sub_option_idx = 0;
            bird_computer->sub_option_area_offset = 0;
            bird_computer->state = BIRD_COMPUTER_STATE_OPTIONS;
        }
    } break;
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
        draw_scroll_bar(
            state,
            &dimensions,
            BIRD_COMPUTER_TEXT_TYPE_OPTION,
            bird_computer->option_count,
            bird_computer->option_area_offset
        );
        // scroll bar
    }
    if (bird_computer->sub_option_count > BIRD_COMPUTER_LINE_COUNT) {
        draw_scroll_bar(
            state,
            &dimensions,
            BIRD_COMPUTER_TEXT_TYPE_SUB_OPTION,
            bird_computer->sub_option_count,
            bird_computer->sub_option_area_offset
        );
        // scroll bar
    }

    // header
    draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_HEADER, "Welcome to the bird computer", 0);

    // options
    {
        for (int option_idx = bird_computer->option_area_offset; option_idx < bird_computer->option_count; option_idx++) {
            if (option_idx == bird_computer->option_idx_results) {
                switch (state->current_level_data.environment) {
                    case LEVEL_ENVIRONMENT_NONE: {
                        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Results from nothing", option_idx);
                    } break;
                    case LEVEL_ENVIRONMENT_FOREST: {
                        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Forest round results", option_idx);
                    } break;
                    case LEVEL_ENVIRONMENT_MEADOWS: {
                        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Meadows round results", option_idx);
                    } break;
                    case LEVEL_ENVIRONMENT_MOUNTAINS: {
                        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Mountains round results", option_idx);
                    } break;
                }

                if (option_idx == bird_computer->option_idx) {
                    // sub options
                    int sub_option_idx = -bird_computer->sub_option_area_offset;
                    char buffer[32];
                    sprintf(buffer, "Current round: %i", state->current_round);
                    draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_SUB_OPTION, buffer, sub_option_idx);
                    sub_option_idx++;
                    sprintf(buffer, "Round score: %i", state->player.level_score);
                    draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_SUB_OPTION, buffer, sub_option_idx);
                    sub_option_idx++;
                    int obliterated_birds_total = 0;
                    for (int i = 0; i < BIRD_TYPES_TOTAL; i++) {
                        obliterated_birds_total += state->player.obliterated_birds[i];
                    }
                    sprintf(buffer, "Birds obliterated: %i/%i", obliterated_birds_total, state->current_level_data.total_birds);
                    draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_SUB_OPTION, buffer, sub_option_idx);
                    sub_option_idx++;
                    int player_max_multiplier_idx = PLAYER_MULTIPLIER_MAX - PLAYER_SMALLEST_VALID_MULTIPLIER;
                    for (int i = 0; i < player_max_multiplier_idx - 1; i++) {
                        if (state->player.multiplier_amounts[i] == 0) {
                            continue;
                        }
                        int multiplier_amount = state->player.multiplier_amounts[i];
                        sprintf(buffer, "%ix-Multipliers: %i", i + PLAYER_SMALLEST_VALID_MULTIPLIER, multiplier_amount);
                        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_SUB_OPTION, buffer, sub_option_idx);
                        sub_option_idx++;
                    }
                    if (state->player.multiplier_amounts[player_max_multiplier_idx] > 0) {
                        sprintf(buffer, "Max-Multipliers: %i", state->player.multiplier_amounts[player_max_multiplier_idx]);
                        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_SUB_OPTION, buffer, sub_option_idx);
                        sub_option_idx++;
                    }
                    if (state->player.obliterated_birds[BIRD_TYPE_WHITE] > 0) {
                        sprintf(buffer, "White birds destroyed: %i", state->player.obliterated_birds[BIRD_TYPE_WHITE]);
                        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_SUB_OPTION, buffer, sub_option_idx);
                        sub_option_idx++;
                    }
                    if (state->player.obliterated_birds[BIRD_TYPE_GIANT] > 0) {
                        sprintf(buffer, "Giant birds destroyed: %i", state->player.obliterated_birds[BIRD_TYPE_GIANT]);
                        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_SUB_OPTION, buffer, sub_option_idx);
                        sub_option_idx++;
                    }
                    if (state->player.obliterated_birds[BIRD_TYPE_YELLOW] > 0) {
                        sprintf(buffer, "Yellow birds destroyed: %i", state->player.obliterated_birds[BIRD_TYPE_YELLOW]);
                        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_SUB_OPTION, buffer, sub_option_idx);
                        sub_option_idx++;
                    }
                    if (state->player.obliterated_birds[BIRD_TYPE_BROWN] > 0) {
                        sprintf(buffer, "Brown birds destroyed: %i", state->player.obliterated_birds[BIRD_TYPE_BROWN]);
                        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_SUB_OPTION, buffer, sub_option_idx);
                        sub_option_idx++;
                    }
                    for (int i = 0; i < 14; i++) {
                        sprintf(buffer, "death: %i", state->player.level_score);
                        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_SUB_OPTION, buffer, sub_option_idx);
                        sub_option_idx++;
                    }
                    bird_computer->sub_option_count = sub_option_idx;
                }
            } else if (option_idx == bird_computer->option_idx_continue) {
                switch (state->next_level_data.environment) {
                    case LEVEL_ENVIRONMENT_NONE: {
                        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Continue to nothing?", option_idx);
                    } break;
                    case LEVEL_ENVIRONMENT_FOREST: {
                        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Continue to Forest", option_idx);
                    } break;
                    case LEVEL_ENVIRONMENT_MEADOWS: {
                        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Continue to Meadows", option_idx);
                    } break;
                    case LEVEL_ENVIRONMENT_MOUNTAINS: {
                        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "Continue to Mountains", option_idx);
                    } break;
                }

                if (option_idx == bird_computer->option_idx) {
                    // sub options
                    char buffer[32];
                    Tex tex;
                    switch (state->next_level_data.environment) {
                    case LEVEL_ENVIRONMENT_NONE: {
                        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_SUB_OPTION, "Nothing", 4);
                    }
                    case LEVEL_ENVIRONMENT_FOREST: {
                        tex = TEX_ENV_NIGHT_SKY;
                        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_SUB_OPTION, "Forest", 4);
                    } break;
                    case LEVEL_ENVIRONMENT_MEADOWS: {
                        tex = TEX_ENV_DAY_SKY;
                        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_SUB_OPTION, "Meadows", 4);
                    } break;
                    case LEVEL_ENVIRONMENT_MOUNTAINS: {
                        tex = TEX_ENV_WINTER_SKY;
                        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_SUB_OPTION, "Mountains", 4);
                    } break;
                    }
                    Vector2 position = {
                        .x = state->game_center_x + (state->game_width / 4),
                        .y = state->game_top + dimensions.header_size + (dimensions.y_fract * 3.5)
                    };
                    for (int i = 0; i < state->next_level_data.scroll_env_amount; i++) {
                        tex_atlas_draw_raw(state, tex + i, position, 0, 0.6f);
                    }
                    sprintf(buffer, "Upcoming birds: %i", state->next_level_data.total_birds);
                    draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_SUB_OPTION, buffer, 5);
                    sprintf(buffer, "Bird frequency: %.2f/sec", state->next_level_data.bird_frequency);
                    draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_SUB_OPTION, buffer, 6);

                    bird_computer->sub_option_count = 0;
                }
            } else if (option_idx == bird_computer->option_idx_shop) {
                switch (state->current_level_data.environment) {
                    case LEVEL_ENVIRONMENT_NONE: {
                        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "The empty shop", option_idx);
                    } break;
                    case LEVEL_ENVIRONMENT_FOREST: {
                        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "The Forest shop", option_idx);
                    } break;
                    case LEVEL_ENVIRONMENT_MEADOWS: {
                        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "The Meadow shop", option_idx);
                    } break;
                    case LEVEL_ENVIRONMENT_MOUNTAINS: {
                        draw_text(state, &dimensions, BIRD_COMPUTER_TEXT_TYPE_OPTION, "The Mountain shop", option_idx);
                    } break;
                }
            }
        }
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
            + (dimensions.line_size * bird_computer->sub_option_idx)
            + (dimensions.line_size / 2);
    } break;
    }
    tex_atlas_draw_raw(state, TEX_BIRD_EYE, position, 0.0f, 1.0f);
}