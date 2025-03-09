#include "../raylib-5.0_win64_mingw-w64/include/raylib.h"
#include "main.h"

#define TERMINAL_LINE_START 4

char *terminal_get_menu_startup_text(Terminal *terminal) {
    switch (terminal->current_line) {
        case 0: return ">>> initializing bird computer...";
        case 1: return ">>> scanning for suitable bird kernels...";
        case 2: return ">>> suitable kernel found: BIRDCORE v:1999.41 [UNSTABLE]";
        case 3: return ">>> loading BIRDCORE v:1999.41...";
        case 4: return ">>> bootstrapping bird computer control utility...";
        case 5: return ">>> executing bird-check...";
        case 6: return ">>> WARNING: 103573 corrupted files found";
        case 7: return ">>> attempting automatic recovery...";
        case 8: return ">>> automatic recovery status: [FAILED]...";
        case 9: return ">>> system status: BARELY_OPERATIONAL";
        default: return NULL;
    }
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

void terminal_reset(Terminal *terminal) {
    terminal->current_line = -1;
    terminal_new_line(terminal);
    terminal->process_timer = 0.0f;
}

void terminal_setup(State *state) {
    Terminal *terminal = &state->terminal;
    switch (state->global_state) {
        default: {
            TextCopy(terminal->lines[0], "The Bird Terminal 1999");
            TextCopy(terminal->lines[1], "Sloppyright (S)");
            terminal->lines[2][0] = '\0';
            TextCopy(terminal->lines[3], "Deprecated version, install Bird Terminal 2005");
            terminal->lines[4][0] = '\0';
            terminal->current_line = 4;
            terminal_new_line(terminal);
            break;
        }
        case GLOBAL_STATE_TERMINAL_MENU_STARTUP:
        case GLOBAL_STATE_TERMINAL_GAME_STARTUP: {
            terminal_reset(terminal);
            break;
        }
    }
}

static bool terminal_update_process(State *state) {
    Terminal *terminal = &state->terminal;
    terminal->process_timer += state->delta_time;
    if (terminal->process_timer > 0.01f) {
        if (GetRandomValue(0, 4) == 0) {
            terminal->current_line++;
            return true;
        }
        terminal->process_timer = 0.0f;
    }
    return false;
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
        case GLOBAL_STATE_TERMINAL_MENU_STARTUP: {
            bool process_step_changed = terminal_update_process(state);
            char *text = terminal_get_menu_startup_text(terminal);
            if (text == NULL) {
                terminal_reset(terminal);
                return true;
            }
            ASSERT(TextLength(text) < (TERMINAL_LINE_MAX_LENGTH - 1));
            if (!TextIsEqual(terminal->lines[terminal->current_line], text)) {
                TextCopy(terminal->lines[terminal->current_line], text);
            }
            break;
        }
        case GLOBAL_STATE_TERMINAL_GAME_STARTUP: {
            terminal->process_timer += state->delta_time;
            break;
        }
        case GLOBAL_STATE_TERMINAL_MANUAL_INPUT: {
            for (int key = KEY_A; key <= KEY_Z; key++) {
                if (IsKeyPressed(key)) {
                    char c = key + 32;
                    terminal_add_char(terminal, c);
                }
            }
            if (IsKeyPressed(KEY_SPACE)) {
                terminal_add_char(terminal, ' ');
            }
            if (IsKeyPressed(KEY_BACKSPACE) && terminal->current_char > TERMINAL_LINE_START) {
                terminal->current_char--;
                terminal->lines[terminal->current_line][terminal->current_char] = '\0';
            }
            if (IsKeyPressed(KEY_ENTER)) {
                terminal_new_line(terminal);
            }
            break;
        }
    };
    return false;
}

void terminal_render(State *state) {
    Terminal *terminal = &state->terminal;
    Rectangle rectangle = {
        state->game_left,
        state->game_top,
        state->game_width,
        state->game_height
    };
    DrawRectangleRec(rectangle, TERMINAL_BG_COLOR);
    const int line_size = state->game_height / TERMINAL_LINE_CAPACITY;
    const int char_size = state->game_width / TERMINAL_LINE_MAX_LENGTH;
    for (int i = 0; i <= terminal->current_line; i++) {
        Vector2 position = {0, i * line_size};
        for (int j = 0; terminal->lines[i][j] != '\0'; j++) {
            position.x = (j * char_size);
            DrawTextCodepoint(
                state->menu.font,
                terminal->lines[i][j],
                position,
                line_size,
                TERMINAL_FG_COLOR
            );
        }
    }

    float cursor_y = line_size * terminal->current_line;
    Vector2 cursor_top = {
        char_size * terminal->current_char,
        cursor_y
    };
    Vector2 cursor_bottom = {
        char_size * terminal->current_char,
        cursor_y + line_size
    };
    DrawLineEx(cursor_top, cursor_bottom, state->scale_multiplier, RED);
}

