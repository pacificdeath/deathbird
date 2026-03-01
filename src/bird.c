#define GRAVITY 4.0f
#define ANIM_SPRITE_AMOUNT 4
#define GORE_SPRITE_AMOUNT 2
#define DAMAGE_TIME 0.2f
#define DEATH_VELOCITY_MULTIPLIER 0.8f
#define EXPLOSIVE_VELOCITY_MULTIPLIER 1.3f
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

int bird_palette_by_area(int area) {
    switch (area) {
        default: ASSERT(false); // fallthrough
        case AREA_FOREST: return BIRD_PALETTE_WHITE;
        case AREA_MEADOWS: return BIRD_PALETTE_YELLOW;
        case AREA_MOUNTAINS: return BIRD_PALETTE_BROWN;
        case AREA_GREEN: return BIRD_PALETTE_GREEN;
        case AREA_VOID: return BIRD_PALETTE_VOID;
        case AREA_INDUSTRIAL: return BIRD_PALETTE_RED;
        case AREA_CASTLE: return BIRD_PALETTE_BLACK;
        case AREA_SKY: return BIRD_PALETTE_BLUE;
    }
}

Vector2 get_bird_scale(Bird *bird) {
    return (has_flag(bird->flags, BIRD_FLAG_FLIP_SPRITE) ? SCALE_X_FLIP : SCALE_NORMAL);
}

Vector2 get_collision_bounds_from_pixels(int px_x, int px_y) {
    return (Vector2){
        (float)px_x / 32.0f * 0.1f,
        (float)px_y / 32.0f * 0.1f,
    };
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

void draw_bird_collision_bounds(Bird *bird) {
    Vector2 px_bird_position = to_pixel_position(bird->position);
    Vector2 px_collision_bounds = to_pixel_size(bird->alive.collision_bounds);
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

void get_bird_death_part_sprites(int sprites[BIRD_DEATH_PARTS], int bird_type) {
    // sprites are added in rendering order
    int gore_sprite = SPRITE_BIRD_GORE1;
    for (int i = 0; i < BIRD_DEATH_GORE_PARTS; i += GORE_SPRITE_AMOUNT) {
        for (int j = 0; j < GORE_SPRITE_AMOUNT; j++) {
            int idx = i + j;

            ASSERT(idx < BIRD_DEATH_PARTS);

            sprites[idx] = gore_sprite + j;
        }
    }
    switch (bird_type) {
        default:
            ASSERT(false);
        case BIRD_TYPE_REGULAR:
        case BIRD_TYPE_STATIONARY:
            sprites[BIRD_DEATH_PARTS - 6] = SPRITE_BIRD_WING;
            sprites[BIRD_DEATH_PARTS - 5] = SPRITE_BIRD_BODY;
            sprites[BIRD_DEATH_PARTS - 4] = SPRITE_BIRD_WING;
            sprites[BIRD_DEATH_PARTS - 3] = SPRITE_BIRD_EYE;
            sprites[BIRD_DEATH_PARTS - 2] = SPRITE_BIRD_HEAD;
            sprites[BIRD_DEATH_PARTS - 1] = SPRITE_BIRD_EYE;
            break;
        case BIRD_TYPE_UMBRELLA_ABOVE:
        case BIRD_TYPE_UMBRELLA_UNDER:
            sprites[BIRD_DEATH_PARTS - 6] = SPRITE_BIRD_GORE1;
            sprites[BIRD_DEATH_PARTS - 5] = SPRITE_BIRD_GORE2;
            sprites[BIRD_DEATH_PARTS - 4] = SPRITE_BIRD_GORE1;
            sprites[BIRD_DEATH_PARTS - 3] = SPRITE_BIRD_EYE;
            sprites[BIRD_DEATH_PARTS - 2] = SPRITE_BIRD_EYE;
            sprites[BIRD_DEATH_PARTS - 1] = SPRITE_UMBRELLA;
            break;
        case BIRD_TYPE_GIANT:
            sprites[BIRD_DEATH_PARTS - 6] = SPRITE_GIANT_BIRD_WING;
            sprites[BIRD_DEATH_PARTS - 5] = SPRITE_GIANT_BIRD_BODY;
            sprites[BIRD_DEATH_PARTS - 4] = SPRITE_GIANT_BIRD_WING;
            sprites[BIRD_DEATH_PARTS - 3] = SPRITE_BIRD_EYE;
            sprites[BIRD_DEATH_PARTS - 2] = SPRITE_GIANT_BIRD_HEAD;
            sprites[BIRD_DEATH_PARTS - 1] = SPRITE_BIRD_EYE;
            break;
        case BIRD_TYPE_BOMB:
            sprites[BIRD_DEATH_PARTS - 6] = SPRITE_BIRD_EYE;
            sprites[BIRD_DEATH_PARTS - 5] = SPRITE_BIRD_GORE1;
            sprites[BIRD_DEATH_PARTS - 4] = SPRITE_BIRD_GORE2;
            sprites[BIRD_DEATH_PARTS - 3] = SPRITE_BIRD_GORE1;
            sprites[BIRD_DEATH_PARTS - 2] = SPRITE_BIRD_GORE2;
            sprites[BIRD_DEATH_PARTS - 1] = SPRITE_BIRD_EYE;
            break;
        case BIRD_TYPE_WHEELS:
            sprites[BIRD_DEATH_PARTS - 6] = SPRITE_WHEEL;
            sprites[BIRD_DEATH_PARTS - 5] = SPRITE_BIRD_GORE1;
            sprites[BIRD_DEATH_PARTS - 4] = SPRITE_BIRD_GORE2;
            sprites[BIRD_DEATH_PARTS - 3] = SPRITE_BIRD_GORE1;
            sprites[BIRD_DEATH_PARTS - 2] = SPRITE_BIRD_GORE2;
            sprites[BIRD_DEATH_PARTS - 1] = SPRITE_WHEEL;
            break;
        case BIRD_TYPE_SHARK:
            sprites[BIRD_DEATH_PARTS - 6] = SPRITE_SHARK_WING;
            sprites[BIRD_DEATH_PARTS - 5] = SPRITE_SHARK_BODY;
            sprites[BIRD_DEATH_PARTS - 4] = SPRITE_SHARK_WING;
            sprites[BIRD_DEATH_PARTS - 3] = SPRITE_SHARK_EYE;
            sprites[BIRD_DEATH_PARTS - 2] = SPRITE_SHARK_HEAD;
            sprites[BIRD_DEATH_PARTS - 1] = SPRITE_SHARK_EYE;
            break;
        case BIRD_TYPE_MONITOR:
            sprites[BIRD_DEATH_PARTS - 6] = SPRITE_BIRDMONITOR1;
            sprites[BIRD_DEATH_PARTS - 5] = SPRITE_BIRDMONITOR2;
            sprites[BIRD_DEATH_PARTS - 4] = SPRITE_BIRDMONITOR3;
            sprites[BIRD_DEATH_PARTS - 3] = SPRITE_BIRDMONITOR4;
            sprites[BIRD_DEATH_PARTS - 2] = SPRITE_BIRDMONITOR1;
            sprites[BIRD_DEATH_PARTS - 1] = SPRITE_BIRDMONITOR2;
            break;
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
        int blood_sprite = bird->dead.anim_start_sprite + bird->dead.anim_idx;
        atlas_draw(blood_sprite, bird->position, 0.0f, get_bird_scale(bird), OPAQUE);
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
        case BIRD_TYPE_WHEELS:
        case BIRD_TYPE_MONITOR: {
            bird->dead.anim_start_sprite = SPRITE_EXPLOSION1;
            bird->dead.anim_sprite_amount = 6;
        } break;
    }
}

void world_update_defaults() {
    for (int i = 0; i < SPAWN_TYPE_TOTAL; i++) {
        state->bird_spawn_weights[i] = 0;
    }
}

void transition_to_world(int area) {
    state->next_area = area;
}

void update_world_by_score() {
    switch (state->score) {
        case 0:
            world_update_defaults();
            state->bird_spawn_weights[SPAWN_TYPE_REGULAR] = 95;
            state->bird_spawn_weights[SPAWN_TYPE_CIRCLE] = 4;
            state->bird_spawn_weights[SPAWN_TYPE_MONITOR] = 1;
            break;
        case 1:
            state->bird_start_count = GetTime();
            break;
        case 150:
            world_update_defaults();
            transition_to_world(AREA_MEADOWS);
            state->bird_spawn_weights[SPAWN_TYPE_REGULAR] = 85;
            state->bird_spawn_weights[SPAWN_TYPE_UMBRELLA] = 10;
            state->bird_spawn_weights[SPAWN_TYPE_UMBRELLA_CIRCLE] = 4;
            state->bird_spawn_weights[SPAWN_TYPE_MONITOR] = 1;
            break;
        case 300:
            world_update_defaults();
            transition_to_world(AREA_MOUNTAINS);
            state->bird_spawn_weights[SPAWN_TYPE_REGULAR] = 85;
            state->bird_spawn_weights[SPAWN_TYPE_GIANT] = 10;
            state->bird_spawn_weights[SPAWN_TYPE_GIANT_CIRCLE] = 4;
            state->bird_spawn_weights[SPAWN_TYPE_MONITOR] = 1;
            break;
        case 450:
            world_update_defaults();
            transition_to_world(AREA_GREEN);
            state->bird_spawn_weights[SPAWN_TYPE_REGULAR] = 85;
            state->bird_spawn_weights[SPAWN_TYPE_BOMB] = 10;
            state->bird_spawn_weights[SPAWN_TYPE_BOMB_CIRCLE] = 4;
            state->bird_spawn_weights[SPAWN_TYPE_MONITOR] = 1;
            break;
        default:
            // no changes, early return to avoid updating total spawn weight
            return;
    }

    state->bird_total_spawn_weight = 0;
    for (int i = 0; i < SPAWN_TYPE_TOTAL; i++) {
        state->bird_total_spawn_weight += state->bird_spawn_weights[i];
    }
}

float bird_move_speed_by_y_position(float y) {
    return 0.2f + (y + 0.75f) * 0.15f;
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
    float min_y = -VERTICAL_FREEDOM + (y_range * (float)state->bird_random_heights[state->bird_height_idx]);
    float max_y = min_y + y_range;

    state->bird_height_idx++;
    if (state->bird_height_idx == BIRD_Y_SECTIONS) {
        state->bird_height_idx = 0;
    }

    return randf(min_y, max_y);
}

void bird_set_defaults(Bird *bird) {
    bird->state = BIRD_STATE_ALIVE;
    bird->palette_index = bird_palette_by_area(state->area);

    bird->position.x = RESET_RIGHT;
    bird->position.y = get_next_bird_spawn_y_position();

    bird->anim_time = 0.0f;

    bird->alive.move_speed = bird_move_speed_by_y_position(bird->position.y);
    bird->alive.current_sprite = 0;
    bird->alive.damage_timer = 0.0f;
}

void set_regular_bird(Bird *bird) {
    bird_set_defaults(bird);
    bird->type = BIRD_TYPE_REGULAR;
    bird->alive.health = 1;
    bird->alive.collision_bounds = get_collision_bounds_from_pixels(32, 32);
}

void set_umbrella_bird(Bird *bird) {
    bird_set_defaults(bird);
    bird->type = (bird->position.y < 0) ? BIRD_TYPE_UMBRELLA_ABOVE : BIRD_TYPE_UMBRELLA_UNDER;
    bird->alive.health = 1;
    bird->alive.collision_bounds = get_collision_bounds_from_pixels(32, 48);
}

void set_giant_bird(Bird *bird) {
    bird_set_defaults(bird);
    bird->type = BIRD_TYPE_GIANT;
    bird->alive.health = 2;
    bird->alive.collision_bounds = get_collision_bounds_from_pixels(64, 64);
}

void set_bomb_bird(Bird *bird) {
    bird_set_defaults(bird);
    bird->type = BIRD_TYPE_BOMB;
    bird->alive.health = 1;
    bird->alive.collision_bounds = get_collision_bounds_from_pixels(32, 48);
}

void set_bird_monitor(Bird *bird) {
    bird_set_defaults(bird);
    bird->type = BIRD_TYPE_MONITOR;
    bird->alive.health = 3;
    bird->alive.collision_bounds = get_collision_bounds_from_pixels(32, 48);
}

float spawn_bird_circle(int spawn_type) {
    ASSERT(state->bird_circle_count <= BIRD_CIRCLE_CAPACITY);

    if (state->bird_circle_count == BIRD_CIRCLE_CAPACITY) {
        return 0;
    }

    BirdCircle *circle = &state->bird_circles[state->bird_circle_count++];
    *circle = (BirdCircle){0};
    circle->count = GetRandomValue(2, BIRD_CIRCLE_MAX_BIRDS);
    circle->radius = (float)circle->count * 0.05f;

    for (int i = 0; i < circle->count; i++) {
        Bird *bird = get_available_bird();
        bird->flags |= BIRD_FLAG_IS_RESERVED;
        if (GetRandomValue(0,4) == 0) {
            switch (spawn_type) {
                default: ASSERT(false);
                case SPAWN_TYPE_CIRCLE:
                    set_regular_bird(bird);
                    break;
                case SPAWN_TYPE_UMBRELLA_CIRCLE:
                    set_umbrella_bird(bird);
                    break;
                case SPAWN_TYPE_GIANT_CIRCLE:
                    set_giant_bird(bird);
                    break;
                case SPAWN_TYPE_BOMB_CIRCLE:
                    set_bomb_bird(bird);
                    break;
            }
        } else {
            set_regular_bird(bird);
        }
        circle->birds[i] = bird;
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
    circle->angular_speed = BIRD_CIRCLE_ANGULAR_BASE_SPEED / (float)circle->count;
    if (GetRandomValue(0,1) == 0) {
        circle->angular_speed *= -1;
    }

    return BIRD_FREQUENCY * (float)circle->count;
}

float spawn_bird(Bird *bird) {
    int spawn_type = -1;
    int random_value = GetRandomValue(0, state->bird_total_spawn_weight - 1);
    int cumulative_weight = 0;
    for (int i = 0; i < SPAWN_TYPE_TOTAL; i++) {
        cumulative_weight += state->bird_spawn_weights[i];
        if (random_value < cumulative_weight) {
            spawn_type = i;
            break;
        }
    }
    ASSERT(spawn_type >= 0);

    switch (spawn_type) {
        default:
            ASSERT(false);
        case SPAWN_TYPE_REGULAR:
            set_regular_bird(bird);
            break;
        case SPAWN_TYPE_MONITOR:
            set_bird_monitor(bird);
            break;
        case SPAWN_TYPE_UMBRELLA:
            set_umbrella_bird(bird);
            break;
        case SPAWN_TYPE_BOMB:
            set_bomb_bird(bird);
            break;
        case SPAWN_TYPE_GIANT:
            set_giant_bird(bird);
            break;
        case SPAWN_TYPE_CIRCLE:
        case SPAWN_TYPE_UMBRELLA_CIRCLE:
        case SPAWN_TYPE_GIANT_CIRCLE:
        case SPAWN_TYPE_BOMB_CIRCLE:
            return spawn_bird_circle(spawn_type);
    }

    return BIRD_FREQUENCY;
}

BirdStationary bird_stationary_find(Bird *bird) {
    for (int j = 0; j < BIRD_STATIONARY_CAPACITY; j++) {
        if (state->bird_stationaries[j].bird == bird) {
            return state->bird_stationaries[j];
        }
    }
    ASSERT(false);
    return (BirdStationary){0};
}

void bird_stationary_add(int index, Vector2 position) {
    ASSERT(index < BIRD_STATIONARY_CAPACITY);
    Bird *stationary = get_available_bird();
    state->bird_stationaries[index] = (BirdStationary){
        .bird = stationary,
        .position = position
    };
    stationary->type = BIRD_TYPE_STATIONARY;
    stationary->state = BIRD_STATE_ALIVE;
    stationary->palette_index = bird_palette_by_area(state->area);
    stationary->position.x = position.x;
    if (position.y < 0) {
        stationary->position.y = -1.1f;
    } else {
        stationary->position.y = 1.1f;
    }
    stationary->alive.health = 1;
    stationary->alive.collision_bounds = get_collision_bounds_from_pixels(32, 32);
}

void set_palette_pixel() {
}

#define BIRD_PALETTE_SET(palette, index, rgb)\
    palette_data[BIRD_PALETTE_##palette][BIRD_PALETTE_IDX_##index] = GetColor(0x##rgb##FF)

void initialize_birds() {
    // TODO: LoadShaderFromMemory
    state->bird_shader = LoadShader(0, "./src/shaders/bird.glsl");
    state->bird_damage_shader = LoadShader(0, "./src/shaders/bird_damage.glsl");
    state->bird_shader_palette_tex_location = GetShaderLocation(state->bird_shader, BIRD_SHADER_LOCATION_PALETTE_TEX);
    state->bird_shader_palette_index_location = GetShaderLocation(state->bird_shader, BIRD_SHADER_LOCATION_PALETTE_INDEX);

    Color palette_data[AREA_TOTAL][BIRD_PALETTE_COLORS];

    for (int y = 0; y < AREA_TOTAL; y++) {
        for (int x = 0; x < BIRD_PALETTE_COLORS; x++) {
            palette_data[y][x] = GetColor(0xff00ffff);
        }
    }

    BIRD_PALETTE_SET(WHITE, OUTLINE, 000000);
    BIRD_PALETTE_SET(WHITE, BODY, FFFFFF);
    BIRD_PALETTE_SET(WHITE, BEAK1, FF0000);
    BIRD_PALETTE_SET(WHITE, BEAK2, C00000);
    BIRD_PALETTE_SET(WHITE, EYES, FFFFFF);
    BIRD_PALETTE_SET(WHITE, GORE1, FF0000);
    BIRD_PALETTE_SET(WHITE, GORE2, C00000);
    BIRD_PALETTE_SET(WHITE, GORE3, 800000);
    BIRD_PALETTE_SET(WHITE, UMBRELLA_HANDLE, 800400);
    BIRD_PALETTE_SET(WHITE, UMBRELLA_CANOPY1, FF8000);
    BIRD_PALETTE_SET(WHITE, UMBRELLA_CANOPY2, 0080FF);
    BIRD_PALETTE_SET(WHITE, EXPLOSION1, FFFF00);
    BIRD_PALETTE_SET(WHITE, EXPLOSION2, FF8000);
    BIRD_PALETTE_SET(WHITE, EXPLOSION3, FF0000);
    BIRD_PALETTE_SET(WHITE, EXPLOSION4, C0C0C0);
    BIRD_PALETTE_SET(WHITE, MONITOR_FRAME, 404040);
    BIRD_PALETTE_SET(WHITE, MONITOR_FRAME_LIT, 808080);
    BIRD_PALETTE_SET(WHITE, MONITOR_FRAME_SHADOW, 202020);
    BIRD_PALETTE_SET(WHITE, MONITOR_SCREEN, 000000);
    BIRD_PALETTE_SET(WHITE, MONITOR_SCREEN_LINE, FFFFFF);
    BIRD_PALETTE_SET(WHITE, MONITOR_BIRD_OUTLINE, 000000);
    BIRD_PALETTE_SET(WHITE, MONITOR_BIRD_BODY, 0080FF);
    BIRD_PALETTE_SET(WHITE, MONITOR_BIRD_BEAK, FF8000);
    BIRD_PALETTE_SET(WHITE, MONITOR_BIRD_EYE, FFFFFF);
    BIRD_PALETTE_SET(WHITE, WHEEL_TIRE1, 808080);
    BIRD_PALETTE_SET(WHITE, WHEEL_TIRE2, 040404);
    BIRD_PALETTE_SET(WHITE, WHEEL_RIM1, FF0000);
    BIRD_PALETTE_SET(WHITE, WHEEL_RIM2, C00000);

    BIRD_PALETTE_SET(YELLOW, OUTLINE, 000000);
    BIRD_PALETTE_SET(YELLOW, BODY, FFFF00);
    BIRD_PALETTE_SET(YELLOW, BEAK1, FF8000);
    BIRD_PALETTE_SET(YELLOW, BEAK2, C00600);
    BIRD_PALETTE_SET(YELLOW, EYES, FFFFFF);
    BIRD_PALETTE_SET(YELLOW, GORE1, FF0000);
    BIRD_PALETTE_SET(YELLOW, GORE2, C00000);
    BIRD_PALETTE_SET(YELLOW, GORE3, 800000);
    BIRD_PALETTE_SET(YELLOW, UMBRELLA_HANDLE, 800400);
    BIRD_PALETTE_SET(YELLOW, UMBRELLA_CANOPY1, FFFFFF);
    BIRD_PALETTE_SET(YELLOW, UMBRELLA_CANOPY2, FF0000);
    BIRD_PALETTE_SET(YELLOW, EXPLOSION1, FFFF00);
    BIRD_PALETTE_SET(YELLOW, EXPLOSION2, FF8000);
    BIRD_PALETTE_SET(YELLOW, EXPLOSION3, FF0000);
    BIRD_PALETTE_SET(YELLOW, EXPLOSION4, C0C0C0);
    BIRD_PALETTE_SET(YELLOW, MONITOR_FRAME, 404040);
    BIRD_PALETTE_SET(YELLOW, MONITOR_FRAME_LIT, 808080);
    BIRD_PALETTE_SET(YELLOW, MONITOR_FRAME_SHADOW, 202020);
    BIRD_PALETTE_SET(YELLOW, MONITOR_SCREEN, 000000);
    BIRD_PALETTE_SET(YELLOW, MONITOR_SCREEN_LINE, FFFFFF);
    BIRD_PALETTE_SET(YELLOW, MONITOR_BIRD_OUTLINE, 000000);
    BIRD_PALETTE_SET(YELLOW, MONITOR_BIRD_BODY, FF0000);
    BIRD_PALETTE_SET(YELLOW, MONITOR_BIRD_BEAK, FFFF00);
    BIRD_PALETTE_SET(YELLOW, MONITOR_BIRD_EYE, FFFFFF);
    BIRD_PALETTE_SET(YELLOW, WHEEL_TIRE1, 808080);
    BIRD_PALETTE_SET(YELLOW, WHEEL_TIRE2, 040404);
    BIRD_PALETTE_SET(YELLOW, WHEEL_RIM1, FF8000);
    BIRD_PALETTE_SET(YELLOW, WHEEL_RIM2, C00680);

    BIRD_PALETTE_SET(BROWN, OUTLINE, 000000);
    BIRD_PALETTE_SET(BROWN, BODY, 800400);
    BIRD_PALETTE_SET(BROWN, BEAK1, FFFF00);
    BIRD_PALETTE_SET(BROWN, BEAK2, C0C000);
    BIRD_PALETTE_SET(BROWN, EYES, FFFFFF);
    BIRD_PALETTE_SET(BROWN, GORE1, FF0000);
    BIRD_PALETTE_SET(BROWN, GORE2, C00000);
    BIRD_PALETTE_SET(BROWN, GORE3, 800000);
    BIRD_PALETTE_SET(BROWN, UMBRELLA_HANDLE, 0C0600);
    BIRD_PALETTE_SET(BROWN, UMBRELLA_CANOPY1, FF80C0);
    BIRD_PALETTE_SET(BROWN, UMBRELLA_CANOPY2, 00FFFF);
    BIRD_PALETTE_SET(BROWN, EXPLOSION1, FFFF00);
    BIRD_PALETTE_SET(BROWN, EXPLOSION2, FF8000);
    BIRD_PALETTE_SET(BROWN, EXPLOSION3, FF0000);
    BIRD_PALETTE_SET(BROWN, EXPLOSION4, C0C0C0);
    BIRD_PALETTE_SET(BROWN, MONITOR_FRAME, 404040);
    BIRD_PALETTE_SET(BROWN, MONITOR_FRAME_LIT, 808080);
    BIRD_PALETTE_SET(BROWN, MONITOR_FRAME_SHADOW, 202020);
    BIRD_PALETTE_SET(BROWN, MONITOR_SCREEN, 000000);
    BIRD_PALETTE_SET(BROWN, MONITOR_SCREEN_LINE, FFFFFF);
    BIRD_PALETTE_SET(BROWN, MONITOR_BIRD_OUTLINE, 000000);
    BIRD_PALETTE_SET(BROWN, MONITOR_BIRD_BODY, FFFFFF);
    BIRD_PALETTE_SET(BROWN, MONITOR_BIRD_BEAK, 00FFFF);
    BIRD_PALETTE_SET(BROWN, MONITOR_BIRD_EYE, FFFFFF);

    BIRD_PALETTE_SET(GREEN, OUTLINE, 000000);
    BIRD_PALETTE_SET(GREEN, BODY, 00FF00);
    BIRD_PALETTE_SET(GREEN, BEAK1, FFFF00);
    BIRD_PALETTE_SET(GREEN, BEAK2, C0C000);
    BIRD_PALETTE_SET(GREEN, EYES, FFFFFF);
    BIRD_PALETTE_SET(GREEN, GORE1, FF00FF);
    BIRD_PALETTE_SET(GREEN, GORE2, C000C0);
    BIRD_PALETTE_SET(GREEN, GORE3, 800080);
    BIRD_PALETTE_SET(GREEN, UMBRELLA_HANDLE, 800400);
    BIRD_PALETTE_SET(GREEN, UMBRELLA_CANOPY1, FFFFFF);
    BIRD_PALETTE_SET(GREEN, UMBRELLA_CANOPY2, FF0000);
    BIRD_PALETTE_SET(GREEN, EXPLOSION1, FFFF00);
    BIRD_PALETTE_SET(GREEN, EXPLOSION2, FF8000);
    BIRD_PALETTE_SET(GREEN, EXPLOSION3, FF0000);
    BIRD_PALETTE_SET(GREEN, EXPLOSION4, C0C0C0);
    BIRD_PALETTE_SET(GREEN, MONITOR_FRAME, 404040);
    BIRD_PALETTE_SET(GREEN, MONITOR_FRAME_LIT, 808080);
    BIRD_PALETTE_SET(GREEN, MONITOR_FRAME_SHADOW, 202020);
    BIRD_PALETTE_SET(GREEN, MONITOR_SCREEN, 000000);
    BIRD_PALETTE_SET(GREEN, MONITOR_SCREEN_LINE, FFFFFF);
    BIRD_PALETTE_SET(GREEN, MONITOR_BIRD_OUTLINE, 000000);
    BIRD_PALETTE_SET(GREEN, MONITOR_BIRD_BODY, FF00FF);
    BIRD_PALETTE_SET(GREEN, MONITOR_BIRD_BEAK, FFFF00);
    BIRD_PALETTE_SET(GREEN, MONITOR_BIRD_EYE, FFFFFF);
    BIRD_PALETTE_SET(GREEN, SHARK_BODY, 404040);
    BIRD_PALETTE_SET(GREEN, SHARK_BODY2, 808080);
    BIRD_PALETTE_SET(GREEN, SHARK_EYES, FFFF00);
    BIRD_PALETTE_SET(GREEN, SHARK_TEETH, FFFF00);

    BIRD_PALETTE_SET(VOID, OUTLINE, FFFFFF);
    BIRD_PALETTE_SET(VOID, BODY, 000000);
    BIRD_PALETTE_SET(VOID, BEAK1, 000000);
    BIRD_PALETTE_SET(VOID, BEAK2, 000000);
    BIRD_PALETTE_SET(VOID, EYES, 000000);
    BIRD_PALETTE_SET(VOID, GORE1, FF0000);
    BIRD_PALETTE_SET(VOID, GORE2, C00000);
    BIRD_PALETTE_SET(VOID, GORE3, 800000);
    BIRD_PALETTE_SET(VOID, UMBRELLA_HANDLE, 000000);
    BIRD_PALETTE_SET(VOID, UMBRELLA_CANOPY1, 000000);
    BIRD_PALETTE_SET(VOID, UMBRELLA_CANOPY2, 000000);
    BIRD_PALETTE_SET(VOID, EXPLOSION1, FFFF00);
    BIRD_PALETTE_SET(VOID, EXPLOSION2, FF8000);
    BIRD_PALETTE_SET(VOID, EXPLOSION3, FF0000);
    BIRD_PALETTE_SET(VOID, EXPLOSION4, C0C0C0);
    BIRD_PALETTE_SET(VOID, MONITOR_FRAME, 000000);
    BIRD_PALETTE_SET(VOID, MONITOR_FRAME_LIT, FFFFFF);
    BIRD_PALETTE_SET(VOID, MONITOR_FRAME_SHADOW, FFFFFF);
    BIRD_PALETTE_SET(VOID, MONITOR_SCREEN, 000000);
    BIRD_PALETTE_SET(VOID, MONITOR_SCREEN_LINE, FFFFFF);
    BIRD_PALETTE_SET(VOID, MONITOR_BIRD_OUTLINE, 000000);
    BIRD_PALETTE_SET(VOID, MONITOR_BIRD_BODY, FFFFFF);
    BIRD_PALETTE_SET(VOID, MONITOR_BIRD_BEAK, 808080);
    BIRD_PALETTE_SET(VOID, MONITOR_BIRD_EYE, FFFFFF);

    BIRD_PALETTE_SET(RED, OUTLINE, 000000);
    BIRD_PALETTE_SET(RED, BODY, FF0000);
    BIRD_PALETTE_SET(RED, BEAK1, FFFF00);
    BIRD_PALETTE_SET(RED, BEAK2, C0C000);
    BIRD_PALETTE_SET(RED, EYES, FFFFFF);
    BIRD_PALETTE_SET(RED, GORE1, 00FF00);
    BIRD_PALETTE_SET(RED, GORE2, 00C000);
    BIRD_PALETTE_SET(RED, GORE3, 008000);
    BIRD_PALETTE_SET(RED, UMBRELLA_HANDLE, 800400);
    BIRD_PALETTE_SET(RED, UMBRELLA_CANOPY1, FFFFFF);
    BIRD_PALETTE_SET(RED, UMBRELLA_CANOPY2, 00FF00);
    BIRD_PALETTE_SET(RED, EXPLOSION1, FFFF00);
    BIRD_PALETTE_SET(RED, EXPLOSION2, FF8000);
    BIRD_PALETTE_SET(RED, EXPLOSION3, FF0000);
    BIRD_PALETTE_SET(RED, EXPLOSION4, C0C0C0);
    BIRD_PALETTE_SET(RED, MONITOR_FRAME, 404040);
    BIRD_PALETTE_SET(RED, MONITOR_FRAME_LIT, 808080);
    BIRD_PALETTE_SET(RED, MONITOR_FRAME_SHADOW, 202020);
    BIRD_PALETTE_SET(RED, MONITOR_SCREEN, FF0000);
    BIRD_PALETTE_SET(RED, MONITOR_SCREEN_LINE, FFFF00);
    BIRD_PALETTE_SET(RED, MONITOR_BIRD_OUTLINE, 000000);
    BIRD_PALETTE_SET(RED, MONITOR_BIRD_BODY, 404040);
    BIRD_PALETTE_SET(RED, MONITOR_BIRD_BEAK, 808080);
    BIRD_PALETTE_SET(RED, MONITOR_BIRD_EYE, FF0000);
    BIRD_PALETTE_SET(RED, WHEEL_TIRE1, 808080);
    BIRD_PALETTE_SET(RED, WHEEL_TIRE2, 404040);
    BIRD_PALETTE_SET(RED, WHEEL_RIM1, FFFF00);
    BIRD_PALETTE_SET(RED, WHEEL_RIM2, C0C080);

    BIRD_PALETTE_SET(BLACK, OUTLINE, 000000);
    BIRD_PALETTE_SET(BLACK, BODY, 404040);
    BIRD_PALETTE_SET(BLACK, BEAK1, 808080);
    BIRD_PALETTE_SET(BLACK, BEAK2, 404040);
    BIRD_PALETTE_SET(BLACK, EYES, FFFF00);
    BIRD_PALETTE_SET(BLACK, GORE1, FF0000);
    BIRD_PALETTE_SET(BLACK, GORE2, C00000);
    BIRD_PALETTE_SET(BLACK, GORE3, 800000);
    BIRD_PALETTE_SET(BLACK, UMBRELLA_HANDLE, 800400);
    BIRD_PALETTE_SET(BLACK, UMBRELLA_CANOPY1, FFFFFF);
    BIRD_PALETTE_SET(BLACK, UMBRELLA_CANOPY2, FF8000);
    BIRD_PALETTE_SET(BLACK, EXPLOSION1, FFFF00);
    BIRD_PALETTE_SET(BLACK, EXPLOSION2, FF8000);
    BIRD_PALETTE_SET(BLACK, EXPLOSION3, FF0000);
    BIRD_PALETTE_SET(BLACK, EXPLOSION4, C0C0C0);
    BIRD_PALETTE_SET(BLACK, MONITOR_FRAME, 400000);
    BIRD_PALETTE_SET(BLACK, MONITOR_FRAME_LIT, 800000);
    BIRD_PALETTE_SET(BLACK, MONITOR_FRAME_SHADOW, 200000);
    BIRD_PALETTE_SET(BLACK, MONITOR_SCREEN, 000000);
    BIRD_PALETTE_SET(BLACK, MONITOR_SCREEN_LINE, FF0000);
    BIRD_PALETTE_SET(BLACK, MONITOR_BIRD_OUTLINE, 000000);
    BIRD_PALETTE_SET(BLACK, MONITOR_BIRD_BODY, FF8000);
    BIRD_PALETTE_SET(BLACK, MONITOR_BIRD_BEAK, 808080);
    BIRD_PALETTE_SET(BLACK, MONITOR_BIRD_EYE, FFFF00);

    BIRD_PALETTE_SET(BLUE, OUTLINE, 000000);
    BIRD_PALETTE_SET(BLUE, BODY, 0080FF);
    BIRD_PALETTE_SET(BLUE, BEAK1, FF8000);
    BIRD_PALETTE_SET(BLUE, BEAK2, C00600);
    BIRD_PALETTE_SET(BLUE, EYES, FFFFFF);
    BIRD_PALETTE_SET(BLUE, GORE1, FF0000);
    BIRD_PALETTE_SET(BLUE, GORE2, C00000);
    BIRD_PALETTE_SET(BLUE, GORE3, 800000);
    BIRD_PALETTE_SET(BLUE, UMBRELLA_HANDLE, 800400);
    BIRD_PALETTE_SET(BLUE, UMBRELLA_CANOPY1, FF8000);
    BIRD_PALETTE_SET(BLUE, UMBRELLA_CANOPY2, 8000FF);
    BIRD_PALETTE_SET(BLUE, EXPLOSION1, FFFF00);
    BIRD_PALETTE_SET(BLUE, EXPLOSION2, FF8000);
    BIRD_PALETTE_SET(BLUE, EXPLOSION3, FF0000);
    BIRD_PALETTE_SET(BLUE, EXPLOSION4, C0C0C0);
    BIRD_PALETTE_SET(BLUE, MONITOR_FRAME, 404040);
    BIRD_PALETTE_SET(BLUE, MONITOR_FRAME_LIT, 808080);
    BIRD_PALETTE_SET(BLUE, MONITOR_FRAME_SHADOW, 202020);
    BIRD_PALETTE_SET(BLUE, MONITOR_SCREEN, 000000);
    BIRD_PALETTE_SET(BLUE, MONITOR_SCREEN_LINE, FFFFFF);
    BIRD_PALETTE_SET(BLUE, MONITOR_BIRD_OUTLINE, 000000);
    BIRD_PALETTE_SET(BLUE, MONITOR_BIRD_BODY, FFFFFF);
    BIRD_PALETTE_SET(BLUE, MONITOR_BIRD_BEAK, FF0000);
    BIRD_PALETTE_SET(BLUE, MONITOR_BIRD_EYE, FFFFFF);
    BIRD_PALETTE_SET(BLUE, SHARK_BODY, E0E0E0);
    BIRD_PALETTE_SET(BLUE, SHARK_BODY2, 8080FF);
    BIRD_PALETTE_SET(BLUE, SHARK_EYES, FFFFFF);
    BIRD_PALETTE_SET(BLUE, SHARK_TEETH, FFFFFF);

    Image palette_image;
    palette_image.data = palette_data;
    palette_image.width = BIRD_PALETTE_COLORS;
    palette_image.height = AREA_TOTAL;
    palette_image.mipmaps = 1;
    palette_image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

    state->bird_palette_texture = LoadTextureFromImage(palette_image);

    SetTextureFilter(state->bird_palette_texture, TEXTURE_FILTER_POINT);
    SetTextureWrap(state->bird_palette_texture, TEXTURE_WRAP_CLAMP);

    SetShaderValueTexture(
        state->bird_shader,
        state->bird_shader_palette_tex_location,
        state->bird_palette_texture
    );

    update_world_by_score();
}

void clean_bird_circles() {
    for (int i = 0; i < BIRD_CIRCLE_CAPACITY; i++) {
        state->bird_circles[i] = (BirdCircle){0};
    }
    state->bird_circle_count = 0;
}

void destroy_all_birds() {
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

void birds_cleanup() {
    UnloadShader(state->bird_shader);
    UnloadShader(state->bird_damage_shader);
    UnloadTexture(state->bird_palette_texture);
}

void update_birds() {
    if (state->bird_start_count > 0) {
        double seconds_per_bird = (GetTime() - state->bird_start_count) / state->score;
        double predicted_time = seconds_per_bird * SCORE_TARGET;
        int minutes = (int)(predicted_time / 60.0);
        int seconds = (int)predicted_time % 60;
        state->the_time = TextFormat("%-2i minutes and %-2i seconds", minutes, seconds);
    }

    state->bird_timer -= state->delta_time;

    for (int i = 0; i < BIRD_CAPACITY; i++) {
        Bird *bird = &state->birds[i];

        switch (bird->state) {
            case BIRD_STATE_AVAILABLE: {
                bool get_more_birds = state->bird_timer <= 0.0f;
                if (!get_more_birds) {
                    break;
                }

                state->bird_timer = spawn_bird(bird);
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
                    default:
                        if (has_flag(bird->flags, BIRD_FLAG_IS_RESERVED)) {
                            // position is controlled by the bird circle
                            // the bird circle is destroyed when all its birds are completely dead
                            if (bird->position.x < RESET_LEFT) {
                                bird->state = BIRD_STATE_RESERVED;
                            }
                        } else {
                            bird->position.x -= bird->alive.move_speed * state->delta_time;
                            if (bird->position.x < RESET_LEFT) {
                                bird_set_available(bird);
                            }
                        }
                        break;
                    case BIRD_TYPE_STATIONARY: {
                        BirdStationary stationary = bird_stationary_find(bird);
                        bird->position.y += (stationary.position.y - bird->position.y) * state->delta_time * 5.0f;
                    } break;
                }
            } break;
            case BIRD_STATE_DEAD: {
                bool all_off_screen = true;
                float out_of_bounds = 0.2f;
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

                bool animation_completed = bird->dead.anim_idx >= bird->dead.anim_sprite_amount;
                if (all_off_screen && animation_completed) {
                    if (has_flag(bird->flags, BIRD_FLAG_IS_RESERVED)) {
                        bird->state = BIRD_STATE_RESERVED;
                    } else {
                        bird_set_available(bird);
                    }
                }
            } break;
            case BIRD_STATE_RESERVED:
                // The slot for this bird is reserved by something
                break;
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
            for (int j = 0; j < circle->count; j++) {
                bird_set_available(circle->birds[j]);
            }
            for (int j = 0; j < state->bird_circle_count; j++) {
                if (&state->bird_circles[j] == circle) {
                    state->bird_circles[j] = state->bird_circles[state->bird_circle_count - 1];
                    state->bird_circle_count--;
                    break;
                }
            }
            continue;
        }

        circle->position.x -= circle->move_speed * state->delta_time;
        circle->rotation += circle->angular_speed * state->delta_time;
        if (circle->rotation >= PI_X2) {
            circle->rotation -= PI_X2;
        }
        float bird_rotation_offset = PI_X2 / (float)circle->count;
        for (int j = 0; j < circle->count; j++) {
            Bird *b = circle->birds[j];
            if (b->state != BIRD_STATE_ALIVE) continue;

            float circular_position = circle->rotation + (bird_rotation_offset * (float)j);
            b->position = (Vector2) {
                circle->position.x + (cosf(circular_position) * circle->radius),
                circle->position.y + (sinf(circular_position) * circle->radius),
            };
        }
    }
}

int apply_damage_to_bird(Bird *bird, int damage, Vector2 from, float velocity_multiplier) {
    bird->alive.health -= damage;
    if (bird->alive.health <= 0) {
        state->score++;
        update_world_by_score();

        ASSERT(bird->type < BIRD_TYPE_TOTAL);

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
                    float x_distance = fabsf(bird->position.x - other_bird->position.x) * GAME_WIDTH_RATIO;
                    float y_distance = fabsf(bird->position.y - other_bird->position.y) * GAME_HEIGHT_RATIO;
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
                    float space = width / ((float)half + 1.0f);
                    float offset = 0.875f;
                    bird_stationary_add(i, (Vector2) {
                        left + space + (space * (float)(i % half)),
                        (i < half) ? offset : -offset,
                    });
                }

                state->bird_timer = 0.0f;
            } break;
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

int apply_damage_to_bird_from_player(Bird *bird) {
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

void set_bird_shader_palette_index(int palette_index) {
    SetShaderValue(
        state->bird_shader,
        state->bird_shader_palette_index_location,
        &palette_index,
        SHADER_UNIFORM_INT
    );
}

void render_birds() {
    for (int palette_index = 0; palette_index < BIRD_PALETTE_TOTAL; palette_index++) {
        set_bird_shader_palette_index(palette_index);

        for (int bird_index = 0; bird_index < BIRD_CAPACITY; bird_index++) {
            Bird *bird = &state->birds[bird_index];
            if (bird->palette_index != palette_index) {
                continue;
            }

            Vector2 scale = get_bird_scale(bird);
            switch (bird->state) {
                default:
                case BIRD_STATE_RESERVED:
                    break;
                case BIRD_STATE_ALIVE: {
                    int bird_sprite;
                    switch (bird->type) {
                        case BIRD_TYPE_REGULAR:
                        case BIRD_TYPE_STATIONARY:
                            bird_sprite = SPRITE_BIRD1;
                            break;
                        case BIRD_TYPE_GIANT:
                            bird_sprite = SPRITE_GIANT_BIRD1;
                            break;
                        case BIRD_TYPE_UMBRELLA_ABOVE:
                        case BIRD_TYPE_UMBRELLA_UNDER:
                            bird_sprite = SPRITE_UMBRELLA_BIRD1;
                            break;
                        case BIRD_TYPE_BOMB:
                            bird_sprite = SPRITE_BOMB_BIRD1;
                            break;
                        case BIRD_TYPE_MONITOR:
                            bird_sprite = SPRITE_BIRDMONITOR1;
                            break;
                        case BIRD_TYPE_WHEELS:
                            bird_sprite = SPRITE_WHEEL_BIRD1;
                            break;
                        case BIRD_TYPE_SHARK:
                            bird_sprite = SPRITE_SHARK1;
                            break;
                        default:
                            ASSERT(false);
                            bird_sprite = 0;
                            break;
                    }
                    bird_sprite += bird->alive.current_sprite;

                    if (bird->alive.damage_timer > 0.0f) {
                        BeginShaderMode(state->bird_damage_shader);
                        atlas_draw(bird_sprite, bird->position, bird->alive.rotation, scale, OPAQUE);
                        EndShaderMode();
                    } else {
                        BeginShaderMode(state->bird_shader);
                        switch (bird->type) {
                            default: {
                                atlas_draw(bird_sprite, bird->position, bird->alive.rotation, scale, OPAQUE);
                            } break;
                            case BIRD_TYPE_UMBRELLA_ABOVE: {
                                atlas_draw(SPRITE_UMBRELLA, bird->position, 0.0f, scale, OPAQUE);
                                Vector2 offset_bird_position = {
                                    bird->position.x,
                                    bird->position.y - 0.03f
                                };
                                atlas_draw(bird_sprite, offset_bird_position, 0.0f, scale, OPAQUE);
                            } break;
                            case BIRD_TYPE_UMBRELLA_UNDER: {
                                float upside_down_rotation = 180.0f;
                                atlas_draw(SPRITE_UMBRELLA, bird->position, upside_down_rotation, scale, OPAQUE);
                                Vector2 offset_bird_position = {
                                    bird->position.x,
                                    bird->position.y + 0.03f
                                };
                                atlas_draw(bird_sprite, offset_bird_position, bird->alive.rotation, scale, OPAQUE);
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
                    int sprites[BIRD_DEATH_PARTS];
                    get_bird_death_part_sprites(sprites, bird->type);
                    for (int j = 0; j < BIRD_DEATH_PARTS; j++) {
                        int sprite = sprites[j];
                        float rotation = bird->dead.rotations[j];
                        atlas_draw(sprite, bird->dead.positions[j], rotation, scale, OPAQUE);
                    }
                    EndShaderMode();
                } break;
            }
        }
    }
}

