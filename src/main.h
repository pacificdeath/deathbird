#ifndef MAIN_H
#define MAIN_H

#define AREA_TEXTURE_SIZE 128.0f

#define GAME_WIDTH_RATIO 3.0f
#define GAME_HEIGHT_RATIO 2.0f
#define GAME_MIN_SIZE 4
#define GAME_GROUND_Y (-0.9f)
#define GAME_CEILING_Y (0.99f)
#define GAME_OUT_OF_BOUNDS_COLOR ((Color){0,0,0,255})
#define GAME_ANIM_SPEED 0.1f

#define SPRITE_SCROLLER_CAP 16

#define BIRD_CAPACITY 128
#define BIRD_DEATH_BODY_PARTS 6 // (head) + (body) + (2 wings) + (2 eyes)
#define BIRD_DEATH_GORE_PARTS 16
#define BIRD_DEATH_PARTS (BIRD_DEATH_BODY_PARTS + BIRD_DEATH_GORE_PARTS)
#define BIRD_Y_SECTIONS 7
#define BIRD_HIGHEST_MULTIPLIER_CAPACITY 16
#define BIRD_CIRCLE_CAPACITY 4
#define BIRD_STATIONARY_CAPACITY 8
#define BIRD_CIRCLE_MAX_BIRDS 16
#define BOSS_CAPACITY 2
#define BOSS_MIN_X (-0.7f)
#define BOSS_MAX_X 0.7f
#define BOSS_MIN_Y (-0.7f)
#define BOSS_MAX_Y 0.7f
#define BOSS_SPEED 0.5f

#define TRAILING_FIREBALL_HISTORY_SIZE 256
#define TRAILING_FIREBALLS_PER_DEATH_PART 7
#define TRAILING_FIREBALL_INTERVAL 0.2f

#define CONSOLE_MAX_LINES 32
#define CONSOLE_LINE_MAX_LENGTH 64
#define CONSOLE_INPUT_LINE_INDEX (-1)
#define CONSOLE_BG ((Color){0,0,0,215})
#define CONSOLE_WHITE ((Color){255,255,255,192})
#define CONSOLE_RED ((Color){255,0,0,192})
#define CONSOLE_GREEN ((Color){0,255,0,192})
#define CONSOLE_BLUE ((Color){128,192,255,192})
#define CONSOLE_PURPLE ((Color){192,64,255,192})
#define CONSOLE_COMMAND_SELECTED_FG_COLOR ((Color){128,255,128,255})
#define CONSOLE_COMMAND_SELECTED_BG_COLOR ((Color){0,255,0,128})

#define FONT "bios.ttf"
#define FONT_SIZE 200

#define PORTAL_RADIUS 0.4f
#define PORTAL_APPEAR_SPEED 1.0f
#define PORTAL_DISAPPEAR_SPEED PORTAL_APPEAR_SPEED
#define PORTAL_EMIT_MULTIPLIER 0.2f
#define PORTAL_EXHALE_RATE 0.05f

#define SCALE_NORMAL ((Vector2){1.0f,1.0f})
#define SCALE_X_FLIP ((Vector2){-1.0f,1.0f})
#define SCALE_UNIFORM(v) ((Vector2){(v),(v)})

#define V3(x,y,z) ((Vector3){(x),(y),(z)})
#define V3_UNIFORM(v) ((Vector3){(v),(v),(v)})

#define OPAQUE ((Color){255,255,255,255})
#define DEATH_SOUND_AMOUNT 8

#define BOSS_NORMAL_BULLET_CAPACITY 32

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

enum {
    PORTAL_RENDER_MODE_INHALE,
    PORTAL_RENDER_MODE_EXHALE,
};

#define GLOBAL_STATE_LIST \
    X(DEFAULT) \
    X(GAME) \
    X(TRANSITION) \
    X(GAME_OVER) \
    X(BOSSBATTLE)
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
    X(EXHALED_BY_PORTAL) \
    X(GROUNDED) \
    X(RETURN_TO_GROUND) \
    X(UP) \
    X(DOWN) \
    X(INHALED_BY_PORTAL) \
    X(INSIDE_PORTAL) \
    X(LOST_LIFE)
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
    PLAYER_FLAG_DRILL_UP = 1 << 1,
    PLAYER_FLAG_DRILL_DOWN = 1 << 2,
};

#define BIRD_STATE_LIST \
    X(AVAILABLE) \
    X(ALIVE) \
    X(DEAD) \
    X(INHALED_BY_PORTAL) \
    X(INSIDE_PORTAL) \
    X(EXHALED_BY_PORTAL) \
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
    X(RESERVED) \
    X(UMBRELLA_ABOVE) \
    X(UMBRELLA_UNDER) \
    X(GIANT) \
    X(TINY) \
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

enum {
    SPAWN_TYPE_REGULAR,
    SPAWN_TYPE_GIANT,
    SPAWN_TYPE_UMBRELLA,
    SPAWN_TYPES_TOTAL,
};

enum {
    BIRD_PALETTE_WHITE,
    BIRD_PALETTE_YELLOW,
    BIRD_PALETTE_BROWN,
    BIRD_PALETTE_GREEN,
    BIRD_PALETTE_VOID,
    BIRD_PALETTE_RED,
    BIRD_PALETTE_BLACK,
    BIRD_PALETTE_BLUE,
    BIRD_PALETTES_TOTAL,
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

typedef enum BirdHit {
    BIRD_HIT_NONE = 0,
    BIRD_HIT_DESTROYED = 1 << 0,
    BIRD_HIT_BOUNCE_ALLOWED = 1 << 1,
    BIRD_HIT_PASS_THROUGH_ALLOWED = 1 << 2,
} BirdHit;

enum {
    TRANSITION_FADE,
    TRANSITION_CARTOON,
};

enum {
    TRANSITION_VALUE_MIN,
    TRANSITION_VALUE_MAX,
};

typedef struct SpriteScroller {
    Sprite sprite;
    int horizontal_sprites;
    float horizontal_speed;
    int vertical_sprites;
    float vertical_speed;
    Vector2 scroll;
    int opacity;
} SpriteScroller;

struct State;

enum {
    BIRDCORE,
    BIRDCOMPUTER,
    BIRDATLAS,
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
} Console;

typedef struct Player {
    int lives;
    int state;
    int flags;
    Vector2 kicker_position;
    Vector2 spinner_position;
    float rotation;
    int current_input;
    float last_turnaround_y;
    int may_not_interact_with_this_bird_idx;
    float vertical_speed;
    float horizontal_acceleration;
    float horizontal_speed;
    float reset_flicker_timer;
    float reset_flicker_count;
} Player;

enum {
    FLAG_NONE = 0,
    FLAG_CONSOLE = 1 << 0,
    FLAG_SHOW_COLLISION_BOUNDS = 1 << 1,
    FLAG_SHOW_OUT_OF_BOUNDS = 1 << 2,
    FLAG_BIRD_SHARK_HEALTH_1 = 1 << 3,
};

enum {
    BOSS_STATE_LEFT,
    BOSS_STATE_DOWN,
    BOSS_STATE_RIGHT,
    BOSS_STATE_UP,
};

typedef struct Bird {
    int state;
    int type;
    Vector2 position;
    float anim_time;
    bool flip_sprite;
    union {
        struct {
            int health;
            int current_sprite;
            Vector2 collision_bounds;
            float move_speed;
            float damage_timer;
            bool part_of_circle;
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

typedef struct TrailingFireballs {
    Bird *bird;
    Vector2 positions[BIRD_DEATH_PARTS][TRAILING_FIREBALLS_PER_DEATH_PART];
} TrailingFireballs;

typedef struct TrailingFireballHistory {
    int write_index;
    Vector2 positions[BIRD_DEATH_PARTS][TRAILING_FIREBALL_HISTORY_SIZE];
    double times[BIRD_DEATH_PARTS][TRAILING_FIREBALL_HISTORY_SIZE];
} TrailingFireballHistory;

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

typedef struct Level {
    int sprite_scroller_amount;
    float bird_frequency;
    float bird_timer;
    int required_fuel;
} Level;

typedef struct Boss {
    Bird *bird;
    int state;
    int full_health;
} Boss;

typedef struct State {
    int global_state;
    int flags;
    Font font;
    float font_size;

    int window_width;
    int window_height;

    int game_width;
    int game_height;
    int game_left;
    int game_right;
    int game_top;
    int game_bottom;
    int game_center_x;
    int game_center_y;

    Sound sounds_death_splats[DEATH_SOUND_AMOUNT];
    Sound sound_explosion;

    Texture atlas;

    int area;
    int areas_discovered_bits;

    SpriteScroller sprite_scrollers[SPRITE_SCROLLER_CAP];

    int level_idx;
    int level_score;

    int portal_fuel;

    Level current_level;

    Console console;

    Player player;

    Bird birds[BIRD_CAPACITY];
    BirdCircle bird_circles[BIRD_CIRCLE_CAPACITY];
    Bird *bird_monitor;
    BirdStationary bird_stationaries[BIRD_STATIONARY_CAPACITY];
    int bird_circle_count;
    Vector3 bird_palette[BIRD_PALETTES_TOTAL][BIRD_PALETTE_IDX_TOTAL];
    Shader bird_shader;
    int bird_shader_palette_location;
    Shader bird_damage_shader;
    int bird_highest_multipliers[BIRD_HIGHEST_MULTIPLIER_CAPACITY];
    int bird_multiplier;
    int bird_multiplier_display;
    float bird_multiplier_timer;
    int birds_destroyed[BIRD_TYPE_TOTAL];
    TrailingFireballs trailing_fireballs;
    TrailingFireballHistory trailing_fireball_history;
    Boss bosses[BOSS_CAPACITY];
    const char *bosses_name;

    int bird_random_heights[BIRD_Y_SECTIONS];
    int bird_height_idx;
    int bird_spawn_weights[SPAWN_TYPES_TOTAL];

    Shader cartoon_transition_shader;
    int cartoon_transition_shader_location_resolution;
    int cartoon_transition_shader_location_size;
    float cartoon_transition_size;

    float fader_level;

    Shader portal_shader;
    Texture portal_texture;
    float portal_linear_disappearance;
    float portal_smooth_disappearance;
    Color portal_color;
    float portal_timer;
    float portal_power;

    float scale_multiplier;
    float delta_time;
    float time_scale;
    int timer_state;
    float timer;

    Boss boss;
} State;

State *state;

Vector2 to_pixel_position(State *state, Vector2 game_position);
Vector2 to_pixel_size(State *state, Vector2 game_size);

void travel_to_area(int area, int level);
void travel_to_next_area();

Rectangle game_rectangle(State *state);
bool has_flag(int flags, int flag);
float randf(float min, float max);

Vector2 vec2_normalized(float x, float y);
Vector2 vec2_direction(Vector2 from, Vector2 to);
float vec2_distance(Vector2 a, Vector2 b);

void atlas_draw(State *state, Sprite sprite, Vector2 position, float rotation, Vector2 scale, Color color);

Vector2 portal_get_position(State *state);
float portal_distance_to_center_ratio(State *state, Vector2 position);
bool portal_inhale_object(float *obj_position, float obj_step, float portal_position);

BirdHit apply_damage_to_bird_from_player(Bird *bird);

inline static Vector2 move_a_towards_b(Vector2 a, Vector2 b, float t) {
    return (Vector2) {
        a.x + ((b.x - a.x) * state->delta_time * t),
        a.y + ((b.y - a.y) * state->delta_time * t),
    };
}

inline static void update_font_size() {
    state->font_size = (FONT_SIZE / state->font.baseSize) * state->scale_multiplier;
}

#endif
