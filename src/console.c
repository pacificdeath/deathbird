static inline Color get_bg_color(Color fg_color) {
    return (Color) {
        fg_color.r,
        fg_color.g,
        fg_color.b,
        fg_color.a / 3,
    };
}

void command_buffer_init() {
    CommandBuffer *cb = &state->console.command_buffer;
    cb->capacity = 8;
    cb->commands = (Command *)malloc(sizeof(Command) * cb->capacity);
}

void command_buffer_add(Command command) {
    CommandBuffer *cb = &state->console.command_buffer;
    ASSERT(cb->commands != NULL);
    ASSERT(cb->count < cb->capacity);
    if (cb->count == cb->capacity - 1) {
        cb->capacity *= 2;
        cb->commands = realloc(cb->commands, sizeof(Command) * cb->capacity);
    }
    cb->commands[cb->count] = command;
    cb->count++;
}

void result_buffer_free() {
    ResultBuffer *rb = &state->console.result_buffer;
    if (rb->results != NULL) {
        free(rb->results);
    }
    memset(rb, 0, sizeof(ResultBuffer));
}

void result_buffer_reset() {
    ResultBuffer *rb = &state->console.result_buffer;
    result_buffer_free();
    rb->capacity = 8;
    rb->results = (CommandResult *)malloc(sizeof(CommandResult) * rb->capacity);
}

void result_buffer_add(const char *text, Color color) {
    ResultBuffer *rb = &state->console.result_buffer;
    ASSERT(rb->results != NULL);
    ASSERT(rb->count < rb->capacity);
    if (rb->count == rb->capacity - 1) {
        rb->capacity *= 2;
        rb->results = realloc(rb->results, sizeof(CommandResult) * rb->capacity);
    }
    CommandResult *result = &rb->results[rb->count];
    TextCopy(result->text, text);
    result->color = color;
    rb->count++;
}

static inline bool is_key_pressed_or_repeat(int key) {
    return IsKeyPressed(key) || IsKeyPressedRepeat(key);
}

static inline bool is_whitespace(char c) {
    return c == ' ' || c == '\0';
}

static inline int find_next_non_whitespace(char *string, int idx) {
    while (is_whitespace(string[idx])) {
        idx++;
    }
    if (idx >= (CONSOLE_LINE_MAX_LENGTH - 1)) {
        return -1;
    }
    return idx;
}

void console_update_dimensions(void) {
    Console *console = &state->console;
    const int padding = state->game_width * 0.1f;
    console->rectangle = (Rectangle) {
        state->game_left + padding,
        state->game_top + padding,
        state->game_width - (padding * 2),
        state->game_height - (padding * 2),
    };
}

static inline char to_lower(char c) {
    if (c >= 'A' && c <= 'Z') return c + ('a' - 'A');
    return c;
}

static int fuzzy_score(const char *pattern, const char *text) {
    int score = 0;
    int consecutive = 0;
    int p_idx = 0;
    int t_idx = 0;

    while (pattern[p_idx] != '\0' && text[t_idx] != '\0') {
        char pc = to_lower(pattern[p_idx]);
        char tc = to_lower(text[t_idx]);

        if (pc == tc) {
            // base point + bonus for streak
            score += 10 + (consecutive * 5);

            // small bias: matches earlier in the text are better
            score += (text[0] == tc) ? 15 : 0;

            ++consecutive;
            ++p_idx;
        } else {
            consecutive = 0;
        }
        ++t_idx;
    }

    // full pattern must be matched, otherwise 0
    if (pattern[p_idx] != '\0') {
        return 0;
    }

    return score;
}

void fuzzy_find_command() {
    Console *console = &state->console;
    console->mode = BIRDCORE;

    char *pattern = console->input_line;

    console->cursor_y = 0;
    console->view_offset = 0;
    console->ranked_command_count = 0;

    for (int i = 0; i < console->command_buffer.count; i++) {
        int score = fuzzy_score(pattern, console->command_buffer.commands[i].name);
        if (score == 0) continue;

        // insert at the end
        int pos = console->ranked_command_count++;
        console->ranked_commands[pos] = (RankedCommand){
            .command = &console->command_buffer.commands[i],
            .score = score,
        };

        // bubble it up until it’s in order
        while (pos > 0 && console->ranked_commands[pos].score > console->ranked_commands[pos - 1].score) {
            RankedCommand tmp = console->ranked_commands[pos - 1];
            console->ranked_commands[pos - 1] = console->ranked_commands[pos];
            console->ranked_commands[pos] = tmp;
            pos--;
        }
    }
}

void default_command_order() {
    Console *console = &state->console;
    console->ranked_command_count = console->command_buffer.count;
    for (int i = 0; i < console->command_buffer.count; i++) {
        console->ranked_commands[i] = (RankedCommand) {
            .command = &console->command_buffer.commands[i],
            .score = 0,
        };
    }
}

void console_reset(void) {
    state->console.input_line[0] = '\0';
    state->console.cursor_x = 0;
    state->console.cursor_y = 0;
    state->console.view_offset = 0;
    default_command_order();
}

void command_just_play_the_game(void) { travel_to_area(AREA_FOREST, 1); state->global_state = GLOBAL_STATE_GAME; state->flags &= ~FLAG_CONSOLE; }
void command_area_load_next(void) { travel_to_next_area(); }
void command_area_load_forest(void) { travel_to_area(AREA_FOREST, 1); }
void command_area_load_meadows(void) { travel_to_area(AREA_MEADOWS, 2); }
void command_area_load_mountains(void) { travel_to_area(AREA_MOUNTAINS, 3); }
void command_area_load_industrial(void) { travel_to_area(AREA_GREEN, 4); }
void command_area_load_void(void) { travel_to_area(AREA_VOID, 5); }
void command_area_load_red(void) { travel_to_area(AREA_INDUSTRIAL, 6); }
void command_area_load_black(void) { travel_to_area(AREA_CASTLE, 7); }
void command_area_load_void2(void) { travel_to_area(AREA_SKY, 8); }
void command_area_play_forest(void) { command_just_play_the_game(); }
void command_area_play_meadows(void) { travel_to_area(AREA_MEADOWS, 2); state->global_state = GLOBAL_STATE_GAME; }
void command_area_play_mountains(void) { travel_to_area(AREA_MOUNTAINS, 3); state->global_state = GLOBAL_STATE_GAME; }
void command_area_play_industrial(void) { travel_to_area(AREA_GREEN, 4); state->global_state = GLOBAL_STATE_GAME; }
void command_area_play_void(void) { travel_to_area(AREA_VOID, 5); state->global_state = GLOBAL_STATE_GAME; }
void command_area_play_red(void) { travel_to_area(AREA_INDUSTRIAL, 6); state->global_state = GLOBAL_STATE_GAME; }
void command_area_play_black(void) { travel_to_area(AREA_CASTLE, 7); state->global_state = GLOBAL_STATE_GAME; }
void command_area_play_void2(void) { travel_to_area(AREA_SKY, 8); state->global_state = GLOBAL_STATE_GAME; }
void command_enable_collision_bounds(void) { state->flags |= FLAG_SHOW_COLLISION_BOUNDS; }
void command_disable_collision_bounds(void) { state->flags &= ~FLAG_SHOW_COLLISION_BOUNDS; }
void command_show_out_of_bounds(void) { state->flags |= FLAG_SHOW_OUT_OF_BOUNDS; }
void command_hide_out_of_bounds(void) { state->flags &= ~FLAG_SHOW_OUT_OF_BOUNDS; }
void command_level_win(void) { state->portal_fuel = state->current_level.required_fuel; }
void command_game_over(void) { state->global_state = GLOBAL_STATE_GAME_OVER; }
void command_time_normal(void) { state->time_scale = 1.0f; }
void command_time_slow(void) { state->time_scale = 0.5f; }
void command_time_super_slow(void) { state->time_scale = 0.1f; }
void command_time_pause(void) { state->time_scale = 0.0f; }
void command_time_fast(void) { state->time_scale = 2.0f; }
void command_time_super_fast(void) { state->time_scale = 10.0f; }
void command_enable_weak_bird_sharks(void) {
    state->flags |= FLAG_BIRD_SHARK_HEALTH_1;
    if (bird_boss_exists() && !bird_boss_defeated()) {
        for (int i = 0; i < BOSS_CAPACITY; i++) {
            state->bosses[i].bird->alive.health = 1;
        }
    }
}
void command_disable_weak_bird_sharks(void) { state->flags &= ~FLAG_BIRD_SHARK_HEALTH_1; }

void command_show_global_state(void) {
    result_buffer_reset();
    result_buffer_add(TextFormat("STATE:    %s", global_state_names[state->global_state]), CONSOLE_WHITE);
    result_buffer_add(TextFormat("PLAYER:   %s", player_state_names[state->player.state]), CONSOLE_WHITE);
    result_buffer_add(TextFormat("AREA:     %s", area_names[state->area]), CONSOLE_WHITE);
    result_buffer_add(TextFormat("LEVEL:    %i", state->level_idx), CONSOLE_WHITE);
    result_buffer_add(TextFormat("FADER:    %.2f [0.0 -> 255.0]", state->fader_level), CONSOLE_WHITE);
    result_buffer_add(TextFormat("CARTOON   %.2f [0.0 ->   2.0]", state->cartoon_transition_size), CONSOLE_WHITE);
    result_buffer_add(TextFormat("PORTAL:   %.2f [1.0 ->   0.0]", state->portal_linear_disappearance), CONSOLE_WHITE);
}

void command_list_birds() {
    result_buffer_reset();
    for (int i = 0; i < BIRD_CAPACITY; i++) {
        Bird *b = &state->birds[i];
        Color color = CONSOLE_PURPLE;
        const char *state_name = bird_state_names[b->state];
        const char *type_name = bird_type_names[b->type];
        switch (b->state) {
            default: break;
            case BIRD_STATE_AVAILABLE: color = CONSOLE_WHITE; break;
            case BIRD_STATE_ALIVE: color = CONSOLE_GREEN; break;
            case BIRD_STATE_DEAD: color = CONSOLE_RED; break;
            case BIRD_STATE_RESERVED: color = CONSOLE_BLUE; break;
        }
        result_buffer_add(TextFormat("%-4i STATE: %-16s TYPE: %-16s", i, state_name, type_name), color);
    }
}

void command_list_birds_live() {
    state->console.live_function = command_list_birds;
    command_list_birds();
}

void command_show_palette(int area) {
    ASSERT(area < AREA_TOTAL);
    result_buffer_reset();
    for (int i = 0; i < BIRD_PALETTE_IDX_TOTAL; i++) {
        const char *name = bird_palette_idx_names[i];
        Vector3 v = state->bird_palette[area][i];
        Color c = {
            .r = v.x * 255.0f,
            .g = v.y * 255.0f,
            .b = v.z * 255.0f,
            .a = 255,
        };
        result_buffer_add(TextFormat( "%-3i #%02x%02x%02x %s", i, c.r, c.g, c.b, name), c);
    }
}
void command_palette_show_forest(void) { command_show_palette(BIRD_PALETTE_WHITE); }
void command_palette_show_meadows(void) { command_show_palette(BIRD_PALETTE_YELLOW); }
void command_palette_show_mountains(void) { command_show_palette(BIRD_PALETTE_BROWN); }
void command_palette_show_industrial(void) { command_show_palette(BIRD_PALETTE_GREEN); }
void command_palette_show_void(void) { command_show_palette(BIRD_PALETTE_VOID); }
void command_palette_show_red(void) { command_show_palette(BIRD_PALETTE_RED); }
void command_palette_show_black(void) { command_show_palette(BIRD_PALETTE_BLACK); }
void command_palette_show_void2(void) { TraceLog(LOG_ERROR, "Unclear"); }

void command_show_textures() {
    Console *console = &state->console;
    if (is_key_pressed_or_repeat(KEY_RIGHT)) {
        console->atlas_sprite_idx = (console->atlas_sprite_idx + 1) % SPRITE_COUNT;
    } else if (is_key_pressed_or_repeat(KEY_LEFT)) {
        console->atlas_sprite_idx = ((console->atlas_sprite_idx + SPRITE_COUNT) - 1) % SPRITE_COUNT;
    } else if (is_key_pressed_or_repeat(KEY_UP)) {
        console->atlas_palette_idx = (console->atlas_palette_idx + 1) % AREA_TOTAL;
    } else if (is_key_pressed_or_repeat(KEY_DOWN)) {
        console->atlas_palette_idx = ((console->atlas_palette_idx + AREA_TOTAL) - 1) % AREA_TOTAL;
    }
}

void command_show_textures_live() {
    state->console.live_function = command_show_textures;
    command_show_textures();
}

static inline void console_register_command(int type, char *name, ConsoleFunction function) {
    command_buffer_add((Command){type,name,function});
}

void console_init(void) {
    state->flags |= FLAG_CONSOLE;
    Console *console = &state->console;
    console->mode = BIRDCORE;
    console_update_dimensions();
    command_buffer_init();

    console_register_command(BIRDCORE, "Just Play The Game", command_just_play_the_game);
    console_register_command(BIRDCORE, "Enable Collision Bounds", command_enable_collision_bounds);
    console_register_command(BIRDCORE, "Disable Collision Bounds", command_disable_collision_bounds);
    console_register_command(BIRDCORE, "Show Out Of Bounds", command_show_out_of_bounds);
    console_register_command(BIRDCORE, "Hide Out Of Bounds", command_hide_out_of_bounds);
    console_register_command(BIRDCORE, "Area Load Next", command_area_load_next);
    console_register_command(BIRDCORE, "Area Load Forest", command_area_load_forest);
    console_register_command(BIRDCORE, "Area Load Meadows", command_area_load_meadows);
    console_register_command(BIRDCORE, "Area Load Mountains", command_area_load_mountains);
    console_register_command(BIRDCORE, "Area Load Industrial", command_area_load_industrial);
    console_register_command(BIRDCORE, "Area Load Void", command_area_load_void);
    console_register_command(BIRDCORE, "Area Load Red", command_area_load_red);
    console_register_command(BIRDCORE, "Area Load Black", command_area_load_black);
    console_register_command(BIRDCORE, "Load Area Void2", command_area_load_void2);
    console_register_command(BIRDCORE, "Area Play Forest, Level 1", command_area_play_forest);
    console_register_command(BIRDCORE, "Area Play Meadows, Level 2", command_area_play_meadows);
    console_register_command(BIRDCORE, "Area Play Mountains, Level 3", command_area_play_mountains);
    console_register_command(BIRDCORE, "Area Play Industrial, Level 4", command_area_play_industrial);
    console_register_command(BIRDCORE, "Area Play Void, Level 5", command_area_play_void);
    console_register_command(BIRDCORE, "Area Play Red, Level 6", command_area_play_red);
    console_register_command(BIRDCORE, "Area Play Black, Level 7", command_area_play_black);
    console_register_command(BIRDCORE, "Area Play Sky, Level 8", command_area_play_void2);
    console_register_command(BIRDCORE, "Level Win", command_level_win);
    console_register_command(BIRDCORE, "Game Over", command_game_over);
    console_register_command(BIRDCORE, "Time Normal", command_time_normal);
    console_register_command(BIRDCORE, "Time Slow", command_time_slow);
    console_register_command(BIRDCORE, "Time Super Slow", command_time_super_slow);
    console_register_command(BIRDCORE, "Time Pause", command_time_pause);
    console_register_command(BIRDCORE, "Time Fast", command_time_fast);
    console_register_command(BIRDCORE, "Time Super Fast", command_time_super_fast);
    console_register_command(BIRDCOMPUTER, "Show Global State", command_show_global_state);
    console_register_command(BIRDCOMPUTER, "List Birds", command_list_birds);
    console_register_command(BIRDCOMPUTER, "List Birds Live", command_list_birds_live);
    console_register_command(BIRDCOMPUTER, "Palette Show Forest", command_palette_show_forest);
    console_register_command(BIRDCOMPUTER, "Palette Show Meadows", command_palette_show_meadows);
    console_register_command(BIRDCOMPUTER, "Palette Show Mountains", command_palette_show_mountains);
    console_register_command(BIRDCOMPUTER, "Palette Show Industrial", command_palette_show_industrial);
    console_register_command(BIRDCOMPUTER, "Palette Show Void", command_palette_show_void);
    console_register_command(BIRDCOMPUTER, "Palette Show Red", command_palette_show_red);
    console_register_command(BIRDCOMPUTER, "Palette Show Black", command_palette_show_black);
    console_register_command(BIRDCOMPUTER, "Palette Show Void2", command_palette_show_void2);
    console_register_command(BIRDATLAS, "Show Textures", command_show_textures_live);
    console_register_command(BIRDCORE, "Enable Weak Bird-Sharks", command_enable_weak_bird_sharks);
    console_register_command(BIRDCORE, "Disable Weak Bird-Sharks", command_disable_weak_bird_sharks);

    console->ranked_commands = (RankedCommand *)malloc(sizeof(RankedCommand) * console->command_buffer.capacity);

    console_reset();
}

void console_cleanup() {
    free(state->console.command_buffer.commands);
    free(state->console.ranked_commands);
    result_buffer_free();
}

static inline void console_add_char(char c) {
    Console *console = &state->console;

    if (console->cursor_x >= (CONSOLE_LINE_MAX_LENGTH - 1)) {
        return;
    }

    console->input_line[console->cursor_x] = c;
    console->input_line[console->cursor_x + 1] = '\0';
    console->cursor_x++;

    fuzzy_find_command();
}

void console_update_view_offset() {
    Console *console = &state->console;
    int view_max = console->view_offset + CONSOLE_MAX_LINES;
    if (console->cursor_y < console->view_offset) {
        console->view_offset = console->cursor_y;
    } else if (console->cursor_y >= view_max) {
        console->view_offset = console->cursor_y - CONSOLE_MAX_LINES + 1;
    }
}

void console_input() {
    Console *console = &state->console;
    for (int key = KEY_A; key <= KEY_Z; key++) {
        if (IsKeyPressed(key)) {
            char c = key + 32;
            console_add_char(c);
        }
    }
    for (int key = KEY_ZERO; key <= KEY_NINE; key++) {
        if (IsKeyPressed(key)) {
            console_add_char(key);
        }
    }
    if (IsKeyPressed(KEY_SPACE)) {
        console_add_char(' ');
    }
    if (is_key_pressed_or_repeat(KEY_BACKSPACE) && console->cursor_x > 0) {
        console->cursor_x--;
        console->input_line[console->cursor_x] = '\0';
        if (console->cursor_x == 0) {
            default_command_order();
        } else {
            fuzzy_find_command();
        }
    }
}

void console_update(void) {
    Console *console = &state->console;

    if (IsKeyPressed(KEY_ENTER)) {
        switch (console->mode) {
            case BIRDCORE: {
                Command *command = console->ranked_commands[console->cursor_y].command;
                command->function();
                switch (command->type) {
                    case BIRDCORE:
                        console_reset();
                        break;
                    case BIRDCOMPUTER:
                        console->mode = BIRDCOMPUTER;
                        break;
                    case BIRDATLAS:
                        console->mode = BIRDATLAS;
                        break;
            } break;
            case BIRDCOMPUTER:
            case BIRDATLAS: {
                console->cursor_y = 0;
                console->view_offset = 0;
                console->mode = BIRDCORE;
                console->live_function = NULL;
            } break;
        }
        }
    }

    switch (console->mode) {
        case BIRDCORE:
            console_input();
            if (is_key_pressed_or_repeat(KEY_UP) && console->cursor_y < console->ranked_command_count - 1) {
                console->cursor_y++;
                console_update_view_offset();
            } else if (is_key_pressed_or_repeat(KEY_DOWN) && console->cursor_y > 0) {
                console->cursor_y--;
                console_update_view_offset();
            }
            break;
        case BIRDCOMPUTER:
            console_input();
            if (is_key_pressed_or_repeat(KEY_UP) && console->view_offset < console->result_buffer.count - CONSOLE_MAX_LINES) {
                console->view_offset++;
            } else if (is_key_pressed_or_repeat(KEY_DOWN) && console->view_offset > 0) {
                console->view_offset--;
            }
            break;
        case BIRDATLAS:
            break;
    }

    if (console->live_function != NULL) {
        console->live_function();
    }
}

void console_render_line(const char *text, int line_idx, float line_size, float char_size, Color bg, Color fg, bool outline) {
    Console *console = &state->console;

    Rectangle rec = {
        console->rectangle.x,
        console->rectangle.y + console->rectangle.height - (line_idx * line_size) - line_size,
        console->rectangle.width,
        line_size,
    };

    if (bg.a > 0) {
        DrawRectangleRec(rec, bg);
    } else {
        int avg = (fg.r + fg.g + fg.b) / 3;
        if (avg < 64) {
            DrawRectangleRec(rec, (Color){255,255,255,128});
        }
    }

    if (outline) {
        DrawRectangleLinesEx(rec, 1, fg);
    }

    for (int i = 0; text[i] != '\0'; i++) {
        rec.x = console->rectangle.x + (i * char_size);
        DrawTextCodepoint(
            state->font,
            text[i],
            (Vector2){rec.x, rec.y},
            line_size,
            fg
        );
    }
}

void console_render_cursor(int char_size, int line_size) {
    Console *console = &state->console;
    Vector2 cursor_bottom = {
        console->rectangle.x + char_size * console->cursor_x,
        console->rectangle.y + console->rectangle.height - (line_size * CONSOLE_INPUT_LINE_INDEX),
    };
    Vector2 cursor_top = {
        cursor_bottom.x,
        cursor_bottom.y - line_size,
    };
    DrawLineEx(cursor_top, cursor_bottom, state->scale_multiplier, RED);
}

void console_render(void) {
    Console *console = &state->console;
    DrawRectangleRec(console->rectangle, CONSOLE_BG);
    DrawRectangleLinesEx(console->rectangle, 1, CONSOLE_WHITE);
    const float line_size = console->rectangle.height / CONSOLE_MAX_LINES;
    const float char_size = console->rectangle.width / CONSOLE_LINE_MAX_LENGTH;

    switch (console->mode) {
        case BIRDCORE: {
            console_render_line(
                console->input_line,
                CONSOLE_INPUT_LINE_INDEX,
                line_size,
                char_size,
                CONSOLE_BG,
                CONSOLE_WHITE,
                true
            );

            for (int i = console->view_offset; i < console->ranked_command_count && i < console->view_offset + CONSOLE_MAX_LINES; i++) {
                int line_idx = i - console->view_offset;
                bool is_selected = i == console->cursor_y;
                Color fg;
                switch (console->ranked_commands[i].command->type) {
                    default: ASSERT(false); break;
                    case BIRDCORE: fg = CONSOLE_GREEN; break;
                    case BIRDCOMPUTER: fg = CONSOLE_PURPLE; break;
                    case BIRDATLAS: fg = CONSOLE_BLUE; break;
                }
                Color bg = is_selected ? get_bg_color(fg) : (Color){0};
                console_render_line(
                    console->ranked_commands[i].command->name,
                    line_idx,
                    line_size,
                    char_size,
                    bg,
                    fg,
                    is_selected
                );
            }
            console_render_cursor(char_size, line_size);
        } break;
        case BIRDCOMPUTER: {
            for (int i = console->view_offset; i < console->result_buffer.count && i < (console->view_offset + CONSOLE_MAX_LINES); i++) {
                int line_idx = i - console->view_offset;
                console_render_line(
                    console->result_buffer.results[i].text,
                    line_idx,
                    line_size,
                    char_size,
                    (Color){0},
                    console->result_buffer.results[i].color,
                    false
                );
            }
        } break;
        case BIRDATLAS: {
            bool bird_shader = console->atlas_palette_idx > AREA_NONE;

            const char *area_name = area_names[console->atlas_palette_idx];
            Vector2 text_position = { console->rectangle.x, console->rectangle.y };
            console_render_line(
                TextFormat("Sprite: %i Palette: \"%s\"", console->atlas_sprite_idx, area_name),
                0,
                line_size,
                char_size,
                (Color){0},
                CONSOLE_WHITE,
                false
            );

            if (bird_shader) {
                prepare_bird_shader(console->atlas_palette_idx);
                BeginShaderMode(state->bird_shader);
            }
            atlas_draw(state, console->atlas_sprite_idx, (Vector2){0}, 0.0f, SCALE_NORMAL, WHITE);
            if (bird_shader) {
                EndShaderMode();
            }
        } break;
    }
}

