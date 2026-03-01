#ifndef MAIN_H
#define MAIN_H

#define AREA_TEXTURE_SIZE 128.0f

#define GAME_WIDTH_RATIO 3.0f
#define GAME_HEIGHT_RATIO 2.0f
#define GAME_MIN_SIZE 4
#define GAME_GROUND_Y (-0.9f)
#define GAME_OUT_OF_BOUNDS_COLOR ((Color){0,0,0,255})
#define GAME_ANIM_SPEED 0.1f

#define SCORE_TARGET 4000

#define SPRITE_SCROLLER_CAP 16

#define ERROR_COLOR ((Color){0xff,0,0,0xff})

#define BIRD_CAPACITY 128
#define BIRD_DEATH_BODY_PARTS 6 // (head) + (body) + (2 wings) + (2 eyes)
#define BIRD_DEATH_GORE_PARTS 16
#define BIRD_DEATH_PARTS (BIRD_DEATH_BODY_PARTS + BIRD_DEATH_GORE_PARTS)
#define BIRD_Y_SECTIONS 7
#define BIRD_CIRCLE_CAPACITY 4
#define BIRD_CIRCLE_ANGULAR_BASE_SPEED 8.0f
#define BIRD_STATIONARY_CAPACITY 8
#define BIRD_CIRCLE_MAX_BIRDS 10
#define BIRD_FREQUENCY 0.2f
#define BIRD_PALETTE_COLORS 32
#define BIRD_SHADER_LOCATION_PALETTE_TEX ("paletteTex")
#define BIRD_SHADER_LOCATION_PALETTE_INDEX ("paletteIndex")

#define CONSOLE_MAX_LINES 32
#define CONSOLE_LINE_MAX_LENGTH 64
#define CONSOLE_INPUT_LINE_INDEX (-1)
#define CONSOLE_BG ((Color){0,0,0,215})
#define CONSOLE_WHITE ((Color){255,255,255,192})
#define CONSOLE_RED ((Color){255,0,0,192})
#define CONSOLE_GREEN ((Color){0,255,0,192})
#define CONSOLE_YELLOW ((Color){255,255,0,192})
#define CONSOLE_BLUE ((Color){128,192,255,192})
#define CONSOLE_PURPLE ((Color){192,64,255,192})
#define CONSOLE_COMMAND_SELECTED_FG_COLOR ((Color){128,255,128,255})
#define CONSOLE_COMMAND_SELECTED_BG_COLOR ((Color){0,255,0,128})

#define WORLD_TRANSITION_TIME (1.0f)

#define FONT "ComicMono.ttf"
#define FONT_SIZE 250

#define SCALE_NORMAL ((Vector2){1.0f,1.0f})
#define SCALE_X_FLIP ((Vector2){-1.0f,1.0f})
#define SCALE_UNIFORM(v) ((Vector2){(v),(v)})

#define OPAQUE ((Color){255,255,255,255})
#define DEATH_SOUND_AMOUNT 8

#define PI_X2 (PI * 2.0f)

enum {
    TIMER_STATE_NONE,
    TIMER_STATE_SLOW_DOWN,
    TIMER_STATE_WAIT,
    TIMER_STATE_SPEED_UP,
};

#define AREA_LIST \
    X(NONE) \
    X(FOREST) \
    X(MEADOWS) \
    X(MOUNTAINS) \
    X(GREEN) \
    X(VOID) \
    X(INDUSTRIAL) \
    X(CASTLE) \
    X(SKY) \
    X(TOTAL)
enum {
#define X(name) AREA_##name,
    AREA_LIST
#undef X
};
const char *area_names[] = {
#define X(name) #name,
    AREA_LIST
#undef X
};

#define GLOBAL_STATE_LIST \
    X(DEFAULT) \
    X(GAME) \
    X(GAME_OVER)
enum {
#define X(name) GLOBAL_STATE_##name,
    GLOBAL_STATE_LIST
#undef X
};
const char *global_state_names[] = {
#define X(name) #name,
    GLOBAL_STATE_LIST
#undef X
};

#define PLAYER_STATE_LIST \
    X(NONE) \
    X(GROUNDED) \
    X(RETURN_TO_GROUND) \
    X(UP) \
    X(DOWN)
enum {
#define X(name) PLAYER_STATE_##name,
    PLAYER_STATE_LIST
#undef X
};
const char *player_state_names[] = {
#define X(name) #name,
    PLAYER_STATE_LIST
#undef X
};

enum {
    PLAYER_FLAG_HIDDEN = 1 << 0,
};

#define BIRD_STATE_LIST \
    X(AVAILABLE) \
    X(ALIVE) \
    X(DEAD) \
    X(RESERVED)
enum {
#define X(name) BIRD_STATE_##name,
    BIRD_STATE_LIST
#undef X
};
const char *bird_state_names[] = {
#define X(name) #name,
    BIRD_STATE_LIST
#undef X
};

#define BIRD_TYPE_LIST \
    X(NONE) \
    X(REGULAR) \
    X(UMBRELLA_ABOVE) \
    X(UMBRELLA_UNDER) \
    X(GIANT) \
    X(BOMB) \
    X(MONITOR) \
    X(STATIONARY) \
    X(WHEELS) \
    X(SHARK) \
    X(TOTAL)
enum {
#define X(name) BIRD_TYPE_##name,
    BIRD_TYPE_LIST
#undef X
};
const char *bird_type_names[] = {
#define X(name) #name,
    BIRD_TYPE_LIST
#undef X
};

#define SPAWN_TYPE_LIST \
    X(REGULAR) \
    X(CIRCLE) \
    X(UMBRELLA) \
    X(UMBRELLA_CIRCLE) \
    X(GIANT) \
    X(GIANT_CIRCLE) \
    X(BOMB) \
    X(BOMB_CIRCLE) \
    X(MONITOR) \
    X(TOTAL)
enum {
#define X(name) SPAWN_TYPE_##name,
    SPAWN_TYPE_LIST
#undef X
};
static const char *spawn_type_names[] = {
#define X(name) #name,
    SPAWN_TYPE_LIST
#undef X
};

#define BIRD_PALETTE_LIST \
    X(WHITE) \
    X(YELLOW) \
    X(BROWN) \
    X(GREEN) \
    X(VOID) \
    X(RED) \
    X(BLACK) \
    X(BLUE) \
    X(TOTAL)
enum {
#define X(name) BIRD_PALETTE_##name,
    BIRD_PALETTE_LIST
#undef X
};
static const char *bird_palette_names[] = {
#define X(name) #name,
    BIRD_PALETTE_LIST
#undef X
};

#define BIRD_PALETTE_IDX_LIST \
    X(OUTLINE) \
    X(BODY) \
    X(BEAK1) \
    X(BEAK2) \
    X(EYES) \
    X(GORE1) \
    X(GORE2) \
    X(GORE3) \
    X(UMBRELLA_HANDLE) \
    X(UMBRELLA_CANOPY1) \
    X(UMBRELLA_CANOPY2) \
    X(EXPLOSION1) \
    X(EXPLOSION2) \
    X(EXPLOSION3) \
    X(EXPLOSION4) \
    X(MONITOR_FRAME) \
    X(MONITOR_FRAME_LIT) \
    X(MONITOR_FRAME_SHADOW) \
    X(MONITOR_SCREEN) \
    X(MONITOR_SCREEN_LINE) \
    X(MONITOR_BIRD_OUTLINE) \
    X(MONITOR_BIRD_BODY) \
    X(MONITOR_BIRD_BEAK) \
    X(MONITOR_BIRD_EYE) \
    X(SHARK_BODY) \
    X(SHARK_BODY2) \
    X(SHARK_EYES) \
    X(SHARK_TEETH) \
    X(WHEEL_TIRE1) \
    X(WHEEL_TIRE2) \
    X(WHEEL_RIM1) \
    X(WHEEL_RIM2) \
    X(TOTAL)
enum {
#define X(name) BIRD_PALETTE_IDX_##name,
    BIRD_PALETTE_IDX_LIST
#undef X
};
static const char *bird_palette_idx_names[] = {
#define X(name) #name,
    BIRD_PALETTE_IDX_LIST
#undef X
};

enum {
    BIRD_HIT_NONE = 0,
    BIRD_HIT_DESTROYED = 1 << 0,
    BIRD_HIT_BOUNCE_ALLOWED = 1 << 1,
    BIRD_HIT_PASS_THROUGH_ALLOWED = 1 << 2,
};

typedef struct SpriteScroller {
    int sprite;
    int horizontal_sprites;
    float horizontal_speed;
    int vertical_sprites;
    float vertical_speed;
    Vector2 scroll;
    unsigned char opacity;
} SpriteScroller;

struct State;

enum {
    BIRDCORE,
    BIRDCOMPUTER,
    BIRDATLAS,
    BIRDTEXTURE,
};

typedef struct Command {
    int type;
    char *name;
    void (*function)(void);
} Command;

typedef struct CommandBuffer {
    int capacity;
    int count;
    Command *commands;
} CommandBuffer;

typedef struct RankedCommand {
    Command *command;
    int score;
} RankedCommand;

typedef struct CommandResult {
    char text[CONSOLE_LINE_MAX_LENGTH];
    Color color;
} CommandResult;

typedef struct ResultBuffer {
    int capacity;
    int count;
    CommandResult *results;
} ResultBuffer;

typedef void (*ConsoleFunction)(void);

typedef struct Console {
    int mode;
    Rectangle rectangle;
    char input_line[CONSOLE_LINE_MAX_LENGTH];
    CommandBuffer command_buffer;
    RankedCommand *ranked_commands;
    int ranked_command_count;
    ResultBuffer result_buffer;
    int cursor_x;
    int cursor_y;
    int view_offset;
    ConsoleFunction live_function;

    int atlas_sprite_idx;
    int atlas_palette_idx;

    Texture texture;
} Console;

typedef struct Player {
    int state;
    int flags;
    Vector2 kicker_position; // TODO?
    Vector2 spinner_position; // TODO?
    float rotation;
    int current_input;
    float last_turnaround_y;
    int may_not_interact_with_this_bird_idx;
    float vertical_speed;
    float horizontal_acceleration;
    float horizontal_speed;
    float reset_flicker_timer;
    int reset_flicker_count;
} Player;

enum {
    FLAG_GAME_IS_FULLSCREEN = 1 << 0,
    FLAG_GAME_IS_PAUSED = 1 << 1,
    FLAG_CONSOLE = 1 << 2,
    FLAG_SHOW_COLLISION_BOUNDS = 1 << 3,
    FLAG_SHOW_OUT_OF_BOUNDS = 1 << 4,
    FLAG_DEATHBIRD_BIG_QUIT = 1 << 5,
};

enum {
    MENU_STATE_OPENING,
    MENU_STATE_ACTIVE,
    MENU_STATE_CLOSING,
    MENU_STATE_INACTIVE,
};

enum {
    BIRD_FLAG_IS_RESERVED = (1 << 0),
    BIRD_FLAG_FLIP_SPRITE = (1 << 1),
};

typedef struct Bird {
    int state;
    int type;
    int flags;
    int palette_index;
    Vector2 position;
    float anim_time;
    union {
        struct {
            int health;
            float rotation;
            int current_sprite;
            Vector2 collision_bounds;
            float move_speed;
            float damage_timer;
        } alive;
        struct {
            int anim_start_sprite;
            int anim_sprite_amount;
            int anim_idx;
            Vector2 velocities[BIRD_DEATH_PARTS];
            Vector2 positions[BIRD_DEATH_PARTS];
            float angular_velocities[BIRD_DEATH_PARTS];
            float rotations[BIRD_DEATH_PARTS];
        } dead;
    };
} Bird;

typedef struct BirdCircle {
    Bird *birds[BIRD_CIRCLE_MAX_BIRDS];
    int count;
    Vector2 position;
    float rotation;
    float radius;
    float move_speed;
    float angular_speed;
} BirdCircle;

typedef struct BirdStationary {
    Bird *bird;
    Vector2 position;
} BirdStationary;

typedef struct FrameData {
    int flags;
} FrameData;

typedef struct State {
    int global_state;
    int flags;
    Font font;
    float font_size;

    FrameData frame_data;

    int cached_window_width;
    int cached_window_height;

    int game_width;
    int game_height;
    int game_left;
    int game_right;
    int game_top;
    int game_bottom;
    int game_center_x;
    int game_center_y;

    int score;

    int menu_state;
    int menu_cached_global_state;
    Rectangle menu_rectangle;
    int menu_selected_idx;
    float menu_big_animation;
    float menu_sine_animation;
    float menu_select_animation;
    float menu_selection_bird_position;
    float menu_selection_bird_animation;
    int menu_selection_bird_sprite;

    Sound sounds_death_splats[DEATH_SOUND_AMOUNT];
    Sound sound_explosion;

    Texture atlas;

    int area;
    int next_area;

    SpriteScroller scrollers[SPRITE_SCROLLER_CAP];
    int scroller_count;

    float world_transition_animation;

    Console console;

    Player player;

    Bird birds[BIRD_CAPACITY];

    BirdCircle bird_circles[BIRD_CIRCLE_CAPACITY];
    int bird_circle_count;

    float bird_timer;
    double bird_start_count;
    const char *the_time;

    BirdStationary bird_stationaries[BIRD_STATIONARY_CAPACITY];
    Shader bird_shader;
    Texture bird_palette_texture;
    int bird_shader_palette_tex_location;
    int bird_shader_palette_index_location;
    Shader bird_damage_shader;

    int bird_random_heights[BIRD_Y_SECTIONS];
    int bird_height_idx;
    int bird_spawn_weights[SPAWN_TYPE_TOTAL];
    int bird_total_spawn_weight;

    Shader cartoon_transition_shader;
    int cartoon_transition_shader_location_resolution;
    int cartoon_transition_shader_location_size;
    float cartoon_transition_size;

    float fader_level;

    float scale_multiplier;
    float delta_time;
    float time_scale;
    int timer_state;
    float timer;
} State;

State *state;

Vector2 to_pixel_position(Vector2 game_position);
Vector2 to_game_position(Vector2 pixel_position);
Vector2 to_pixel_size(Vector2 game_size);

bool has_flag(int flags, int flag) {
    return (flags & flag) == flag;
}

float randf(float min, float max) {
    float random = (float)GetRandomValue(0, 1000000) / 1000000.0f;
    return min + random * (max - min);
}

Vector2 vec2_normalized(float x, float y) {
    float w = sqrtf((x * x) + (y * y));
    if (w <= 0) {
        return (Vector2){0};
    }
    Vector2 result = {
        x /= w,
        y /= w
    };
    return result;
}

Vector2 vec2_direction(Vector2 from, Vector2 to) {
    return (Vector2) {
        .x = to.x - from.x,
        .y = to.y - from.y
    };
}

float vec2_distance(Vector2 a, Vector2 b) {
    float x = fabsf(a.x - b.x);
    float y = fabsf(a.y - b.y);
    return sqrtf(x*x + y*y);
}

void atlas_draw(int sprite, Vector2 position, float rotation, Vector2 scale, Color color);

int apply_damage_to_bird_from_player(Bird *bird);

Vector2 to_pixel_position(Vector2 game_position) {
    return (Vector2) {
        (float)state->game_left + ((float)game_position.x + 1.0f) / 2.0f * (float)state->game_width,
        (float)state->game_bottom - ((float)game_position.y + 1.0f) / 2.0f * (float)state->game_height
    };
}

Vector2 to_game_position(Vector2 pixel_position) {
    return (Vector2) {
        (((pixel_position.x - (float)state->game_left) / (float)state->game_width) * 2.0f) - 1.0f,
        ((((float)state->game_bottom - pixel_position.y) / (float)(float)state->game_height) * 2.0f) - 1.0f
    };
}

Vector2 to_pixel_size(Vector2 game_size) {
    return (Vector2) {
        game_size.x / 2.0f * (float)state->game_width,
        game_size.y / 2.0f * (float)state->game_height
    };
}

void game();
void game_over();

#endif
