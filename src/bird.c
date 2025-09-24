#define GRAVITY 4.0f
#define ANIM_SPRITE_AMOUNT 4
#define GORE_SPRITE_AMOUNT 2
#define DAMAGE_TIME 0.2
#define DEATH_VELOCITY_MULTIPLIER 0.8f
#define EXPLOSIVE_VELOCITY_MULTIPLIER 1.3f
#define DEATH_VELOCITY_MULTIPLIER_PORTAL_INHALE 1.5f
#define DEATH_VELOCITY_MULTIPLIER_PORTAL_EXHALE 0.8f
#define DEATH_MAX_RANDOM_VELOCITY 1.4f
#define DEATH_GROUND_BOUNCE_VELOCITY_THRESHOLD 2.0f
#define DEATH_GROUND_BOUNCE_VELOCITY -0.4f
#define DEATH_MAX_RANDOM_ANGULAR_VELOCITY 1400.0f
#define RESET_RIGHT 1.2f
#define RESET_LEFT (-RESET_RIGHT)
#define VERTICAL_FREEDOM 0.8f
#define GIANT_SPLASH_RADIUS 1.2f
#define PLAYER_DAMAGE 1
#define EXPLOSIVE_DAMAGE 5

static inline Vector2 get_bird_scale(Bird *bird) {
    return (bird->flip_sprite ? SCALE_X_FLIP : SCALE_NORMAL);
}

void list_all_birds() {
    for (int i = 0; i < BIRD_CAPACITY; i++) {
        Bird *b = &state->birds[i];
        TraceLog(LOG_ERROR, "%i %s %s", i, bird_state_names[b->state], bird_type_names[b->type]);
    }
}

void bird_set_available(Bird *bird) {
    *bird = (Bird){0};
    bird->state = BIRD_STATE_AVAILABLE;
}

Bird *get_available_bird() {
    for (int i = 0; i < BIRD_CAPACITY; i++) {
        if (state->birds[i].state == BIRD_STATE_AVAILABLE) {
            return &state->birds[i];
        }
    }
    ASSERT(false);
    return NULL;
}

Bird *get_available_bird_in_reverse_order() {
    for (int i = BIRD_CAPACITY - 1; i >= 0; i--) {
        if (state->birds[i].state == BIRD_STATE_AVAILABLE) {
            return &state->birds[i];
        }
    }
    ASSERT(false);
    return NULL;
}

bool are_all_birds_available() {
    for (int i = 0; i < BIRD_CAPACITY; i++) {
        if (state->birds[i].state != BIRD_STATE_AVAILABLE) return false;
    }
    return true;
}

void restore_all_reserved_birds() {
    for (int i = 0; i < BIRD_CAPACITY; i++) {
        Bird *b = &state->birds[i];
        if (b->state == BIRD_STATE_RESERVED) {
            bird_set_available(b);
        }
        if (b->type == BIRD_TYPE_RESERVED) {
            b->type = BIRD_TYPE_REGULAR;
        }
    }
}

void restore_bosses() {
    for (int i = 0; i < BOSS_CAPACITY; i++) {
        state->bosses[i].bird = NULL;
    }
}

void draw_bird_collision_bounds(Bird *bird) {
    Vector2 px_bird_position = to_pixel_position(state, bird->position);
    Vector2 px_collision_bounds = to_pixel_size(state, bird->alive.collision_bounds);
    Vector2 px_rec_top_left = {
        px_bird_position.x - px_collision_bounds.x,
        px_bird_position.y - px_collision_bounds.y
    };
    Rectangle collision_rec = {
        .x = px_rec_top_left.x,
        .y = px_rec_top_left.y,
        .width = px_collision_bounds.x * 2,
        .height = px_collision_bounds.y * 2
    };
    DrawRectangleLinesEx(collision_rec, state->scale_multiplier, GREEN);
}

void get_bird_death_part_sprites(Sprite sprites[BIRD_DEATH_PARTS], int bird_type) {
    if (bird_type == BIRD_TYPE_MONITOR) {
        // this uses trailing fireballs which is rendered separately
        return;
    }

    // sprites are added in rendering order
    Sprite gore_sprite = SPRITE_BIRD_GORE1;
    for (int i = 0; i < BIRD_DEATH_GORE_PARTS; i += GORE_SPRITE_AMOUNT) {
        for (int j = 0; j < GORE_SPRITE_AMOUNT; j++) {
            int idx = i + j;

            ASSERT(idx < BIRD_DEATH_PARTS);

            sprites[idx] = gore_sprite + j;
        }
    }
    switch (bird_type) {
        default: ASSERT(false);
        case BIRD_TYPE_REGULAR:
        case BIRD_TYPE_RESERVED:
        case BIRD_TYPE_STATIONARY: {
            sprites[BIRD_DEATH_PARTS - 6] = SPRITE_BIRD_WING;
            sprites[BIRD_DEATH_PARTS - 5] = SPRITE_BIRD_BODY;
            sprites[BIRD_DEATH_PARTS - 4] = SPRITE_BIRD_WING;
            sprites[BIRD_DEATH_PARTS - 3] = SPRITE_BIRD_EYE;
            sprites[BIRD_DEATH_PARTS - 2] = SPRITE_BIRD_HEAD;
            sprites[BIRD_DEATH_PARTS - 1] = SPRITE_BIRD_EYE;
        } break;
        case BIRD_TYPE_UMBRELLA_ABOVE:
        case BIRD_TYPE_UMBRELLA_UNDER: {
            sprites[BIRD_DEATH_PARTS - 6] = SPRITE_BIRD_GORE1;
            sprites[BIRD_DEATH_PARTS - 5] = SPRITE_BIRD_GORE2;
            sprites[BIRD_DEATH_PARTS - 4] = SPRITE_BIRD_GORE1;
            sprites[BIRD_DEATH_PARTS - 3] = SPRITE_BIRD_EYE;
            sprites[BIRD_DEATH_PARTS - 2] = SPRITE_BIRD_EYE;
            sprites[BIRD_DEATH_PARTS - 1] = SPRITE_UMBRELLA;
        } break;
        case BIRD_TYPE_GIANT: {
            sprites[BIRD_DEATH_PARTS - 6] = SPRITE_GIANT_BIRD_WING;
            sprites[BIRD_DEATH_PARTS - 5] = SPRITE_GIANT_BIRD_BODY;
            sprites[BIRD_DEATH_PARTS - 4] = SPRITE_GIANT_BIRD_WING;
            sprites[BIRD_DEATH_PARTS - 3] = SPRITE_BIRD_EYE;
            sprites[BIRD_DEATH_PARTS - 2] = SPRITE_GIANT_BIRD_HEAD;
            sprites[BIRD_DEATH_PARTS - 1] = SPRITE_BIRD_EYE;
        } break;
        case BIRD_TYPE_TINY: {
            sprites[BIRD_DEATH_PARTS - 6] = SPRITE_BIRD_GORE1;
            sprites[BIRD_DEATH_PARTS - 5] = SPRITE_BIRD_GORE2;
            sprites[BIRD_DEATH_PARTS - 4] = SPRITE_BIRD_GORE1;
            sprites[BIRD_DEATH_PARTS - 3] = SPRITE_BIRD_GORE2;
            sprites[BIRD_DEATH_PARTS - 2] = SPRITE_BIRD_GORE1;
            sprites[BIRD_DEATH_PARTS - 1] = SPRITE_BIRD_GORE2;
        } break;
        case BIRD_TYPE_BOMB: {
            sprites[BIRD_DEATH_PARTS - 6] = SPRITE_BIRD_EYE;
            sprites[BIRD_DEATH_PARTS - 5] = SPRITE_BIRD_GORE1;
            sprites[BIRD_DEATH_PARTS - 4] = SPRITE_BIRD_GORE2;
            sprites[BIRD_DEATH_PARTS - 3] = SPRITE_BIRD_GORE1;
            sprites[BIRD_DEATH_PARTS - 2] = SPRITE_BIRD_GORE2;
            sprites[BIRD_DEATH_PARTS - 1] = SPRITE_BIRD_EYE;
        } break;
        case BIRD_TYPE_WHEELS: {
            sprites[BIRD_DEATH_PARTS - 6] = SPRITE_WHEEL;
            sprites[BIRD_DEATH_PARTS - 5] = SPRITE_BIRD_GORE1;
            sprites[BIRD_DEATH_PARTS - 4] = SPRITE_BIRD_GORE2;
            sprites[BIRD_DEATH_PARTS - 3] = SPRITE_BIRD_GORE1;
            sprites[BIRD_DEATH_PARTS - 2] = SPRITE_BIRD_GORE2;
            sprites[BIRD_DEATH_PARTS - 1] = SPRITE_WHEEL;
        } break;
        case BIRD_TYPE_SHARK: {
            sprites[BIRD_DEATH_PARTS - 6] = SPRITE_SHARK_WING;
            sprites[BIRD_DEATH_PARTS - 5] = SPRITE_SHARK_BODY;
            sprites[BIRD_DEATH_PARTS - 4] = SPRITE_SHARK_WING;
            sprites[BIRD_DEATH_PARTS - 3] = SPRITE_SHARK_EYE;
            sprites[BIRD_DEATH_PARTS - 2] = SPRITE_SHARK_HEAD;
            sprites[BIRD_DEATH_PARTS - 1] = SPRITE_SHARK_EYE;
        } break;
        case BIRD_TYPE_MONITOR: {
            ASSERT(false);
        } break;
    }
}

void update_death_animation(Bird *bird) {
    if (bird->dead.anim_idx < bird->dead.anim_sprite_amount) {
        bird->anim_time += state->delta_time;
        if (bird->anim_time > GAME_ANIM_SPEED) {
            bird->anim_time = 0.0f;
            bird->dead.anim_idx++;
        }
    }
}

void death_animation_render(Bird *bird) {
    if (bird->dead.anim_idx < bird->dead.anim_sprite_amount) {
        Sprite blood_sprite = bird->dead.anim_start_sprite + bird->dead.anim_idx;
        atlas_draw(state, blood_sprite, bird->position, 0.0f, get_bird_scale(bird), OPAQUE);
    }
}

Vector2 get_trailing_fireball_history_position(int part, double seconds_ago, double now) {
    TrailingFireballHistory *history = &state->trailing_fireball_history;
    double target_time = now - seconds_ago;

    int idx = history->write_index;
    for (int n = 0; n < TRAILING_FIREBALL_HISTORY_SIZE; n++) {
        int prev_idx = (idx - 1 + TRAILING_FIREBALL_HISTORY_SIZE) % TRAILING_FIREBALL_HISTORY_SIZE;

        double t1 = history->times[part][prev_idx];
        double t2 = history->times[part][idx];

        if (t1 <= target_time && target_time <= t2) {
            Vector2 p1 = history->positions[part][prev_idx];
            Vector2 p2 = history->positions[part][idx];

            double alpha = (target_time - t1) / (t2 - t1 + 1e-6f);
            return (Vector2) {
                p1.x + (p2.x - p1.x) * alpha,
                p1.y + (p2.y - p1.y) * alpha
            };
        }

        idx = prev_idx;
    }

    // Fallback: if target_time is older than buffer
    return history->positions[part][idx];
}

void prepare_trailing_fireballs(Bird *bird) {
    TrailingFireballs *tfb = &state->trailing_fireballs;
    TrailingFireballHistory *history = &state->trailing_fireball_history;

    tfb->bird = bird;
    history->write_index = 0;

    double time = GetTime();

    for (int i = 0; i < BIRD_DEATH_PARTS; i++) {
        for (int j = 0; j < TRAILING_FIREBALL_HISTORY_SIZE; j++) {
            history->positions[i][j] = bird->position;
            history->times[i][j] = time;
        }
        for (int j = 0; j < TRAILING_FIREBALLS_PER_DEATH_PART; j++) {
            tfb->positions[i][j] = bird->position;
        }
    }
}

bool update_trailing_fireballs() {
    TrailingFireballs *tfb = &state->trailing_fireballs;
    TrailingFireballHistory *history = &state->trailing_fireball_history;

    history->write_index = (history->write_index + 1) % TRAILING_FIREBALL_HISTORY_SIZE;

    double time = GetTime();

    for (int i = 0; i < BIRD_DEATH_PARTS; i++) {
        history->positions[i][history->write_index] = tfb->bird->dead.positions[i];
        history->times[i][history->write_index] = time;
    }

    // Fill the visible fireballs using history lookups
    for (int i = 0; i < BIRD_DEATH_PARTS; i++) {
        tfb->positions[i][0] = tfb->bird->dead.positions[i];

        for (int j = 1; j < TRAILING_FIREBALLS_PER_DEATH_PART; j++) {
            double delay = j * 0.05f;
            tfb->positions[i][j] = get_trailing_fireball_history_position(i, delay, time);
        }
    }

    for (int i = 0; i < BIRD_DEATH_PARTS; i++) {
        Vector2 first = tfb->positions[i][0];
        Vector2 last = tfb->positions[i][TRAILING_FIREBALLS_PER_DEATH_PART - 1];

        if (first.x != last.x || first.y != last.y) {
            return false;
        }
    }

    return true;
}

void render_trailing_fireballs() {
    TrailingFireballs *tfb = &state->trailing_fireballs;
    for (int i = 0; i < BIRD_DEATH_PARTS; i++) {
        for (int j = TRAILING_FIREBALLS_PER_DEATH_PART - 1; j >= 0; j--) {
            atlas_draw(
                state,
                SPRITE_FIREBALL1 + j,
                tfb->positions[i][j],
                tfb->bird->dead.rotations[i],
                SCALE_NORMAL,
                OPAQUE
            );
        }
    }
}

void render_trailing_fireballs_with_portal_influence() {
    TrailingFireballs *tfb = &state->trailing_fireballs;
    for (int i = 0; i < BIRD_DEATH_PARTS; i++) {
        for (int j = TRAILING_FIREBALLS_PER_DEATH_PART - 1; j >= 0; j--) {
            float ratio = portal_distance_to_center_ratio(state, tfb->positions[i][j]);
            float c = ratio * 255.0f;
            Color color = {c,c,c,255};
            atlas_draw( state, SPRITE_FIREBALL1 + j, tfb->positions[i][j], tfb->bird->dead.rotations[i], SCALE_UNIFORM(ratio), OPAQUE);
        }
    }
}

void bird_death_setup(Bird *bird, Vector2 master_velocity, float multiplier) {
    bird->state = BIRD_STATE_DEAD;
    for (int i = 0; i < BIRD_DEATH_PARTS; i++) {
        bird->dead.positions[i] = bird->position;
        Vector2 velocity = {
            (master_velocity.x + randf(-DEATH_MAX_RANDOM_VELOCITY, DEATH_MAX_RANDOM_VELOCITY)) * multiplier,
            (master_velocity.y + randf(-DEATH_MAX_RANDOM_VELOCITY, DEATH_MAX_RANDOM_VELOCITY)) * multiplier
        };
        bird->dead.velocities[i] = velocity;
        float angular_velocity = randf(-DEATH_MAX_RANDOM_ANGULAR_VELOCITY, DEATH_MAX_RANDOM_ANGULAR_VELOCITY);
        bird->dead.angular_velocities[i] = angular_velocity;
        bird->dead.rotations[i] = 0.0f;
        bird->dead.anim_idx = 0;
        bird->anim_time = 0.0f;
        if (velocity.x == 0.0f || velocity.y == 0.0f) {
            bird->dead.velocities[i] = (Vector2){0.0f, -0.1f};
        }
    }

    bird->dead.anim_start_sprite = SPRITE_BIRD_BLOOD1;
    bird->dead.anim_sprite_amount = 4;

    switch (bird->type) {
        default: break;
        case BIRD_TYPE_UMBRELLA_UNDER: {
            int umbrella_idx = BIRD_DEATH_PARTS - 1;
            bird->dead.rotations[umbrella_idx] = 180.0f;
        } break;
        case BIRD_TYPE_BOMB:
        case BIRD_TYPE_WHEELS: {
            bird->dead.anim_start_sprite = SPRITE_EXPLOSION1;
            bird->dead.anim_sprite_amount = 6;
        } break;
        case BIRD_TYPE_MONITOR: {
            bird->dead.anim_start_sprite = SPRITE_EXPLOSION1;
            bird->dead.anim_sprite_amount = 6;
            // this uses trailing fireballs which are rendered separately
            prepare_trailing_fireballs(bird);
        } break;
    }
}

void render_bird_parts_with_portal_influence(Bird *bird) {
    if (bird->type == BIRD_TYPE_MONITOR) {
        render_trailing_fireballs_with_portal_influence();
        return;
    }

    Sprite sprites[BIRD_DEATH_PARTS];
    get_bird_death_part_sprites(sprites, bird->type);
    for (int i = 0; i < BIRD_DEATH_PARTS; i++) {
        float ratio = portal_distance_to_center_ratio(state, bird->dead.positions[i]);
        float c = ratio * 255.0f;
        Color color = {c,c,c,255};
        Vector2 scale = get_bird_scale(bird);
        atlas_draw(state, sprites[i], bird->dead.positions[i], bird->dead.rotations[i], SCALE_UNIFORM(ratio), color);
    }
}

void update_bird_spawn_weights() {
    for (int i = 0; i < SPAWN_TYPES_TOTAL; i++) {
        state->bird_spawn_weights[i] = 0;
    }
    switch (state->level_idx) {
        case 1:
            state->bird_spawn_weights[SPAWN_TYPE_REGULAR] = 1;
            return;
        case 2:
            state->bird_spawn_weights[SPAWN_TYPE_REGULAR] = 9;
            state->bird_spawn_weights[SPAWN_TYPE_UMBRELLA] = 1;
            return;
        case 3:
            state->bird_spawn_weights[SPAWN_TYPE_REGULAR] = 9;
            state->bird_spawn_weights[SPAWN_TYPE_GIANT] = 1;
            return;
        case 4:
            state->bird_spawn_weights[SPAWN_TYPE_REGULAR] = 8;
            state->bird_spawn_weights[SPAWN_TYPE_UMBRELLA] = 1;
            return;
        case 5:
            state->bird_spawn_weights[SPAWN_TYPE_REGULAR] = 1;
            return;
        case 6:
            state->bird_spawn_weights[SPAWN_TYPE_REGULAR] = 1;
            return;
        case 7:
            state->bird_spawn_weights[SPAWN_TYPE_REGULAR] = 7;
            state->bird_spawn_weights[SPAWN_TYPE_UMBRELLA] = 1;
            state->bird_spawn_weights[SPAWN_TYPE_GIANT] = 1;
            return;
        case 8:
            state->bird_spawn_weights[SPAWN_TYPE_REGULAR] = 1;
            return;
        case 9:
            state->bird_spawn_weights[SPAWN_TYPE_REGULAR] = 3;
            state->bird_spawn_weights[SPAWN_TYPE_UMBRELLA] = 1;
            state->bird_spawn_weights[SPAWN_TYPE_GIANT] = 1;
            return;
        case 10:
            state->bird_spawn_weights[SPAWN_TYPE_REGULAR] = 1;
            state->bird_spawn_weights[SPAWN_TYPE_UMBRELLA] = 1;
            return;
        case 11:
            state->bird_spawn_weights[SPAWN_TYPE_REGULAR] = 1;
            state->bird_spawn_weights[SPAWN_TYPE_GIANT] = 1;
            return;
        default:
            state->bird_spawn_weights[SPAWN_TYPE_REGULAR] = 1;
            return;
    }
}

float bird_move_speed_by_y_position(float y) {
    return 0.15f + (y + 0.75f) * 0.15f;
}

float get_next_bird_spawn_y_position() {
    if (state->bird_height_idx == 0) {
        int used_sections = 0;
        for (int i = 0; i < BIRD_Y_SECTIONS; i++) {
            int idx = GetRandomValue(0, BIRD_Y_SECTIONS - 1);
            while (has_flag(used_sections, (1 << idx))) {
                idx++;
                if (idx == BIRD_Y_SECTIONS) {
                    idx = 0;
                }
            }
            used_sections |= (1 << idx);
            state->bird_random_heights[i] = idx;
        }
    }

    ASSERT(state->bird_height_idx < BIRD_Y_SECTIONS);

    float y_range = ((VERTICAL_FREEDOM * 2) / BIRD_Y_SECTIONS);
    float min_y = -VERTICAL_FREEDOM + (y_range * state->bird_random_heights[state->bird_height_idx]);
    float max_y = min_y + y_range;

    state->bird_height_idx++;
    if (state->bird_height_idx == BIRD_Y_SECTIONS) {
        state->bird_height_idx = 0;
    }

    return randf(min_y, max_y);
}

void setup_bird_by_level(Bird *bird) {
    bird->state = BIRD_STATE_ALIVE;
    bird->position.x = RESET_RIGHT;
    bird->position.y = get_next_bird_spawn_y_position();

    int spawn_type = -1;
    int total_weight = 0;
    for (int i = 0; i < SPAWN_TYPES_TOTAL; i++) {
        total_weight += state->bird_spawn_weights[i];
    }
    int random_value = GetRandomValue(0, total_weight - 1);
    int cumulative_weight = 0;
    for (int i = 0; i < SPAWN_TYPES_TOTAL; i++) {
        cumulative_weight += state->bird_spawn_weights[i];
        if (random_value < cumulative_weight) {
            spawn_type = i;
            break;
        }
    }
    ASSERT(spawn_type >= 0);

    bird->anim_time = 0.0f;

    bird->alive.move_speed = bird_move_speed_by_y_position(bird->position.y);
    bird->alive.current_sprite = 0;
    bird->alive.damage_timer = 0.0f;

    switch (spawn_type) {
        case SPAWN_TYPE_REGULAR: {
            bird->type = BIRD_TYPE_REGULAR;
            bird->alive.health = 1;
            bird->alive.collision_bounds.x = 0.1f;
            bird->alive.collision_bounds.y = 0.1f;
        } break;
        case SPAWN_TYPE_UMBRELLA: {
            bool bird_y_under_center = (bird->position.y < 0);
            bird->type = bird_y_under_center
                ? BIRD_TYPE_UMBRELLA_ABOVE
                : BIRD_TYPE_UMBRELLA_UNDER;
            bird->alive.health = 1;
            bird->alive.collision_bounds.x = 0.1f;
            bird->alive.collision_bounds.y = 0.15f;
        } break;
        case SPAWN_TYPE_GIANT: {
            bird->type = BIRD_TYPE_GIANT;
            bird->alive.health = 2;
            bird->alive.collision_bounds.x = 0.2f;
            bird->alive.collision_bounds.y = 0.2f;
        } break;
        case 40000: {
            bird->type = BIRD_TYPE_TINY;
            bird->alive.health = 1;
            bird->alive.collision_bounds.x = 0.05f;
            bird->alive.collision_bounds.y = 0.05f;
        } break;
    }
}

void bird_stationary_add(int index, Vector2 position) {
    ASSERT(index < BIRD_STATIONARY_CAPACITY);
    Bird *stationary = get_available_bird();
    state->bird_stationaries[index] = (BirdStationary){
        .bird = stationary,
        .position = position
    };
    stationary->state = BIRD_STATE_ALIVE;
    stationary->type = BIRD_TYPE_STATIONARY;
    stationary->position.x = position.x;
    if (position.y < 0) {
        stationary->position.y = -1.1f;
    } else {
        stationary->position.y = 1.1f;
    }
    stationary->alive.health = 1;
    stationary->alive.collision_bounds.x = 0.1f;
    stationary->alive.collision_bounds.y = 0.1f;
}

void remove_bird_circle(BirdCircle *circle) {
    for (int i = 0; i < circle->count; i++) {
        bird_set_available(circle->birds[i]);
    }
    for (int i = 0; i < state->bird_circle_count; i++) {
        if (&state->bird_circles[i] == circle) {
            state->bird_circles[i] = state->bird_circles[state->bird_circle_count - 1];
            state->bird_circle_count--;
            break;
        }
    }
}

void initialize_birds() {
    // TODO: LoadShaderFromMemory
    state->bird_shader = LoadShader(0, "./src/shaders/bird.glsl");
    state->bird_shader_palette_location = GetShaderLocation(state->bird_shader, "palette");
    state->bird_damage_shader = LoadShader(0, "./src/shaders/bird_damage.glsl");

    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_OUTLINE] = V3(0.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_BODY] = V3(1.0f,1.0f,1.0f);
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_BEAK1] = V3(1.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_BEAK2] = V3(0.75f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_EYES] = V3(1.0f,1.0f,1.0f);
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_GORE1] = V3(1.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_GORE2] = V3(0.75f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_GORE3] = V3(0.5f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_UMBRELLA_HANDLE] = V3(0.5f,0.25f,0.0f);
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_UMBRELLA_CANOPY1] = V3(1.0f,0.5f,0.0f);
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_UMBRELLA_CANOPY2] = V3(0.0f,0.5f,1.0f);
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_EXPLOSION1] = V3(1.0f,1.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_EXPLOSION2] = V3(1.0f,0.5f,0.0f);
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_EXPLOSION3] = V3(1.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_EXPLOSION4] = V3(0.75f,0.75f,0.75f);
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_MONITOR_FRAME] = V3_UNIFORM(0.25f);
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_MONITOR_FRAME_LIT] = V3_UNIFORM(0.5f);
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_MONITOR_FRAME_SHADOW] = V3_UNIFORM(0.125f);
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_MONITOR_SCREEN] = V3_UNIFORM(0.0f);
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_MONITOR_SCREEN_LINE] = V3_UNIFORM(1.0f);
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_MONITOR_BIRD_OUTLINE] = V3_UNIFORM(0.0f);
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_MONITOR_BIRD_BODY] = V3(0.0f,0.5f,1.0f);
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_MONITOR_BIRD_BEAK] = V3(1.0f,0.5f,0.0f);
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_MONITOR_BIRD_EYE] = V3_UNIFORM(1.0f);

    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_OUTLINE] = V3(0.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_BODY] = V3(1.0f,1.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_BEAK1] = V3(1.0f,0.5f,0.0f);
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_BEAK2] = V3(0.75f,0.375f,0.0f);
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_EYES] = V3(1.0f,1.0f,1.0f);
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_GORE1] = V3(1.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_GORE2] = V3(0.75f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_GORE3] = V3(0.5f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_UMBRELLA_HANDLE] = V3(0.5f,0.25f,0.0f);
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_UMBRELLA_CANOPY1] = V3(1.0f,1.0f,1.0f);
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_UMBRELLA_CANOPY2] = V3(1.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_EXPLOSION1] = V3(1.0f,1.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_EXPLOSION2] = V3(1.0f,0.5f,0.0f);
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_EXPLOSION3] = V3(1.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_EXPLOSION4] = V3(0.75f,0.75f,0.75f);
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_MONITOR_FRAME] = V3_UNIFORM(0.25f);
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_MONITOR_FRAME_LIT] = V3_UNIFORM(0.5f);
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_MONITOR_FRAME_SHADOW] = V3_UNIFORM(0.125f);
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_MONITOR_SCREEN] = V3_UNIFORM(0.0f);
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_MONITOR_SCREEN_LINE] = V3_UNIFORM(1.0f);
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_MONITOR_BIRD_OUTLINE] = V3_UNIFORM(0.0f);
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_MONITOR_BIRD_BODY] = V3(1.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_MONITOR_BIRD_BEAK] = V3(1.0f,1.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_MONITOR_BIRD_EYE] = V3_UNIFORM(1.0f);
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_WHEEL_TIRE1] = V3(0.25f,0.25f,0.25f);
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_WHEEL_TIRE1] = V3_UNIFORM(0.5f);
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_WHEEL_TIRE2] = V3_UNIFORM(0.25f);
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_WHEEL_RIM1] = V3(1.0f,0.5f,0.0f);
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_WHEEL_RIM2] = V3(0.75f,0.375f,0.5f);

    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_OUTLINE] = V3(0.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_BODY] = V3(0.5f,0.25f,0.0f);
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_BEAK1] = V3(1.0f,1.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_BEAK2] = V3(0.75f,0.75f,0.0f);
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_EYES] = V3(1.0f,1.0f,1.0f);
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_GORE1] = V3(1.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_GORE2] = V3(0.75f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_GORE3] = V3(0.5f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_UMBRELLA_HANDLE] = V3(0.75f,0.37f,0.0f);
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_UMBRELLA_CANOPY1] = V3(1.0f,0.5f,0.75f);
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_UMBRELLA_CANOPY2] = V3(0.0f,1.0f,1.0f);
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_EXPLOSION1] = V3(1.0f,1.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_EXPLOSION2] = V3(1.0f,0.5f,0.0f);
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_EXPLOSION3] = V3(1.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_EXPLOSION4] = V3(0.75f,0.75f,0.75f);
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_MONITOR_FRAME] = V3_UNIFORM(0.25f);
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_MONITOR_FRAME_LIT] = V3_UNIFORM(0.5f);
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_MONITOR_FRAME_SHADOW] = V3_UNIFORM(0.125f);
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_MONITOR_SCREEN] = V3_UNIFORM(0.0f);
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_MONITOR_SCREEN_LINE] = V3_UNIFORM(1.0f);
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_MONITOR_BIRD_OUTLINE] = V3_UNIFORM(0.0f);
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_MONITOR_BIRD_BODY] = V3(1.0f,1.0f,1.0f);
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_MONITOR_BIRD_BEAK] = V3(0.0f,1.0f,1.0f);
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_MONITOR_BIRD_EYE] = V3_UNIFORM(1.0f);

    state->bird_palette[BIRD_PALETTE_GREEN][BIRD_PALETTE_IDX_OUTLINE] = V3(0.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_GREEN][BIRD_PALETTE_IDX_BODY] = V3(0.0f,1.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_GREEN][BIRD_PALETTE_IDX_BEAK1] = V3(1.0f,1.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_GREEN][BIRD_PALETTE_IDX_BEAK2] = V3(0.75f,0.75f,0.0f);
    state->bird_palette[BIRD_PALETTE_GREEN][BIRD_PALETTE_IDX_EYES] = V3(1.0f,1.0f,1.0f);
    state->bird_palette[BIRD_PALETTE_GREEN][BIRD_PALETTE_IDX_GORE1] = V3(1.0f,0.0f,1.0f);
    state->bird_palette[BIRD_PALETTE_GREEN][BIRD_PALETTE_IDX_GORE2] = V3(0.75f,0.0f,0.75f);
    state->bird_palette[BIRD_PALETTE_GREEN][BIRD_PALETTE_IDX_GORE3] = V3(0.5f,0.0f,0.5f);
    state->bird_palette[BIRD_PALETTE_GREEN][BIRD_PALETTE_IDX_UMBRELLA_HANDLE] = V3(0.5f,0.25f,0.0f);
    state->bird_palette[BIRD_PALETTE_GREEN][BIRD_PALETTE_IDX_UMBRELLA_CANOPY1] = V3(1.0f,1.0f,1.0f);
    state->bird_palette[BIRD_PALETTE_GREEN][BIRD_PALETTE_IDX_UMBRELLA_CANOPY2] = V3(1.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_GREEN][BIRD_PALETTE_IDX_EXPLOSION1] = V3(1.0f,1.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_GREEN][BIRD_PALETTE_IDX_EXPLOSION2] = V3(1.0f,0.5f,0.0f);
    state->bird_palette[BIRD_PALETTE_GREEN][BIRD_PALETTE_IDX_EXPLOSION3] = V3(1.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_GREEN][BIRD_PALETTE_IDX_EXPLOSION4] = V3(0.75f,0.75f,0.75f);
    state->bird_palette[BIRD_PALETTE_GREEN][BIRD_PALETTE_IDX_MONITOR_FRAME] = V3_UNIFORM(0.25f);
    state->bird_palette[BIRD_PALETTE_GREEN][BIRD_PALETTE_IDX_MONITOR_FRAME_LIT] = V3_UNIFORM(0.5f);
    state->bird_palette[BIRD_PALETTE_GREEN][BIRD_PALETTE_IDX_MONITOR_FRAME_SHADOW] = V3_UNIFORM(0.125f);
    state->bird_palette[BIRD_PALETTE_GREEN][BIRD_PALETTE_IDX_MONITOR_SCREEN] = V3_UNIFORM(0.0f);
    state->bird_palette[BIRD_PALETTE_GREEN][BIRD_PALETTE_IDX_MONITOR_SCREEN_LINE] = V3_UNIFORM(1.0f);
    state->bird_palette[BIRD_PALETTE_GREEN][BIRD_PALETTE_IDX_MONITOR_BIRD_OUTLINE] = V3_UNIFORM(0.0f);
    state->bird_palette[BIRD_PALETTE_GREEN][BIRD_PALETTE_IDX_MONITOR_BIRD_BODY] = V3(1.0f,0.0f,1.0f);
    state->bird_palette[BIRD_PALETTE_GREEN][BIRD_PALETTE_IDX_MONITOR_BIRD_BEAK] = V3(1.0f,1.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_GREEN][BIRD_PALETTE_IDX_MONITOR_BIRD_EYE] = V3_UNIFORM(1.0f);
    state->bird_palette[BIRD_PALETTE_GREEN][BIRD_PALETTE_IDX_SHARK_BODY] = V3_UNIFORM(0.25f);
    state->bird_palette[BIRD_PALETTE_GREEN][BIRD_PALETTE_IDX_SHARK_BODY2] = V3_UNIFORM(0.5f);
    state->bird_palette[BIRD_PALETTE_GREEN][BIRD_PALETTE_IDX_SHARK_EYES] = V3(1.0f,1.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_GREEN][BIRD_PALETTE_IDX_SHARK_TEETH] = V3(1.0f,1.0f,0.0f);

    state->bird_palette[BIRD_PALETTE_VOID][BIRD_PALETTE_IDX_OUTLINE] = V3(1.0f,1.0f,1.0f);
    state->bird_palette[BIRD_PALETTE_VOID][BIRD_PALETTE_IDX_BODY] = V3(0.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_VOID][BIRD_PALETTE_IDX_BEAK1] = V3(0.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_VOID][BIRD_PALETTE_IDX_BEAK2] = V3(0.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_VOID][BIRD_PALETTE_IDX_EYES] = V3(0.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_VOID][BIRD_PALETTE_IDX_GORE1] = V3(1.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_VOID][BIRD_PALETTE_IDX_GORE2] = V3(0.75f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_VOID][BIRD_PALETTE_IDX_GORE3] = V3(0.5f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_VOID][BIRD_PALETTE_IDX_UMBRELLA_HANDLE] = V3(0.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_VOID][BIRD_PALETTE_IDX_UMBRELLA_CANOPY1] = V3(0.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_VOID][BIRD_PALETTE_IDX_UMBRELLA_CANOPY2] = V3(0.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_VOID][BIRD_PALETTE_IDX_EXPLOSION1] = V3(1.0f,1.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_VOID][BIRD_PALETTE_IDX_EXPLOSION2] = V3(1.0f,0.5f,0.0f);
    state->bird_palette[BIRD_PALETTE_VOID][BIRD_PALETTE_IDX_EXPLOSION3] = V3(1.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_VOID][BIRD_PALETTE_IDX_EXPLOSION4] = V3(0.75f,0.75f,0.75f);
    state->bird_palette[BIRD_PALETTE_VOID][BIRD_PALETTE_IDX_MONITOR_FRAME] = V3_UNIFORM(0.0f);
    state->bird_palette[BIRD_PALETTE_VOID][BIRD_PALETTE_IDX_MONITOR_FRAME_LIT] = V3_UNIFORM(1.0f);
    state->bird_palette[BIRD_PALETTE_VOID][BIRD_PALETTE_IDX_MONITOR_FRAME_SHADOW] = V3_UNIFORM(1.0f);
    state->bird_palette[BIRD_PALETTE_VOID][BIRD_PALETTE_IDX_MONITOR_SCREEN] = V3(0.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_VOID][BIRD_PALETTE_IDX_MONITOR_SCREEN_LINE] = V3(1.0f,1.0f,1.0f);
    state->bird_palette[BIRD_PALETTE_VOID][BIRD_PALETTE_IDX_MONITOR_BIRD_OUTLINE] = V3_UNIFORM(0.0f);
    state->bird_palette[BIRD_PALETTE_VOID][BIRD_PALETTE_IDX_MONITOR_BIRD_BODY] = V3_UNIFORM(1.0f);
    state->bird_palette[BIRD_PALETTE_VOID][BIRD_PALETTE_IDX_MONITOR_BIRD_BEAK] = V3_UNIFORM(0.5f);
    state->bird_palette[BIRD_PALETTE_VOID][BIRD_PALETTE_IDX_MONITOR_BIRD_EYE] = V3_UNIFORM(1.0f);

    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_OUTLINE] = V3(0.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_BODY] = V3(1.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_BEAK1] = V3(1.0f,1.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_BEAK2] = V3(0.75f,0.75f,0.0f);
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_EYES] = V3(1.0f,1.0f,1.0f);
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_GORE1] = V3(0.0f,1.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_GORE2] = V3(0.0f,0.75f,0.0f);
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_GORE3] = V3(0.0f,0.5f,0.0f);
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_UMBRELLA_HANDLE] = V3(0.5f,0.25f,0.0f);
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_UMBRELLA_CANOPY1] = V3(1.0f,1.0f,1.0f);
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_UMBRELLA_CANOPY2] = V3(0.0f,1.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_EXPLOSION1] = V3(1.0f,1.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_EXPLOSION2] = V3(1.0f,0.5f,0.0f);
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_EXPLOSION3] = V3(1.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_EXPLOSION4] = V3(0.75f,0.75f,0.75f);
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_MONITOR_FRAME] = V3(0.25f,0.25f,0.25f);
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_MONITOR_FRAME_LIT] = V3(0.5f,0.5f,0.5f);
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_MONITOR_FRAME_SHADOW] = V3(0.125f,0.125f,0.125f);
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_MONITOR_SCREEN] = V3(1.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_MONITOR_SCREEN_LINE] = V3(1.0f,1.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_MONITOR_BIRD_OUTLINE] = V3_UNIFORM(0.0f);
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_MONITOR_BIRD_BODY] = V3_UNIFORM(0.25f);
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_MONITOR_BIRD_BEAK] = V3(0.5f,0.5f,0.5f);
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_MONITOR_BIRD_EYE] = V3(1.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_WHEEL_TIRE1] = V3_UNIFORM(0.5f);
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_WHEEL_TIRE2] = V3_UNIFORM(0.25f);
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_WHEEL_RIM1] = V3(1.0f,1.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_WHEEL_RIM2] = V3(0.75f,0.75f,0.5f);

    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_OUTLINE] = V3(0.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_BODY] = V3(0.25f,0.25f,0.25f);
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_BEAK1] = V3(0.5f,0.5f,0.5f);
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_BEAK2] = V3(0.25f,0.25f,0.25f);
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_EYES] = V3(1.0f,1.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_GORE1] = V3(1.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_GORE2] = V3(0.75f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_GORE3] = V3(0.5f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_UMBRELLA_HANDLE] = V3(0.5f,0.25f,0.0f);
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_UMBRELLA_CANOPY1] = V3(1.0f,1.0f,1.0f);
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_UMBRELLA_CANOPY2] = V3(1.0f,0.5f,0.0f);
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_EXPLOSION1] = V3(1.0f,1.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_EXPLOSION2] = V3(1.0f,0.5f,0.0f);
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_EXPLOSION3] = V3(1.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_EXPLOSION4] = V3(0.75f,0.75f,0.75f);
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_MONITOR_FRAME] = V3(0.25f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_MONITOR_FRAME_LIT] = V3(0.5f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_MONITOR_FRAME_SHADOW] = V3(0.125f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_MONITOR_SCREEN] = V3(0.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_MONITOR_SCREEN_LINE] = V3(1.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_MONITOR_BIRD_OUTLINE] = V3_UNIFORM(0.0f);
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_MONITOR_BIRD_BODY] = V3(1.0f,0.5f,0.0f);
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_MONITOR_BIRD_BEAK] = V3(0.5f,0.5f,0.5f);
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_MONITOR_BIRD_EYE] = V3(1.0f,1.0f,0.0f);

    state->bird_palette[BIRD_PALETTE_BLUE][BIRD_PALETTE_IDX_OUTLINE] = V3_UNIFORM(0.0f);
    state->bird_palette[BIRD_PALETTE_BLUE][BIRD_PALETTE_IDX_BODY] = V3(0.0f,0.5f,1.0f);
    state->bird_palette[BIRD_PALETTE_BLUE][BIRD_PALETTE_IDX_BEAK1] = V3(1.0f,0.5f,0.0f);
    state->bird_palette[BIRD_PALETTE_BLUE][BIRD_PALETTE_IDX_BEAK2] = V3(0.75f,0.375f,0.0f);
    state->bird_palette[BIRD_PALETTE_BLUE][BIRD_PALETTE_IDX_EYES] = V3_UNIFORM(1.0f);
    state->bird_palette[BIRD_PALETTE_BLUE][BIRD_PALETTE_IDX_GORE1] = V3(1.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_BLUE][BIRD_PALETTE_IDX_GORE2] = V3(0.75f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_BLUE][BIRD_PALETTE_IDX_GORE3] = V3(0.5f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_BLUE][BIRD_PALETTE_IDX_UMBRELLA_HANDLE] = V3(0.5f,0.25f,0.0f);
    state->bird_palette[BIRD_PALETTE_BLUE][BIRD_PALETTE_IDX_UMBRELLA_CANOPY1] = V3(1.0f,0.5f,0.0f);
    state->bird_palette[BIRD_PALETTE_BLUE][BIRD_PALETTE_IDX_UMBRELLA_CANOPY2] = V3(0.5f,0.0f,1.0f);
    state->bird_palette[BIRD_PALETTE_BLUE][BIRD_PALETTE_IDX_EXPLOSION1] = V3(1.0f,1.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_BLUE][BIRD_PALETTE_IDX_EXPLOSION2] = V3(1.0f,0.5f,0.0f);
    state->bird_palette[BIRD_PALETTE_BLUE][BIRD_PALETTE_IDX_EXPLOSION3] = V3(1.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_BLUE][BIRD_PALETTE_IDX_EXPLOSION4] = V3(0.75f,0.75f,0.75f);
    state->bird_palette[BIRD_PALETTE_BLUE][BIRD_PALETTE_IDX_MONITOR_FRAME] = V3_UNIFORM(0.25f);
    state->bird_palette[BIRD_PALETTE_BLUE][BIRD_PALETTE_IDX_MONITOR_FRAME_LIT] = V3_UNIFORM(0.5f);
    state->bird_palette[BIRD_PALETTE_BLUE][BIRD_PALETTE_IDX_MONITOR_FRAME_SHADOW] = V3_UNIFORM(0.125f);
    state->bird_palette[BIRD_PALETTE_BLUE][BIRD_PALETTE_IDX_MONITOR_SCREEN] = V3_UNIFORM(0.0f);
    state->bird_palette[BIRD_PALETTE_BLUE][BIRD_PALETTE_IDX_MONITOR_SCREEN_LINE] = V3_UNIFORM(1.0f);
    state->bird_palette[BIRD_PALETTE_BLUE][BIRD_PALETTE_IDX_MONITOR_BIRD_OUTLINE] = V3_UNIFORM(0.0f);
    state->bird_palette[BIRD_PALETTE_BLUE][BIRD_PALETTE_IDX_MONITOR_BIRD_BODY] = V3_UNIFORM(1.0f);
    state->bird_palette[BIRD_PALETTE_BLUE][BIRD_PALETTE_IDX_MONITOR_BIRD_BEAK] = V3(1.0f,0.0f,0.0f);
    state->bird_palette[BIRD_PALETTE_BLUE][BIRD_PALETTE_IDX_MONITOR_BIRD_EYE] = V3_UNIFORM(1.0f);
    state->bird_palette[BIRD_PALETTE_BLUE][BIRD_PALETTE_IDX_SHARK_BODY] = V3(0.9f,0.9f,1.0f);
    state->bird_palette[BIRD_PALETTE_BLUE][BIRD_PALETTE_IDX_SHARK_BODY2] = V3(0.5f,0.5f,1.0f);
    state->bird_palette[BIRD_PALETTE_BLUE][BIRD_PALETTE_IDX_SHARK_EYES] = V3(1.0f,1.0f,1.0f);
    state->bird_palette[BIRD_PALETTE_BLUE][BIRD_PALETTE_IDX_SHARK_TEETH] = V3(1.0f,1.0f,1.0f);
}

void clean_bird_circles() {
    for (int i = 0; i < BIRD_CIRCLE_CAPACITY; i++) {
        state->bird_circles[i] = (BirdCircle){0};
    }
    state->bird_circle_count = 0;
}

void destroy_all_birds() {
    restore_all_reserved_birds();
    clean_bird_circles();
    for (int i = 0; i < BIRD_CAPACITY; i++) {
        Bird *b = &state->birds[i];
        if (b->state == BIRD_STATE_ALIVE) {
            bird_death_setup(b, (Vector2){0}, 1.0f);
        }
    }
}

void reset_birds() {
    for (int i = 0; i < BIRD_CAPACITY; i++) {
        bird_set_available(&state->birds[i]);
    }
    clean_bird_circles();
}

void unload_bird_shaders() {
    UnloadShader(state->bird_shader);
    UnloadShader(state->bird_damage_shader);
}

int find_available_boss_slot() {
    for (int i = 0; i < BOSS_CAPACITY; i++) {
        if (state->bosses[i].bird == NULL) {
            return i;
        }
    }
    return -1;
}

bool bird_boss_exists() {
    for (int i = 0; i < BOSS_CAPACITY; i++) {
        if (state->bosses[i].bird != NULL) {
            return true;
        }
    }
    return false;
}

bool bird_boss_defeated() {
    for (int i = 0; i < BOSS_CAPACITY; i++) {
        if (state->bosses[i].bird == NULL) {
            continue;
        }
        if (state->bosses[i].bird->state == BIRD_STATE_ALIVE) {
            return false;
        }
    }
    return true;
}

Boss *find_boss_by_bird(Bird *bird) {
    for (int i = 0; i < BOSS_CAPACITY; i++) {
        if (state->bosses[i].bird == bird) {
            return &state->bosses[i];
        }
    }
    return NULL;
}

void spawn_wheel_bird(const char *name, int health, Vector2 position, int init_state) {
    int boss_slot = find_available_boss_slot();
    ASSERT(boss_slot >= 0);

    Bird *wheel_bird = get_available_bird_in_reverse_order();
    wheel_bird->state = BIRD_STATE_ALIVE;
    wheel_bird->type = BIRD_TYPE_WHEELS;
    wheel_bird->position = position;
    wheel_bird->alive.health = has_flag(state->flags, FLAG_BIRD_SHARK_HEALTH_1) ? 1 : health;
    wheel_bird->alive.collision_bounds.x = 0.15f;
    wheel_bird->alive.collision_bounds.y = 0.15f;

    if (init_state == BOSS_STATE_RIGHT) {
        wheel_bird->flip_sprite = true;
    }

    state->bosses[boss_slot] = (Boss){
        .bird = wheel_bird,
        .state = init_state,
        .full_health = health,
    };

    state->bosses_name = name;
}

void spawn_1_wheel_bird(const char *name, int health) {
    spawn_wheel_bird(name, health, (Vector2){RESET_RIGHT,0.0f}, BOSS_STATE_LEFT);
}

void spawn_2_wheel_birds(const char *name, int health) {
    spawn_wheel_bird(name, health, (Vector2){RESET_RIGHT, 0.6f}, BOSS_STATE_LEFT);
    spawn_wheel_bird(name, health, (Vector2){RESET_LEFT, -0.6f}, BOSS_STATE_RIGHT);
}

void render_wheel_bird_line(Bird *bird, Color color) {
    Boss *boss = find_boss_by_bird(bird);
    ASSERT(boss != NULL);

    float half_wheel_bird_height = ((2.0/GAME_HEIGHT_RATIO) / (AREA_TEXTURE_SIZE/sprite_rectangles[SPRITE_WHEEL_BIRD1].height)) / 2;
    float line_y = boss->bird->position.y - half_wheel_bird_height;
    DrawLineEx(
        to_pixel_position(state, (Vector2){ 1.0f, line_y }),
        to_pixel_position(state, (Vector2){ -1.0f, line_y }),
        5, color
    );
}

void spawn_bird_shark(const char *name, int health) {
    int boss_slot = find_available_boss_slot();
    ASSERT(boss_slot >= 0);

    Bird *shark = get_available_bird_in_reverse_order();
    shark->state = BIRD_STATE_ALIVE;
    shark->type = BIRD_TYPE_SHARK;
    shark->position.x = RESET_RIGHT;
    shark->position.y = BOSS_MAX_Y;
    shark->alive.health = has_flag(state->flags, FLAG_BIRD_SHARK_HEALTH_1) ? 1 : health;
    shark->alive.collision_bounds.x = 0.2f;
    shark->alive.collision_bounds.y = 0.2f;

    state->bosses[boss_slot] = (Boss){
        .bird = shark,
        .state = BOSS_STATE_LEFT,
        .full_health = health,
    };

    state->bosses_name = name;
}

void update_birds() {
    bool is_global_state_suitable_for_birds = state->global_state == GLOBAL_STATE_GAME;
    if (is_global_state_suitable_for_birds) {
        state->current_level.bird_timer -= state->delta_time;
    }

    for (int i = 0; i < BIRD_CAPACITY; i++) {
        Bird *bird = &state->birds[i];

        switch (bird->state) {
            case BIRD_STATE_AVAILABLE: {
                if (!is_global_state_suitable_for_birds) {
                    break;
                }

                bool get_more_birds = state->current_level.bird_timer <= 0.0f;
                if (!get_more_birds) {
                    break;
                }

                bool monitor_or_bomb_exists = (
                    state->bird_monitor != NULL &&
                    ( state->bird_monitor->type == BIRD_TYPE_MONITOR || state->bird_monitor->type == BIRD_TYPE_BOMB )
                );

                if (!monitor_or_bomb_exists && GetRandomValue(0, 9) == 0) {
                    // bomb bird is the less preferable version of the bird monitor
                    // it first appears as a random replacement for the bird monitor in the industrial place
                    if (GetRandomValue(0, state->level_idx / AREA_GREEN) == 0) {
                        state->bird_monitor = get_available_bird();
                        state->bird_monitor->state = BIRD_STATE_ALIVE;
                        state->bird_monitor->type = BIRD_TYPE_MONITOR;
                        state->bird_monitor->position.x = RESET_RIGHT;
                        state->bird_monitor->position.y = get_next_bird_spawn_y_position();
                        state->bird_monitor->alive.move_speed = bird_move_speed_by_y_position(state->bird_monitor->position.y);
                        state->bird_monitor->alive.health = 3;
                        state->bird_monitor->alive.collision_bounds.x = 0.1f;
                        state->bird_monitor->alive.collision_bounds.y = 0.15f;
                        state->current_level.bird_timer = state->current_level.bird_frequency;
                    } else {
                        state->bird_monitor = get_available_bird();
                        state->bird_monitor->state = BIRD_STATE_ALIVE;
                        state->bird_monitor->type = BIRD_TYPE_BOMB;
                        state->bird_monitor->position.x = RESET_RIGHT;
                        state->bird_monitor->position.y = get_next_bird_spawn_y_position();
                        state->bird_monitor->alive.move_speed = bird_move_speed_by_y_position(state->bird_monitor->position.y);
                        state->bird_monitor->alive.health = 1;
                        state->bird_monitor->alive.collision_bounds.x = 0.1f;
                        state->bird_monitor->alive.collision_bounds.y = 0.15f;
                        state->current_level.bird_timer = state->current_level.bird_frequency;
                    }
                } else if (state->bird_circle_count < BIRD_CIRCLE_CAPACITY && GetRandomValue(0, 9) == 0) {
                    BirdCircle *circle = &state->bird_circles[state->bird_circle_count++];
                    *circle = (BirdCircle){0};
                    circle->count = GetRandomValue(2, BIRD_CIRCLE_MAX_BIRDS);
                    circle->radius = circle->count * 0.04f;

                    for (int i = 0; i < circle->count; i++) {
                        Bird *b = get_available_bird();
                        b->state = BIRD_STATE_ALIVE;
                        b->type = BIRD_TYPE_RESERVED;
                        b->position = (Vector2){ RESET_RIGHT, 0 };
                        b->alive.health = 1;
                        b->alive.collision_bounds.x = 0.1f;
                        b->alive.collision_bounds.y = 0.1f;
                        b->alive.current_sprite = GetRandomValue(0, ANIM_SPRITE_AMOUNT - 1);
                        b->anim_time = randf(0.0f, GAME_ANIM_SPEED);
                        circle->birds[i] = b;
                    }

                    circle->position.x = RESET_RIGHT + circle->radius;
                    float max_y = 0.9f - circle->radius;
                    circle->position.y = get_next_bird_spawn_y_position();
                    if (circle->position.y < (-max_y)) {
                        circle->position.y = (-max_y);
                    } else if (circle->position.y > max_y) {
                        circle->position.y = max_y;
                    }
                    circle->move_speed = bird_move_speed_by_y_position(circle->position.y);
                    circle->angular_speed = 20.0f / circle->count;

                    state->current_level.bird_timer = (state->current_level.bird_frequency * circle->count);
                } else {
                    setup_bird_by_level(bird);
                    state->current_level.bird_timer = state->current_level.bird_frequency;
                }
            } break;
            case BIRD_STATE_ALIVE: {
                bird->anim_time += state->delta_time;
                if (bird->anim_time > GAME_ANIM_SPEED) {
                    bird->anim_time = 0.0f;
                    bird->alive.current_sprite = (bird->alive.current_sprite + 1) % ANIM_SPRITE_AMOUNT;
                }
                if (bird->alive.damage_timer > 0.0f) {
                    bird->alive.damage_timer -= state->delta_time;
                }
                switch (bird->type) {
                    default: {
                        bird->position.x -= bird->alive.move_speed * state->delta_time;
                        if (bird->position.x < RESET_LEFT) {
                            bird_set_available(bird);
                        }
                    } break;
                    case BIRD_TYPE_STATIONARY: {
                        Vector2 stationary_position = {0};
                        for (int j = 0; j < BIRD_STATIONARY_CAPACITY; j++) {
                            if (state->bird_stationaries[j].bird == bird) {
                                stationary_position = state->bird_stationaries[j].position;
                                break;
                            }
                        }
                        bird->position.y += (stationary_position.y - bird->position.y) * state->delta_time * 5.0f;
                    } break;
                    case BIRD_TYPE_RESERVED: {
                        // position is controlled by the bird circle
                        // the bird circle is destroyed when all its birds are completely dead
                        if (bird->position.x < RESET_LEFT) {
                            bird->state = BIRD_STATE_RESERVED;
                        }
                    } break;
                    case BIRD_TYPE_WHEELS: {
                        Boss *wheel_bird = find_boss_by_bird(bird);
                        ASSERT(wheel_bird != NULL);

                        float move = (BOSS_SPEED * state->delta_time);
                        switch (wheel_bird->state) {
                            case BOSS_STATE_LEFT:
                                bird->position.x -= move;
                                if (bird->position.x > BOSS_MIN_X) break;
                                bird->position.x = BOSS_MIN_X;
                                wheel_bird->state = BOSS_STATE_RIGHT;
                                bird->flip_sprite = true;
                                break;
                            case BOSS_STATE_RIGHT:
                                bird->position.x += move;
                                if (bird->position.x < BOSS_MAX_X) break;
                                bird->position.x = BOSS_MAX_X;
                                wheel_bird->state = BOSS_STATE_LEFT;
                                bird->flip_sprite = false;
                                break;
                        }
                    } break;
                    case BIRD_TYPE_SHARK: {
                        Boss *shark = find_boss_by_bird(bird);
                        ASSERT(shark != NULL);

                        float move = (BOSS_SPEED * state->delta_time);
                        switch (shark->state) {
                            case BOSS_STATE_LEFT:
                                if (bird->position.x > 0 && (bird->position.x - move) < 0 && (GetRandomValue(0,1) == 0)) {
                                    bird->position.x = 0;
                                    shark->state = BOSS_STATE_DOWN;
                                    break;
                                }
                                bird->position.x -= move;
                                if (bird->position.x > BOSS_MIN_X) break;
                                bird->position.x = BOSS_MIN_X;
                                shark->state = BOSS_STATE_DOWN;
                                bird->flip_sprite = true;
                                break;
                            case BOSS_STATE_DOWN:
                                if (bird->position.y > 0 && (bird->position.y - move) < 0 && (GetRandomValue(0,1) == 0)) {
                                    bird->position.y = 0;
                                    shark->state = BOSS_STATE_RIGHT;
                                    break;
                                }
                                bird->position.y -= move;
                                if (bird->position.y > BOSS_MIN_Y) break;
                                bird->position.y = BOSS_MIN_Y;
                                shark->state = BOSS_STATE_RIGHT;
                                break;
                            case BOSS_STATE_RIGHT:
                                if (bird->position.x < 0 && (bird->position.x + move) > 0 && (GetRandomValue(0,1) == 0)) {
                                    bird->position.x = 0;
                                    shark->state = BOSS_STATE_UP;
                                    break;
                                }
                                bird->position.x += move;
                                if (bird->position.x < BOSS_MAX_X) break;
                                bird->position.x = BOSS_MAX_X;
                                shark->state = BOSS_STATE_UP;
                                bird->flip_sprite = false;
                                break;
                            case BOSS_STATE_UP:
                                if (bird->position.y < 0 && (bird->position.y + move) > 0 && (GetRandomValue(0,1) == 0)) {
                                    bird->position.y = 0;
                                    shark->state = BOSS_STATE_LEFT;
                                    break;
                                }
                                bird->position.y += move;
                                if (bird->position.y < BOSS_MAX_Y) break;
                                bird->position.y = BOSS_MAX_Y;
                                shark->state = BOSS_STATE_LEFT;
                                break;
                        }
                    } break;
                }
            } break;
            case BIRD_STATE_DEAD: {
                bool all_off_screen = true;
                float out_of_bounds = 0.2;
                for (int j = 0; j < BIRD_DEATH_PARTS; j++) {
                    if (
                        bird->dead.velocities[j].y > DEATH_GROUND_BOUNCE_VELOCITY_THRESHOLD &&
                        bird->dead.positions[j].y < GAME_GROUND_Y
                    ) {
                        bird->dead.velocities[j].y *= DEATH_GROUND_BOUNCE_VELOCITY;
                    } else if (
                        bird->dead.positions[j].x < (-1.0f - out_of_bounds) ||
                        bird->dead.positions[j].x > (1.0f + out_of_bounds) ||
                        bird->dead.positions[j].y < (-1.0f - out_of_bounds)
                        /*  ignore the case when the part is above the screen because it is
                            funny if the bloody bodypart comes back down */
                    ) {
                        continue;
                    }
                    all_off_screen = false;
                    bird->dead.velocities[j].y -= GRAVITY * state->delta_time;
                    bird->dead.positions[j].x += bird->dead.velocities[j].x * state->delta_time;
                    bird->dead.positions[j].y += bird->dead.velocities[j].y * state->delta_time;
                    bird->dead.rotations[j] += bird->dead.angular_velocities[j] * state->delta_time;
                }
                update_death_animation(bird);

                bool trailing_fireball_condition = bird->type == BIRD_TYPE_MONITOR ? update_trailing_fireballs() : true;
                bool animation_completed = bird->dead.anim_idx >= bird->dead.anim_sprite_amount;
                if (all_off_screen && trailing_fireball_condition && animation_completed) {
                    if (bird->type == BIRD_TYPE_RESERVED) {
                        bird->state = BIRD_STATE_RESERVED;
                    } else {
                        bird_set_available(bird);
                    }
                }
            } break;
            case BIRD_STATE_INHALED_BY_PORTAL: {
                Vector2 portal_position = portal_get_position(state);
                bool all_inside_portal = true;
                for (int j = 0; j < BIRD_DEATH_PARTS; j++) {
                    Vector2 direction = vec2_direction(bird->dead.positions[j], portal_position);
                    direction = vec2_normalized(direction.x, direction.y);
                    direction.x *= state->portal_power * state->delta_time;
                    direction.y *= state->portal_power * state->delta_time;

                    float velocity_loss = 1.0f - state->delta_time;
                    bird->dead.velocities[j].x *= velocity_loss;
                    bird->dead.velocities[j].y *= velocity_loss;

                    bool x_fully_inhaled = portal_inhale_object(
                        &(bird->dead.positions[j].x),
                        direction.x,
                        portal_position.x
                    );
                    bool y_fully_inhaled = portal_inhale_object(
                        &(bird->dead.positions[j].y),
                        direction.y,
                        portal_position.y
                    );

                    if (!x_fully_inhaled || !y_fully_inhaled) {
                        float x_velocity = bird->dead.velocities[j].x * state->delta_time;
                        float y_velocity = bird->dead.velocities[j].y * state->delta_time;
                        bird->dead.positions[j].x += x_velocity;
                        bird->dead.positions[j].y += y_velocity;
                        all_inside_portal = false;
                    }

                    bird->dead.rotations[j] += bird->dead.angular_velocities[j] * state->delta_time;
                }
                update_death_animation(bird);

                bool trailing_fireball_condition = bird->type == BIRD_TYPE_MONITOR ? update_trailing_fireballs() : true;
                if (all_inside_portal && trailing_fireball_condition) {
                    // set up bird to be exhaled by the portal
                    bird->position = portal_position;
                    bird_death_setup(bird, portal_position, DEATH_VELOCITY_MULTIPLIER_PORTAL_EXHALE);
                    bird->state = BIRD_STATE_INSIDE_PORTAL;
                }
            } break;
            case BIRD_STATE_EXHALED_BY_PORTAL: {
                bool all_off_screen = true;
                float out_of_bounds = 0.2;
                for (int j = 0; j < BIRD_DEATH_PARTS; j++) {
                    float velocity_gain = 1.0f + (PORTAL_EMIT_MULTIPLIER * state->delta_time);
                    bird->dead.velocities[j].x *= velocity_gain;
                    bird->dead.velocities[j].y *= velocity_gain;
                    if (
                        bird->dead.velocities[j].y > DEATH_GROUND_BOUNCE_VELOCITY_THRESHOLD &&
                        bird->dead.positions[j].y < GAME_GROUND_Y
                    ) {
                        bird->dead.velocities[j].y *= DEATH_GROUND_BOUNCE_VELOCITY;
                    } else if (
                        bird->dead.positions[j].x < (-1.0f - out_of_bounds) ||
                        bird->dead.positions[j].x > (1.0f + out_of_bounds) ||
                        bird->dead.positions[j].y < (-1.0f - out_of_bounds) ||
                        bird->dead.positions[j].y > (1.0f + out_of_bounds)
                    ) {
                        continue;
                    }
                    all_off_screen = false;
                    bird->dead.positions[j].x += bird->dead.velocities[j].x * state->delta_time;
                    bird->dead.positions[j].y += bird->dead.velocities[j].y * state->delta_time;
                    bird->dead.rotations[j] += bird->dead.angular_velocities[j] * state->delta_time;
                }

                bool trailing_fireball_condition = bird->type == BIRD_TYPE_MONITOR ? update_trailing_fireballs() : true;
                if (all_off_screen && trailing_fireball_condition) {
                    bird_set_available(bird);
                }
            } break;
            case BIRD_STATE_RESERVED: {
                // The slot for this bird is reserved by something
                if (state->global_state != GLOBAL_STATE_GAME) {
                    bird_set_available(bird);
                }
            } break;
            default: break;
        }
    }

    for (int i = state->bird_circle_count - 1; i >= 0; i--) {
        BirdCircle *circle = &state->bird_circles[i];

        bool all_reserved = true;
        for (int j = 0; j < circle->count; j++) {
            if (circle->birds[j]->state != BIRD_STATE_RESERVED) {
                all_reserved = false;
                break;
            }
        }

        if (all_reserved) {
            remove_bird_circle(circle);
            continue;
        }

        float pi_x2 = (PI * 2);
        circle->position.x -= circle->move_speed * state->delta_time;
        circle->rotation += circle->angular_speed * state->delta_time;
        if (circle->rotation >= pi_x2) {
            circle->rotation -= pi_x2;
        }
        float bird_rotation_offset = pi_x2 / circle->count;
        for (int j = 0; j < circle->count; j++) {
            Bird *b = circle->birds[j];
            if (b->state != BIRD_STATE_ALIVE) continue;

            float circular_position = circle->rotation + (bird_rotation_offset * j);
            b->position = (Vector2) {
                circle->position.x + (cosf(circular_position) * circle->radius),
                circle->position.y + (sinf(circular_position) * circle->radius),
            };
        }
    }
}

BirdHit apply_damage_to_bird(Bird *bird, int damage, Vector2 from, float velocity_multiplier) {
    bird->alive.health -= damage;
    if (bird->alive.health <= 0) {
        if (!level_target_reached()) {
            state->portal_fuel++;
        }
        state->bird_multiplier++;

        ASSERT(bird->type < BIRD_TYPE_TOTAL);

        state->birds_destroyed[bird->type]++;

        switch (bird->type) {
            default: {
                Vector2 master_velocity = vec2_normalized(
                    bird->position.x - from.x,
                    bird->position.y - from.y
                );
                master_velocity.x *= velocity_multiplier;
                master_velocity.y *= velocity_multiplier;

                bird_death_setup(bird, master_velocity, velocity_multiplier);
                int sound_idx = GetRandomValue(0, DEATH_SOUND_AMOUNT - 1);
                PlaySound(state->sounds_death_splats[sound_idx]);
            } break;
            case BIRD_TYPE_BOMB: {
                bird_death_setup(bird, (Vector2){0}, EXPLOSIVE_VELOCITY_MULTIPLIER);
                PlaySound(state->sound_explosion);
                for (int i = 0; i < BIRD_CAPACITY; i++) {
                    Bird *other_bird = &state->birds[i];
                    if (other_bird->state != BIRD_STATE_ALIVE) {
                        // note that this will also catch the case where bird == other_bird
                        // because it is already marked dead
                        continue;
                    }
                    float x_distance = fabs(bird->position.x - other_bird->position.x) * GAME_WIDTH_RATIO;
                    float y_distance = fabs(bird->position.y - other_bird->position.y) * GAME_HEIGHT_RATIO;
                    float hypotenuse = sqrtf(x_distance*x_distance + y_distance*y_distance);
                    if (hypotenuse < GIANT_SPLASH_RADIUS) {
                        apply_damage_to_bird(other_bird, EXPLOSIVE_DAMAGE, bird->position, EXPLOSIVE_VELOCITY_MULTIPLIER);
                    }
                }
            } break;
            case BIRD_TYPE_MONITOR: {
                bird_death_setup(bird, (Vector2){0}, EXPLOSIVE_VELOCITY_MULTIPLIER);
                PlaySound(state->sound_explosion);

                for (int i = 0; i < BIRD_CAPACITY; i++) {
                    Bird *b = &state->birds[i];
                    if (b->state == BIRD_STATE_ALIVE) {
                        apply_damage_to_bird(b, EXPLOSIVE_DAMAGE, bird->position, EXPLOSIVE_VELOCITY_MULTIPLIER);
                    }
                }

                for (int i = 0; i < BIRD_STATIONARY_CAPACITY; i++) {
                    int half = BIRD_STATIONARY_CAPACITY / 2;
                    float left = -1.0f;
                    float width = 2.0f;
                    float space = width / (half + 1);
                    bird_stationary_add(i, (Vector2) {
                        left + space + (space * (i % half)),
                        (i < 4) ? 0.9f : -0.9f,
                    });
                }
            } break;
        }

        if (bird->type == BIRD_TYPE_TINY) {
            return (
                BIRD_HIT_DESTROYED |
                BIRD_HIT_PASS_THROUGH_ALLOWED
            );
        }

        return (
            BIRD_HIT_DESTROYED |
            BIRD_HIT_BOUNCE_ALLOWED |
            BIRD_HIT_PASS_THROUGH_ALLOWED
        );
    }

    bird->alive.damage_timer = DAMAGE_TIME;

    return BIRD_HIT_BOUNCE_ALLOWED;
}

BirdHit apply_damage_to_bird_from_player(Bird *bird) {
    switch (bird->type) {
        default: break;
        case BIRD_TYPE_UMBRELLA_ABOVE: {
            if (state->player.state == PLAYER_STATE_DOWN) {
                return BIRD_HIT_BOUNCE_ALLOWED;
            }
        } break;
        case BIRD_TYPE_UMBRELLA_UNDER: {
            if (state->player.state == PLAYER_STATE_UP) {
                return BIRD_HIT_BOUNCE_ALLOWED;
            }
        } break;
    }
    return apply_damage_to_bird(bird, PLAYER_DAMAGE, state->player.spinner_position, DEATH_VELOCITY_MULTIPLIER);
}

void give_alive_birds_to_portal() {
    Vector2 portal_position = portal_get_position(state);
    for (int i = 0; i < BIRD_CAPACITY; i++) {
        switch (state->birds[i].state) {
            default: continue;
            case BIRD_STATE_ALIVE: {
                Vector2 master_velocity = vec2_normalized(
                    state->birds[i].position.x - portal_position.x,
                    state->birds[i].position.y - portal_position.y
                );
                bird_death_setup(&state->birds[i], master_velocity, DEATH_VELOCITY_MULTIPLIER_PORTAL_INHALE);
            } break;
            case BIRD_STATE_DEAD: break;
        }
        state->birds[i].state = BIRD_STATE_INHALED_BY_PORTAL;
    }
}

Vector3 *bird_palette_by_area(int area) {
    switch (area) {
        default: ASSERT(false);
        case AREA_FOREST: return state->bird_palette[BIRD_PALETTE_WHITE];
        case AREA_MEADOWS: return state->bird_palette[BIRD_PALETTE_YELLOW];
        case AREA_MOUNTAINS: return state->bird_palette[BIRD_PALETTE_BROWN];
        case AREA_GREEN: return state->bird_palette[BIRD_PALETTE_GREEN];
        case AREA_VOID: return state->bird_palette[BIRD_PALETTE_VOID];
        case AREA_INDUSTRIAL: return state->bird_palette[BIRD_PALETTE_RED];
        case AREA_CASTLE: return state->bird_palette[BIRD_PALETTE_BLACK];
        case AREA_SKY: return state->bird_palette[BIRD_PALETTE_BLUE];
    }
}

void prepare_bird_shader(int area) {
    Vector3 *palette = bird_palette_by_area(area);
    ASSERT(palette != NULL);
    SetShaderValueV(
        state->bird_shader,
        state->bird_shader_palette_location,
        palette,
        SHADER_UNIFORM_VEC3,
        BIRD_PALETTE_IDX_TOTAL
    );
}

void render_birds() {
    for (int i = 0; i < BIRD_CAPACITY; i++) {
        Bird *bird = &state->birds[i];
        Vector2 scale = get_bird_scale(bird);
        switch (bird->state) {
            default: break;
            case BIRD_STATE_ALIVE: {
                Sprite bird_sprite;
                switch (bird->type) {
                    default: bird_sprite = SPRITE_BIRD1 + bird->alive.current_sprite; break;
                    case BIRD_TYPE_GIANT: bird_sprite = SPRITE_GIANT_BIRD1 + bird->alive.current_sprite; break;
                    case BIRD_TYPE_UMBRELLA_ABOVE:
                    case BIRD_TYPE_UMBRELLA_UNDER: bird_sprite = SPRITE_UMBRELLA_BIRD1 + bird->alive.current_sprite; break;
                    case BIRD_TYPE_BOMB: bird_sprite = SPRITE_BOMB_BIRD1 + bird->alive.current_sprite; break;
                    case BIRD_TYPE_MONITOR: bird_sprite = SPRITE_BIRDMONITOR1 + bird->alive.current_sprite; break;
                    case BIRD_TYPE_WHEELS: bird_sprite = SPRITE_WHEEL_BIRD1 + bird->alive.current_sprite; break;
                    case BIRD_TYPE_SHARK: bird_sprite = SPRITE_SHARK1 + bird->alive.current_sprite; break;
                }
                if (bird->alive.damage_timer > 0.0f) {
                    BeginShaderMode(state->bird_damage_shader);
                    if (bird->type == BIRD_TYPE_WHEELS) {
                        render_wheel_bird_line(bird, (Color){255,255,255,255});
                    }
                    atlas_draw(state, bird_sprite, bird->position, 0.0f, scale, OPAQUE);
                    EndShaderMode();
                } else {
                    BeginShaderMode(state->bird_shader);
                    switch (bird->type) {
                        default: {
                            atlas_draw(state, bird_sprite, bird->position, 0.0f, scale, OPAQUE);
                        } break;
                        case BIRD_TYPE_TINY: {
                            Vector2 tiny_scale = {
                                scale.x * 0.5f,
                                scale.y * 0.5f
                            };
                            atlas_draw(state, bird_sprite, bird->position, 0.0f, tiny_scale, OPAQUE);
                            break;
                        } break;
                        case BIRD_TYPE_UMBRELLA_ABOVE: {
                            atlas_draw(state, SPRITE_UMBRELLA, bird->position, 0.0f, scale, OPAQUE);
                            Vector2 offset_bird_position = {
                                bird->position.x,
                                bird->position.y - 0.03f
                            };
                            atlas_draw(state, bird_sprite, offset_bird_position, 0.0f, scale, OPAQUE);
                        } break;
                        case BIRD_TYPE_UMBRELLA_UNDER: {
                            float upside_down_rotation = 180.0f;
                            atlas_draw(state, SPRITE_UMBRELLA, bird->position, upside_down_rotation, scale, OPAQUE);
                            Vector2 offset_bird_position = {
                                bird->position.x,
                                bird->position.y + 0.03f
                            };
                            atlas_draw(state, bird_sprite, offset_bird_position, 0.0f, scale, OPAQUE);
                        } break;
                        case BIRD_TYPE_WHEELS: {
                            render_wheel_bird_line(bird, (Color){255,0,0,255});
                            atlas_draw(state, bird_sprite, bird->position, 0.0f, scale, OPAQUE);
                        } break;
                    }
                    EndShaderMode();
                }
                if (has_flag(state->flags, FLAG_SHOW_COLLISION_BOUNDS)) {
                    draw_bird_collision_bounds(bird);
                }
            } break;
            case BIRD_STATE_DEAD: {
                BeginShaderMode(state->bird_shader);
                death_animation_render(bird);
                Sprite sprites[BIRD_DEATH_PARTS];
                get_bird_death_part_sprites(sprites, bird->type);
                if (bird->type == BIRD_TYPE_MONITOR) {
                    render_trailing_fireballs();
                } else {
                    for (int j = 0; j < BIRD_DEATH_PARTS; j++) {
                        Sprite sprite = sprites[j];
                        float rotation = bird->dead.rotations[j];
                        atlas_draw(state, sprite, bird->dead.positions[j], rotation, scale, OPAQUE);
                    }
                }
                EndShaderMode();
            } break;
            case BIRD_STATE_INHALED_BY_PORTAL: {
                BeginShaderMode(state->bird_shader);
                death_animation_render(bird);
                render_bird_parts_with_portal_influence(bird);
                EndShaderMode();
            } break;
            case BIRD_STATE_EXHALED_BY_PORTAL: {
                int previous_area = state->area - 1;
                if (previous_area < 0) {
                    previous_area = AREA_TOTAL - 1;
                }
                prepare_bird_shader(previous_area);
                BeginShaderMode(state->bird_shader);
                render_bird_parts_with_portal_influence(bird);
                EndShaderMode();
                prepare_bird_shader(state->area);
            } break;
        }
    }
}

