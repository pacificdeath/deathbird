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

static void birds_draw_collision_bounds(State *state, Bird *bird) {
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

static void get_bird_death_part_sprites(Sprite sprites[BIRD_DEATH_PARTS], BirdType bird_type) {
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
    default:
    case BIRD_TYPE_REGULAR: {
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
        sprites[BIRD_DEATH_PARTS - 6] = SPRITE_BIRD_GORE1;
        sprites[BIRD_DEATH_PARTS - 5] = SPRITE_BIRD_GORE2;
        sprites[BIRD_DEATH_PARTS - 4] = SPRITE_BIRD_GORE1;
        sprites[BIRD_DEATH_PARTS - 3] = SPRITE_BIRD_GORE2;
        sprites[BIRD_DEATH_PARTS - 2] = SPRITE_BIRD_EYE;
        sprites[BIRD_DEATH_PARTS - 1] = SPRITE_BIRD_EYE;
    } break;
    }
}

static void death_animation_update(State* state, Bird *bird) {
    if (bird->dead.anim_idx < bird->dead.anim_sprite_amount) {
        bird->anim_time += state->delta_time;
        if (bird->anim_time > GAME_ANIM_SPEED) {
            bird->anim_time = 0.0f;
            bird->dead.anim_idx++;
        }
    }
}

static void death_animation_render(State *state, Bird *bird) {
    if (bird->dead.anim_idx < bird->dead.anim_sprite_amount) {
        Sprite blood_sprite = bird->dead.anim_start_sprite + bird->dead.anim_idx;
        atlas_draw(state, blood_sprite, bird->position, 0.0f, 1.0f, OPAQUE);
    }
}

static void bird_death_setup(Bird *bird, Vector2 master_velocity, float multiplier) {
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
    case BIRD_TYPE_BOMB: {
        bird->dead.anim_start_sprite = SPRITE_EXPLOSION1;
        bird->dead.anim_sprite_amount = 6;
    } break;
    }
}

static void render_bird_parts_with_portal_influence(State *state, Bird *bird) {
    Sprite sprites[BIRD_DEATH_PARTS];
    get_bird_death_part_sprites(sprites, bird->type);
    for (int i = 0; i < BIRD_DEATH_PARTS; i++) {
        float ratio = portal_distance_to_center_ratio(state, bird->dead.positions[i]);
        float scale = ratio;
        float c = ratio * 255.0f;
        Color color = {c,c,c,255};
        atlas_draw(state, sprites[i], bird->dead.positions[i], bird->dead.rotations[i], scale, color);
    }
}

void birds_update_spawn_weights(State *state) {
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
        state->bird_spawn_weights[SPAWN_TYPE_REGULAR] = 7;
        state->bird_spawn_weights[SPAWN_TYPE_UMBRELLA] = 2;
        state->bird_spawn_weights[SPAWN_TYPE_BOMB] = 1;
        return;
    case 5:
        state->bird_spawn_weights[SPAWN_TYPE_REGULAR] = 7;
        state->bird_spawn_weights[SPAWN_TYPE_GIANT] = 2;
        state->bird_spawn_weights[SPAWN_TYPE_BOMB] = 1;
        return;
    default:
        state->bird_spawn_weights[SPAWN_TYPE_REGULAR] = 10;
        state->bird_spawn_weights[SPAWN_TYPE_UMBRELLA] = 2;
        state->bird_spawn_weights[SPAWN_TYPE_BOMB] = 1;
        state->bird_spawn_weights[SPAWN_TYPE_GIANT] = 1;
        return;
    }
}

static void setup_bird_by_level(State *state, Bird *bird) {
    bird->state = BIRD_STATE_ALIVE;
    bird->position.x = RESET_RIGHT;

    if (state->bird_height_idx == 0) {
        uint used_sections = 0;
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
    bird->position.y = randf(min_y, max_y);
    state->bird_height_idx++;
    if (state->bird_height_idx == BIRD_Y_SECTIONS) {
        state->bird_height_idx = 0;
    }

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

    bird->alive.move_speed = 0.2f + (bird->position.y + 1.0f) * 0.2f;
    bird->alive.current_sprite = 0;
    bird->alive.damage_timer = 0.0f;

    switch (spawn_type) {
    default:
        ASSERT(false);
    case SPAWN_TYPE_REGULAR: {
        bird->type = BIRD_TYPE_REGULAR;
        bird->alive.health = 1;
        bird->alive.collision_bounds.x = 0.07f;
        bird->alive.collision_bounds.y = 0.05f;
    } break;
    case SPAWN_TYPE_UMBRELLA: {
        bool bird_y_under_center = (bird->position.y < 0);
        bird->type = bird_y_under_center
            ? BIRD_TYPE_UMBRELLA_ABOVE
            : BIRD_TYPE_UMBRELLA_UNDER;
        bird->alive.health = 1;
        bird->alive.collision_bounds.x = 0.07f;
        bird->alive.collision_bounds.y = 0.09f;
    } break;
    case SPAWN_TYPE_BOMB: {
        bird->type = BIRD_TYPE_BOMB;
        bird->alive.health = 1;
        bird->alive.collision_bounds.x = 0.07f;
        bird->alive.collision_bounds.y = 0.07f;
    } break;
    case SPAWN_TYPE_GIANT: {
        bird->type = BIRD_TYPE_GIANT;
        bird->alive.health = 2;
        bird->alive.collision_bounds.x = 0.12f;
        bird->alive.collision_bounds.y = 0.1f;
    } break;
    case 40000: {
        bird->type = BIRD_TYPE_TINY;
        bird->alive.health = 1;
        bird->alive.collision_bounds.x = 0.05f;
        bird->alive.collision_bounds.y = 0.05f;
    } break;
    }
}

void birds_init(State *state) {
    state->bird_shader = LoadShader(0, "./src/shaders/bird.glsl");
    state->bird_shader_palette_location = GetShaderLocation(state->bird_shader, "palette");
    state->bird_damage_shader = LoadShader(0, "./src/shaders/bird_damage.glsl");
    state->birds_requested = 0;

    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_OUTLINE] = (Vector3){0.0f,0.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_BODY] = (Vector3){1.0f,1.0f,1.0f};
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_BEAK1] = (Vector3){1.0f,0.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_BEAK2] = (Vector3){0.75f,0.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_EYES] = (Vector3){1.0f,1.0f,1.0f};
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_GORE1] = (Vector3){1.0f,0.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_GORE2] = (Vector3){0.75f,0.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_GORE3] = (Vector3){0.5f,0.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_UMBRELLA_HANDLE] = (Vector3){0.5f,0.25f,0.0f};
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_UMBRELLA_CANOPY1] = (Vector3){1.0f,0.5f,0.0f};
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_UMBRELLA_CANOPY2] = (Vector3){0.0f,0.5f,1.0f};
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_EXPLOSION1] = (Vector3){1.0f,1.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_EXPLOSION2] = (Vector3){1.0f,0.5f,0.0f};
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_EXPLOSION3] = (Vector3){1.0f,0.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_WHITE][BIRD_PALETTE_IDX_EXPLOSION4] = (Vector3){0.75f,0.75f,0.75f};

    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_OUTLINE] = (Vector3){0.0f,0.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_BODY] = (Vector3){1.0f,1.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_BEAK1] = (Vector3){1.0f,0.5f,0.0f};
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_BEAK2] = (Vector3){0.75f,0.375f,0.0f};
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_EYES] = (Vector3){1.0f,1.0f,1.0f};
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_GORE1] = (Vector3){1.0f,0.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_GORE2] = (Vector3){0.75f,0.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_GORE3] = (Vector3){0.5f,0.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_UMBRELLA_HANDLE] = (Vector3){0.5f,0.25f,0.0f};
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_UMBRELLA_CANOPY1] = (Vector3){1.0f,1.0f,1.0f};
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_UMBRELLA_CANOPY2] = (Vector3){1.0f,0.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_EXPLOSION1] = (Vector3){1.0f,1.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_EXPLOSION2] = (Vector3){1.0f,0.5f,0.0f};
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_EXPLOSION3] = (Vector3){1.0f,0.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_YELLOW][BIRD_PALETTE_IDX_EXPLOSION4] = (Vector3){0.75f,0.75f,0.75f};

    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_OUTLINE] = (Vector3){0.0f,0.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_BODY] = (Vector3){0.5f,0.25f,0.0f};
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_BEAK1] = (Vector3){1.0f,1.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_BEAK2] = (Vector3){0.75f,0.75f,0.0f};
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_EYES] = (Vector3){1.0f,1.0f,1.0f};
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_GORE1] = (Vector3){1.0f,0.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_GORE2] = (Vector3){0.75f,0.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_GORE3] = (Vector3){0.5f,0.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_UMBRELLA_HANDLE] = (Vector3){0.75f,0.37f,0.0f};
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_UMBRELLA_CANOPY1] = (Vector3){1.0f,0.5f,0.75f};
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_UMBRELLA_CANOPY2] = (Vector3){0.0f,1.0f,1.0f};
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_EXPLOSION1] = (Vector3){1.0f,1.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_EXPLOSION2] = (Vector3){1.0f,0.5f,0.0f};
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_EXPLOSION3] = (Vector3){1.0f,0.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_BROWN][BIRD_PALETTE_IDX_EXPLOSION4] = (Vector3){0.75f,0.75f,0.75f};

    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_OUTLINE] = (Vector3){0.0f,0.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_BODY] = (Vector3){1.0f,0.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_BEAK1] = (Vector3){1.0f,1.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_BEAK2] = (Vector3){0.75f,0.75f,0.0f};
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_EYES] = (Vector3){1.0f,1.0f,1.0f};
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_GORE1] = (Vector3){0.0f,1.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_GORE2] = (Vector3){0.0f,0.75f,0.0f};
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_GORE3] = (Vector3){0.0f,0.5f,0.0f};
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_UMBRELLA_HANDLE] = (Vector3){0.5f,0.25f,0.0f};
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_UMBRELLA_CANOPY1] = (Vector3){1.0f,1.0f,1.0f};
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_UMBRELLA_CANOPY2] = (Vector3){0.0f,1.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_EXPLOSION1] = (Vector3){1.0f,1.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_EXPLOSION2] = (Vector3){1.0f,0.5f,0.0f};
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_EXPLOSION3] = (Vector3){1.0f,0.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_RED][BIRD_PALETTE_IDX_EXPLOSION4] = (Vector3){0.75f,0.75f,0.75f};

    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_OUTLINE] = (Vector3){0.0f,0.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_BODY] = (Vector3){0.25f,0.25f,0.25f};
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_BEAK1] = (Vector3){0.5f,0.5f,0.5f};
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_BEAK2] = (Vector3){0.25f,0.25f,0.25f};
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_EYES] = (Vector3){1.0f,1.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_GORE1] = (Vector3){1.0f,0.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_GORE2] = (Vector3){0.75f,0.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_GORE3] = (Vector3){0.5f,0.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_UMBRELLA_HANDLE] = (Vector3){0.5f,0.25f,0.0f};
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_UMBRELLA_CANOPY1] = (Vector3){1.0f,1.0f,1.0f};
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_UMBRELLA_CANOPY2] = (Vector3){1.0f,0.5f,0.0f};
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_EXPLOSION1] = (Vector3){1.0f,1.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_EXPLOSION2] = (Vector3){1.0f,0.5f,0.0f};
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_EXPLOSION3] = (Vector3){1.0f,0.0f,0.0f};
    state->bird_palette[BIRD_PALETTE_BLACK][BIRD_PALETTE_IDX_EXPLOSION4] = (Vector3){0.75f,0.75f,0.75f};
}

bool birds_all_available(State *state) {
    for (int i = 0; i < BIRD_CAPACITY; i++) {
        if (state->birds[i].state != BIRD_STATE_AVAILABLE) return false;
    }
    return true;
}

void birds_destroy_all(State *state) {
    for (int i = 0; i < BIRD_CAPACITY; i++) {
        Bird *b = &state->birds[i];
        if (b->state == BIRD_STATE_ALIVE) {
            bird_death_setup(b, (Vector2){0}, 1.0f);
        }
    }
}

void birds_reset(State *state) {
    for (int i = 0; i < BIRD_CAPACITY; i++) {
        state->birds[i].state = BIRD_STATE_AVAILABLE;
    }
    state->birds_requested = 0;
}

void birds_cleanup(State *state) {
    UnloadShader(state->bird_damage_shader);
}

void birds_update(State *state) {
    int birds_in_use = 0;
    for (int i = 0; i < BIRD_CAPACITY; i++) {
        if (state->birds[i].state != BIRD_STATE_AVAILABLE) {
            birds_in_use++;
        }
    }

    int available_birds = BIRD_CAPACITY - birds_in_use;

    bool is_global_state_suitable_for_birds = state->global_state == GLOBAL_STATE_GAME;
    bool we_need_more_birds = state->birds_requested < available_birds;

    if (is_global_state_suitable_for_birds && we_need_more_birds) {
        if (state->current_level.bird_timer < state->current_level.bird_frequency) {
            state->current_level.bird_timer += state->delta_time;
        } else {
            state->current_level.bird_timer = 0.0f;
            state->birds_requested++;
        }
    }

    for (int i = 0; i < BIRD_CAPACITY; i++) {
        Bird *bird = &state->birds[i];

        switch (bird->state) {
        case BIRD_STATE_AVAILABLE: {
            if (!is_global_state_suitable_for_birds) {
                break;
            }

            bool we_need_this_bird = birds_in_use < state->birds_requested;
            if (!we_need_this_bird) {
                break;
            }

            setup_bird_by_level(state, bird);

            state->birds_requested--;
        } break;
        case BIRD_STATE_ALIVE: {
            bird->position.x -= bird->alive.move_speed * state->delta_time;
            if (bird->position.x < RESET_LEFT) {
                bird->state = BIRD_STATE_AVAILABLE;
            } else {
                bird->anim_time += state->delta_time;
                if (bird->anim_time > GAME_ANIM_SPEED) {
                    bird->anim_time = 0.0f;
                    bird->alive.current_sprite = (bird->alive.current_sprite + 1) % ANIM_SPRITE_AMOUNT;
                }
            }
            if (bird->alive.damage_timer > 0.0f) {
                bird->alive.damage_timer -= state->delta_time;
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
            death_animation_update(state, bird);
            if (all_off_screen && bird->dead.anim_idx >= bird->dead.anim_sprite_amount) {
                bird->state = BIRD_STATE_AVAILABLE;
            }
        } break;
        case BIRD_STATE_INHALED_BY_PORTAL: {
            Vector2 portal_position = portal_get_position(state);
            bool all_inside_portal = true;
            for (int j = 0; j < BIRD_DEATH_PARTS; j++) {
                Vector2 direction = vec2_direction(bird->dead.positions[j], portal_position);
                direction = vec2_normalized(direction.x, direction.y);
                direction.x *= PORTAL_ATTRACT_MULTIPLIER * state->delta_time;
                direction.y *= PORTAL_ATTRACT_MULTIPLIER * state->delta_time;

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
            death_animation_update(state, bird);
            if (all_inside_portal) {
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
            if (all_off_screen) {
                bird->state = BIRD_STATE_AVAILABLE;
            }
        } break;
        default: break;
        }
    }
}

BirdHit bird_try_destroy(State *state, Bird *bird, Vector2 from, float velocity_multiplier) {
    bird->alive.health--;
    if (bird->alive.health <= 0) {
        state->portal_fuel++;
        state->bird_multiplier++;

        ASSERT(bird->type < BIRD_TYPES_TOTAL);

        state->birds_destroyed[bird->type]++;

        if (bird->type == BIRD_TYPE_BOMB) {
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
                    bird_try_destroy(state, other_bird, bird->position, EXPLOSIVE_VELOCITY_MULTIPLIER);
                }
            }
        } else {
            Vector2 master_velocity = vec2_normalized(
                bird->position.x - from.x,
                bird->position.y - from.y
            );
            master_velocity.x *= velocity_multiplier;
            master_velocity.y *= velocity_multiplier;

            bird_death_setup(bird, master_velocity, velocity_multiplier);
            int sound_idx = GetRandomValue(0, DEATH_SOUND_AMOUNT - 1);
            PlaySound(state->sounds_death_splats[sound_idx]);
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

BirdHit bird_try_destroy_by_player(State *state, Bird *bird) {
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
    return bird_try_destroy(state, bird, state->player.spinner_position, DEATH_VELOCITY_MULTIPLIER);
}

void birds_give_alive_ones_to_portal(State *state) {
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

void birds_prepare_shader(State *state, Area area) {
    Vector3 *palette = NULL;
    switch (area) {
    default:
    case AREA_FOREST: palette = state->bird_palette[BIRD_PALETTE_WHITE]; break;
    case AREA_MEADOWS: palette = state->bird_palette[BIRD_PALETTE_YELLOW]; break;
    case AREA_MOUNTAINS: palette = state->bird_palette[BIRD_PALETTE_BROWN]; break;
    case AREA_INDUSTRIAL: palette = state->bird_palette[BIRD_PALETTE_RED]; break;
    case AREA_CASTLE: palette = state->bird_palette[BIRD_PALETTE_BLACK]; break;
    }

    ASSERT(palette != NULL);

    SetShaderValueV(
        state->bird_shader,
        state->bird_shader_palette_location,
        palette,
        SHADER_UNIFORM_VEC3,
        BIRD_PALETTE_IDX_TOTAL
    );
}

void birds_render(State *state) {
    for (int i = 0; i < BIRD_CAPACITY; i++) {
        Bird *bird = &state->birds[i];
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
            }
            if (bird->alive.damage_timer > 0.0f) {
                BeginShaderMode(state->bird_damage_shader);
                atlas_draw(state, bird_sprite, bird->position, 0.0f, 1.0f, OPAQUE);
                EndShaderMode();
            } else {
                BeginShaderMode(state->bird_shader); {
                    switch (bird->type) {
                    default: {
                        atlas_draw(state, bird_sprite, bird->position, 0.0f, 1.0f, OPAQUE);
                    } break;
                    case BIRD_TYPE_TINY: {
                        float tiny_scale = 0.5f;
                        atlas_draw(state, bird_sprite, bird->position, 0.0f, tiny_scale, OPAQUE);
                        break;
                    } break;
                    case BIRD_TYPE_UMBRELLA_ABOVE: {
                        atlas_draw(state, SPRITE_UMBRELLA, bird->position, 0.0f, 1.0f, OPAQUE);
                        Vector2 offset_bird_position = {
                            bird->position.x,
                            bird->position.y - 0.03f
                        };
                        atlas_draw(state, bird_sprite, offset_bird_position, 0.0f, 1.0f, OPAQUE);
                    } break;
                    case BIRD_TYPE_UMBRELLA_UNDER: {
                        float upside_down_rotation = 180.0f;
                        atlas_draw(state, SPRITE_UMBRELLA, bird->position, upside_down_rotation, 1.0f, OPAQUE);
                        Vector2 offset_bird_position = {
                            bird->position.x,
                            bird->position.y + 0.03f
                        };
                        atlas_draw(state, bird_sprite, offset_bird_position, 0.0f, 1.0f, OPAQUE);
                    } break;
                    }
                } EndShaderMode();
            }
            if (has_flag(state->bird_flags, BIRD_FLAG_SHOW_COLLISION_BOUNDS)) {
                birds_draw_collision_bounds(state, bird);
            }
        } break;
        case BIRD_STATE_DEAD: {
            BeginShaderMode(state->bird_shader); {
                death_animation_render(state, bird);
                Sprite sprites[BIRD_DEATH_PARTS];
                get_bird_death_part_sprites(sprites, bird->type);
                for (int j = 0; j < BIRD_DEATH_PARTS; j++) {
                    Sprite sprite = sprites[j];
                    float rotation = bird->dead.rotations[j];
                    atlas_draw(state, sprite, bird->dead.positions[j], rotation, 1.0f, OPAQUE);
                }
            } EndShaderMode();
        } break;
        case BIRD_STATE_INHALED_BY_PORTAL: {
            BeginShaderMode(state->bird_shader); {
                death_animation_render(state, bird);
                render_bird_parts_with_portal_influence(state, bird);
            } EndShaderMode();
        } break;
        case BIRD_STATE_EXHALED_BY_PORTAL: {
            BeginShaderMode(state->bird_shader); {
                render_bird_parts_with_portal_influence(state, bird);
            } EndShaderMode();
        } break;
        }
    }
}
