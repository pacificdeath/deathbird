#define TERMINAL_LINE_START 4

inline static bool is_whitespace(char c) {
    return c == ' ' || c == '\0';
}

inline static int find_next_non_whitespace(char *string, int idx) {
    while (is_whitespace(string[idx])) {
        idx++;
    }
    if (idx >= (TERMINAL_LINE_MAX_LENGTH - 1)) {
        return -1;
    }
    return idx;
}

static void terminal_new_line(Terminal *terminal) {
    if (terminal->current_line == (TERMINAL_LINE_CAPACITY - 1)) {
        for (int i = 0; i < (TERMINAL_LINE_CAPACITY - 1); i++) {
            TextCopy(terminal->lines[i], terminal->lines[i + 1]);
        }
    } else {
        terminal->current_line++;
    }
    for (int i = 0; i < (TERMINAL_LINE_START - 1); i++) {
        terminal->lines[terminal->current_line][i] = '>';
    }
    terminal->lines[terminal->current_line][TERMINAL_LINE_START - 1] = ' ';
    terminal->lines[terminal->current_line][TERMINAL_LINE_START] = '\0';
    terminal->current_char = TERMINAL_LINE_START;
}

static void terminal_output_line(Terminal *terminal, char *text) {
    TextCopy(terminal->lines[terminal->current_line], text);
    terminal_new_line(terminal);
}

inline static void terminal_reset(Terminal *terminal) {
    terminal->current_line = -1;
    terminal_new_line(terminal);
}

void terminal_update_dimensions(State *state) {
    Terminal *terminal = &state->terminal;
    const int padding = state->game_width * 0.01f;
    terminal->rectangle = (Rectangle) {
        state->game_left + padding,
        state->game_top + padding,
        state->game_width - (padding * 2),
        state->game_height - (padding * 2),
    };
}

static Command *fuzzy_find_command(Terminal *terminal, int start_char) {
    char *text = terminal->lines[terminal->current_line];

    int closest_match = -1;
    int closest_match_score = 0;

    for (int i = 0; i < COMMAND_TOTAL; i++) {
        Command *c = &terminal->commands[i];
        int score = 0;
        int text_idx = start_char;
        int command_idx = 0;
        while (!is_whitespace(c->name[command_idx]) && text[text_idx] == c->name[command_idx]) {
            score++;
            text_idx++;
            command_idx++;
        }
        if (score > 0 && score > closest_match_score) {
            closest_match = i;
            closest_match_score = score;
        }
    }
    if (closest_match < 0) {
        return NULL;
    }

    return &terminal->commands[closest_match];
}

static Command *try_get_command(Terminal *terminal, char *text) {
    for (int i = 0; i < COMMAND_TOTAL; i++) {
        if (TextIsEqual(terminal->commands[i].name, text)) {
            return &terminal->commands[i];
        }
    }
    return NULL;
}

static void command_play(void *data) {
    State *state = (State *)data;
}
static void command_menu(void *data) {
    State *state = (State *)data;
    state->global_state = GLOBAL_STATE_MENU;
}
static void command_game(void *data) {
    State *state = (State *)data;
    state->global_state = GLOBAL_STATE_GAME;
}
static void command_toggle_collision_bounds(void *data) {
    State *state = (State *)data;
    if (has_flag(state->bird_flags, BIRD_FLAG_SHOW_COLLISION_BOUNDS)) {
        state->bird_flags &= ~BIRD_FLAG_SHOW_COLLISION_BOUNDS;
    } else {
        state->bird_flags |= BIRD_FLAG_SHOW_COLLISION_BOUNDS;
    }
}
static void command_bossbattle(void *data) {
    State *state = (State *)data;
    state->global_state = GLOBAL_STATE_BOSSBATTLE;
}

inline static void terminal_register_command(Terminal *terminal, int idx, char *name, void (*function)(void *)) {
    ASSERT(idx < COMMAND_TOTAL);
    terminal->commands[idx] = (Command) { idx, name, function };
}

void terminal_init(State *state) {
    Terminal *terminal = &state->terminal;
    terminal_update_dimensions(state);
    terminal_reset(terminal);
    terminal_register_command(terminal, COMMAND_PLAY, "play", command_play);
    terminal_register_command(terminal, COMMAND_MENU, "menu", command_menu);
    terminal_register_command(terminal, COMMAND_GAME, "game", command_game);
    terminal_register_command(terminal, COMMAND_BOSSBATTLE, "bossbattle", command_bossbattle);
    terminal_register_command(terminal, COMMAND_TOGGLECOLLISIONBOUNDS, "togglecollisionbounds", command_toggle_collision_bounds);
}

void terminal_setup(State *state) {
    Terminal *terminal = &state->terminal;
    TextCopy(terminal->lines[0], "The Bird Terminal 1999");
    TextCopy(terminal->lines[1], "Sloppyright (S)");
    terminal->lines[2][0] = '\0';
    TextCopy(terminal->lines[3], "Deprecated version, install Bird Terminal 2005");
    terminal->lines[4][0] = '\0';
    terminal->current_line = 4;
    terminal_new_line(terminal);
}

inline static void terminal_add_char(Terminal *terminal, char c) {
    if (terminal->current_char >= (TERMINAL_LINE_MAX_LENGTH - 1)) {
        return;
    }

    terminal->lines[terminal->current_line][terminal->current_char] = c;
    terminal->lines[terminal->current_line][terminal->current_char + 1] = '\0';
    terminal->current_char++;
}

bool terminal_update(State *state) {
    Terminal *terminal = &state->terminal;

    switch (state->global_state) {
        case GLOBAL_STATE_TERMINAL: {
            char *line = terminal->lines[terminal->current_line];
            for (int key = KEY_A; key <= KEY_Z; key++) {
                if (IsKeyPressed(key)) {
                    char c = key + 32;
                    terminal_add_char(terminal, c);
                }
            }
            for (int key = KEY_ZERO; key <= KEY_NINE; key++) {
                if (IsKeyPressed(key)) {
                    terminal_add_char(terminal, key);
                }
            }
            if (IsKeyPressed(KEY_SPACE)) {
                terminal_add_char(terminal, ' ');
            }
            if (IsKeyPressed(KEY_BACKSPACE) && terminal->current_char > TERMINAL_LINE_START) {
                terminal->current_char--;
                line[terminal->current_char] = '\0';
            }

            int command_start_idx = terminal->current_char;
            while (command_start_idx > TERMINAL_LINE_START && !is_whitespace(line[command_start_idx - 1])) {
                command_start_idx--;
            }

            terminal->fuzzy[0] = '\0';

            if (terminal->current_char > TERMINAL_LINE_START) {
                Command *fuzzy_command = fuzzy_find_command(terminal, command_start_idx);
                if (fuzzy_command != NULL) {
                    for (int i = 0; i <= terminal->current_char; i++) {
                        terminal->fuzzy[i] = ' ';
                    }

                    int fuzzy_idx = terminal->current_char;
                    int fuzzy_text_length = TextLength(fuzzy_command->name);

                    int command_offset = terminal->current_char - command_start_idx;
                    int command_idx = fuzzy_idx - terminal->current_char + command_offset;

                    while (!(is_whitespace(fuzzy_command->name[command_idx])) && command_idx < fuzzy_text_length) {
                        terminal->fuzzy[fuzzy_idx] = fuzzy_command->name[fuzzy_idx - terminal->current_char + command_offset];
                        fuzzy_idx++;
                        command_idx++;
                    }
                    terminal->fuzzy[fuzzy_idx] = '\0';
                }
            }

            if (IsKeyPressed(KEY_ENTER)) {
                terminal_new_line(terminal);
                char buffer[TERMINAL_LINE_MAX_LENGTH];
                int command_1_offset = find_next_non_whitespace(line, TERMINAL_LINE_START);
                int buffer_idx = 0;
                int i;
                for (i = 0; !is_whitespace(line[command_1_offset + i]); i++) {
                    buffer[i] = line[command_1_offset + i];
                }
                buffer[i] = '\0';
                Command *command = try_get_command(terminal, buffer);
                if (command != NULL) {
                    command->function(state);
                } else {
                    TraceLog(LOG_ERROR, "You are a horrible person");
                }
            }

            if (IsKeyPressed(KEY_TAB)) {
                int i;
                for (i = terminal->current_char; !is_whitespace(terminal->fuzzy[i]); i++) {
                    line[i] = terminal->fuzzy[i];
                }
                line[i] = '\0';
                terminal->current_char = i;
            }

            break;
        }
    };
    return false;
}

inline static void terminal_render_line(State *state, char *line, int line_idx, int line_size, int char_size, Color color) {
    Terminal *terminal = &state->terminal;
    Vector2 position = {
        0,
        terminal->rectangle.y + line_idx * line_size
    };
    for (int i = 0; line[i] != '\0'; i++) {
        position.x = terminal->rectangle.x + (i * char_size);
        DrawTextCodepoint(
            state->menu.font,
            line[i],
            position,
            line_size,
            color
        );
    }
}

void terminal_render(State *state) {
    Terminal *terminal = &state->terminal;
    DrawRectangleRec(terminal->rectangle, TERMINAL_BG_COLOR);
    const int line_size = terminal->rectangle.height / TERMINAL_LINE_CAPACITY;
    const int char_size = terminal->rectangle.width / TERMINAL_LINE_MAX_LENGTH;
    for (int i = 0; i <= terminal->current_line; i++) {
        terminal_render_line(
            state,
            terminal->lines[i],
            i,
            line_size,
            char_size,
            TERMINAL_FG_COLOR
        );
    }
    terminal_render_line(
        state,
        terminal->fuzzy,
        terminal->current_line,
        line_size,
        char_size,
        TERMINAL_FUZZY_COLOR
    );

    float cursor_y = line_size * terminal->current_line;
    Vector2 cursor_top = {
        terminal->rectangle.x + char_size * terminal->current_char,
        terminal->rectangle.y + cursor_y
    };
    Vector2 cursor_bottom = {
        cursor_top.x,
        terminal->rectangle.y + cursor_y + line_size
    };
    DrawLineEx(cursor_top, cursor_bottom, state->scale_multiplier, RED);
}

