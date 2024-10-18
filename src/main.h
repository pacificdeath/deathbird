#ifndef MAIN_H
#define MAIN_H

#include "raylib.h"

#define GAME_WIDTH_RATIO 3.0f
#define GAME_HEIGHT_RATIO 2.0f
#define GAME_MIN_SIZE 3
#define GAME_GROUND_Y -0.87f
#define GAME_CEILING_Y 0.97f
#define GAME_OUT_OF_BOUNDS_COLOR BLACK

#define ATLAS_PADDING 1

#define AREA_TEXTURE_SIZE 128.0f

#define LEVEL_BASE_BIRD_TOTAL 40
#define LEVEL_MIN_RANDOM_BIRD_AMOUNT 0
#define LEVEL_MAX_RANDOM_BIRD_AMOUNT 10
#define LEVEL_BIRD_AMOUNT_MULTIPLIER 5
#define LEVEL_MIN_BIRD_FREQUENCY 0.1f
#define LEVEL_MAX_BIRD_FREQUENCY 0.3f
#define LEVELS_BEFORE_MIN_FREQUENCY 31
#define LEVEL_MIN_RANDOM_BIRD_FREQUENCY 0.0f
#define LEVEL_MAX_RANDOM_BIRD_FREQUENCY 0.1f
#define LEVEL_IDX_TEXT_BG_COLOR ((Color) { 0, 0, 0, 255 })
#define LEVEL_IDX_TEXT_COLOR ((Color) { 255, 255, 255, 255 })
#define LEVEL_SCORE_BAR_BG_COLOR ((Color) { 0, 32, 0, 128 })
#define LEVEL_SCORE_BAR_FILL_FG_COLOR ((Color) { 0, 255, 0, 128 })
#define LEVEL_SCORE_BAR_PREVIEW_FILL_FG_COLOR ((Color) { 0, 255, 0, 255 })
#define LEVEL_SCORE_BAR_FILLED_FG_COLOR ((Color) { 0, 128, 255, 128 })
#define LEVEL_SCORE_TEXT_COLOR ((Color) { 255, 255, 255, 255 })
#define LEVEL_PASSED_BIRDS_BAR_BG_COLOR ((Color) { 32, 0, 0, 128 })
#define LEVEL_PASSED_BIRDS_BAR_FG_COLOR ((Color) { 255, 0, 0, 128 })
#define LEVEL_MULTIPLIER_TEXT_BG_COLOR ((Color) { 255, 0, 0, 255 })
#define LEVEL_MULTIPLIER_TEXT_COLOR ((Color) { 255, 255, 255, 255 })

#define PLAYER_VERTICAL_SPEED 1.5f
#define PLAYER_HORIZONTAL_SPEED 2.0f
#define PLAYER_MAX_LEFT -0.9f
#define PLAYER_MAX_RIGHT 0.9f
#define PLAYER_ROTATION_SPEED_GROUND -450.0f
#define PLAYER_ROTATION_SPEED_GROUND_MOVEMENT 1200.0f
#define PLAYER_ROTATION_SPEED_PORTAL 1000.0f
#define PLAYER_ROTATION_SPEED_AIR 2500.0f
#define PLAYER_MULTIPLIER_DISPLAY_TIME 1.0f

#define BIRD_CAPACITY 32
#define BIRD_DEATH_BODY_PARTS 6 // (head) + (body) + (2 wings) + (2 eyes)
#define BIRD_DEATH_GORE_PARTS 4
#define BIRD_DEATH_PARTS (BIRD_DEATH_BODY_PARTS + BIRD_DEATH_GORE_PARTS)
#define BIRD_GRAVITY 4.0f
#define BIRD_MIN_MOVE_SPEED 0.2f
#define BIRD_MAX_MOVE_SPEED 0.8f
#define BIRD_ANIM_SPEED 0.1f
#define BIRD_ANIM_TEX_AMOUNT 4
#define BIRD_GORE_TEX_AMOUNT 2
#define BIRD_BLOOD_TEX_AMOUNT 4
#define BIRD_DAMAGE_TIME 0.2
#define BIRD_DEATH_VELOCITY_MULTIPLIER 0.8f
#define BIRD_DEATH_VELOCITY_MULTIPLIER_PORTAL_INHALE 1.5f
#define BIRD_DEATH_VELOCITY_MULTIPLIER_PORTAL_EXHALE 0.8f
#define BIRD_DEATH_MAX_RANDOM_VELOCITY 1.4f
#define BIRD_DEATH_GROUND_BOUNCE_VELOCITY_THRESHOLD 2.0f
#define BIRD_DEATH_GROUND_BOUNCE_VELOCITY -0.4f
#define BIRD_DEATH_MAX_RANDOM_ANGULAR_VELOCITY 1400.0f
#define BIRD_RESET_RIGHT 1.2f
#define BIRD_RESET_LEFT -BIRD_RESET_RIGHT
#define BIRD_VERTICAL_FREEDOM 0.7f
#define BIRD_GIANT_SPLASH_RADIUS 1.2f

#define MENU_FONT "bios.ttf"
#define MENU_FONT_SIZE 250
#define MENU_BG_COLOR ((Color) { 0, 0, 192, 255 })
#define MENU_CURSOR_BG_COLOR ((Color) { 255, 0, 0, 255 })
#define MENU_FG_COLOR ((Color) { 255, 255, 255, 255 })
#define MENU_TEXT_COLOR ((Color) { 255, 255, 0, 255 })
#define MENU_ACTIVE_LEVEL_COLOR ((Color) { 0, 255, 0, 255 })
#define MENU_LINE_COUNT 8

#define FADER_MAX 255
#define FADER_MIN 0
#define FADER_SPEED 500.0f

#define PORTAL_RADIUS 0.3f
#define PORTAL_APPEAR_SPEED 2.0f
#define PORTAL_DISAPPEAR_SPEED PORTAL_APPEAR_SPEED
#define PORTAL_EXHALE_RATE 0.1f
#define PORTAL_ATTRACT_MULTIPLIER 1.0f
#define PORTAL_EMIT_MULTIPLIER 1.0f

#define OPAQUE (Color){255,255,255,255}
#define DEATH_SOUND_AMOUNT 8

#if DEBUG
#include "stdio.h"
#define dbgi(i) printf("%f Deathbird: %i\n", (float)(GetTime()), i)
#define dbgf(f) printf("%f Deathbird: %f\n", (float)(GetTime()), f)
#else
#define dbgi(i)
#define dbgf(f)
#endif

typedef char int8;
typedef short int16;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint;

typedef enum Tex {
    TEX_AREA_NIGHT_SKY,
    TEX_AREA_OCEAN,
    TEX_AREA_HILLS,
    TEX_AREA_TREES,
    TEX_AREA_DAY_SKY,
    TEX_AREA_CLOUDS,
    TEX_AREA_MEADOWS,
    TEX_AREA_FENCE,
    TEX_AREA_WINTER_SKY,
    TEX_AREA_MOUNTAINS,
    TEX_AREA_WINTER_HILLS,
    TEX_AREA_WINTER_TREES,
    TEX_AREA_SNOW,
    TEX_AREA_WEIRD_WALL,
    TEX_GIANT_BIRD_1,
    TEX_GIANT_BIRD_2,
    TEX_GIANT_BIRD_3,
    TEX_GIANT_BIRD_4,
    TEX_GIANT_BIRD_HEAD,
    TEX_GIANT_BIRD_BODY,
    TEX_GIANT_BIRD_WING,
    TEX_WHITE_BIRD_1,
    TEX_WHITE_BIRD_2,
    TEX_WHITE_BIRD_3,
    TEX_WHITE_BIRD_4,
    TEX_WHITE_BIRD_HEAD,
    TEX_WHITE_BIRD_BODY,
    TEX_WHITE_BIRD_WING,
    TEX_YELLOW_BIRD_1,
    TEX_YELLOW_BIRD_2,
    TEX_YELLOW_BIRD_3,
    TEX_YELLOW_BIRD_4,
    TEX_YELLOW_BIRD_HEAD,
    TEX_YELLOW_BIRD_BODY,
    TEX_YELLOW_BIRD_WING,
    TEX_BROWN_BIRD_1,
    TEX_BROWN_BIRD_2,
    TEX_BROWN_BIRD_3,
    TEX_BROWN_BIRD_4,
    TEX_BROWN_BIRD_HEAD,
    TEX_BROWN_BIRD_BODY,
    TEX_BROWN_BIRD_WING,
    TEX_RED_BIRD_1,
    TEX_RED_BIRD_2,
    TEX_RED_BIRD_3,
    TEX_RED_BIRD_4,
    TEX_RED_BIRD_HEAD,
    TEX_RED_BIRD_BODY,
    TEX_RED_BIRD_WING,
    TEX_BIRD_BLOOD_1,
    TEX_BIRD_BLOOD_2,
    TEX_BIRD_BLOOD_3,
    TEX_BIRD_BLOOD_4,
    TEX_BIRD_EYE,
    TEX_BIRD_GORE_1,
    TEX_BIRD_GORE_2,
    TEX_BIRD_BLOOD_GREEN_1,
    TEX_BIRD_BLOOD_GREEN_2,
    TEX_BIRD_BLOOD_GREEN_3,
    TEX_BIRD_BLOOD_GREEN_4,
    TEX_BIRD_EYE_GREEN,
    TEX_BIRD_GORE_GREEN_1,
    TEX_BIRD_GORE_GREEN_2,
    TEX_PLAYER_1,
    TEX_PLAYER_2,
    TEX_PLAYER_3,
    TEX_TOTAL,
} Tex;

typedef enum Area {
    AREA_NONE = 0,
    AREA_FOREST,
    AREA_MEADOWS,
    AREA_MOUNTAINS,
    AREA_RED_BIRD_NEST,
    AREA_TOTAL,
} Area;

typedef enum Player_State {
    PLAYER_STATE_NONE = 0,
    PLAYER_STATE_EXHALED_BY_PORTAL,
    PLAYER_STATE_GROUNDED,
    PLAYER_STATE_UP,
    PLAYER_STATE_DOWN,
    PLAYER_STATE_INHALED_BY_PORTAL,
    PLAYER_STATE_INSIDE_PORTAL,
} Player_State;

typedef enum Bird_Type {
    BIRD_TYPE_NONE,
    BIRD_TYPE_WHITE,
    BIRD_TYPE_GIANT,
    BIRD_TYPE_YELLOW,
    BIRD_TYPE_BROWN,
    BIRD_TYPE_RED,
    BIRD_TYPES_TOTAL,
} Bird_Type;

typedef enum Bird_State {
    BIRD_STATE_NONE = 0,
    BIRD_STATE_AVAILABLE,
    BIRD_STATE_ALIVE,
    BIRD_STATE_DEAD,
    BIRD_STATE_INHALED_BY_PORTAL,
    BIRD_STATE_INSIDE_PORTAL,
    BIRD_STATE_EXHALED_BY_PORTAL,
} Bird_State;

typedef enum Menu_State {
    MENU_STATE_DEFAULT,
    MENU_STATE_SHOP,
    MENU_STATE_INFO_BOX,
} Menu_State;

typedef enum Fader_State {
    FADER_STATE_NONE = 0,
    FADER_STATE_IN,
    FADER_STATE_IN_COMPLETE,
    FADER_STATE_OUT,
    FADER_STATE_OUT_COMPLETE,
} Fader_State;

typedef enum Portal_Bits {
    PORTAL_BIT_NONE = 0,
    PORTAL_BIT_INHALE = 1 << 0,
    PORTAL_BIT_EXHALE = 1 << 1,
    PORTAL_BIT_RED = 1 << 2,
    PORTAL_BIT_GREEN = 1 << 3,
    PORTAL_BIT_BLUE = 1 << 4,
    PORTAL_BIT_YELLOW = (
        PORTAL_BIT_RED |
        PORTAL_BIT_GREEN
    ),
    PORTAL_BIT_CYAN = (
        PORTAL_BIT_GREEN |
        PORTAL_BIT_BLUE
    ),
    PORTAL_BIT_MAGENTA = (
        PORTAL_BIT_BLUE |
        PORTAL_BIT_RED
    ),
    PORTAL_BIT_WHITE = (
        PORTAL_BIT_RED |
        PORTAL_BIT_GREEN |
        PORTAL_BIT_BLUE
    )
} Portal_Bits;

typedef enum Global_State {
    GLOBAL_STATE_MENU = 0,
    GLOBAL_STATE_MENU_FADE_OUT,
    GLOBAL_STATE_GAME_FADE_IN,
    GLOBAL_STATE_PRE_GAME_PORTAL_APPEAR,
    GLOBAL_STATE_PRE_GAME_PORTAL_EXHALE,
    GLOBAL_STATE_PRE_GAME_PORTAL_DISAPPEAR,
    GLOBAL_STATE_GAME,
    GLOBAL_STATE_POST_GAME_PORTAL_APPEAR,
    GLOBAL_STATE_POST_GAME_PORTAL_INHALE,
    GLOBAL_STATE_POST_GAME_PORTAL_DISAPPEAR,
    GLOBAL_STATE_GAME_FADE_OUT,
    GLOBAL_STATE_MENU_FADE_IN,
} Global_State;

typedef struct Atlas_Texture {
    uint16 x;
    uint16 y;
    uint16 size;
} Atlas_Texture;

typedef struct Texture_Scroller {
    Tex tex;
    uint8 horizontal_textures;
    float horizontal_speed;
    uint8 vertical_textures;
    float vertical_speed;
    Vector2 scroll;
    uint8 opacity;
} Texture_Scroller;

typedef struct Bird {
    Bird_Type type;
    Bird_State state;
    Vector2 position;
    float anim_time;
    union {
        struct {
            uint8 health;
            uint8 current_tex;
            Vector2 collision_bounds;
            float move_speed;
            float damage_timer;
        } alive;
        struct {
            uint8 blood_idx;
            Vector2 velocities[BIRD_DEATH_PARTS];
            Vector2 positions[BIRD_DEATH_PARTS];
            float angular_velocities[BIRD_DEATH_PARTS];
            float rotations[BIRD_DEATH_PARTS];
        } dead;
    };
} Bird;

typedef struct State {
    Global_State global_state;

    uint window_width;
    uint window_height;

    uint game_width;
    uint game_height;
    uint game_left;
    uint game_right;
    uint game_top;
    uint game_bottom;
    uint game_center_x;
    uint game_center_y;

    Sound sounds_death_splats[DEATH_SOUND_AMOUNT];

    Texture atlas;
    Atlas_Texture atlas_textures[TEX_TOTAL];

    uint16 areas_discovered;

    Texture_Scroller tex_scrollers[8];

    uint level_idx;
    uint level_score;

    struct {
        Area area;
        uint8 tex_scroller_amount;
        uint required_score;
        uint total_birds;
        float bird_frequency;
        float bird_timer;
        uint requested_birds;
        uint spawned_birds;
        uint passed_birds;
    } level_current_data, next_level_data;

    Menu_State menu_state;
    Font menu_font;
    uint8 menu_option_idx;
    uint8 menu_option_count;
    uint8 menu_option_area_offset;
    uint8 menu_option_idx_results;
    uint8 menu_option_idx_continue;
    uint8 menu_option_idx_multipliers;

    Player_State player_state;
    Vector2 player_position;
    float player_rotation;
    uint16 player_current_input;

    Bird birds[BIRD_CAPACITY];
    Shader bird_damage_shader;
    uint birds_in_use;
    uint bird_multiplier;
    uint bird_multiplier_display;
    float bird_multiplier_timer;
    Bird_Type birds_destroyed[BIRD_TYPES_TOTAL];
    uint bird_highest_multipliers[MENU_LINE_COUNT];

    Fader_State fader_state;
    float fader_level;

    Shader portal_shader;
    Portal_Bits portal_bits;
    Texture portal_texture;
    float portal_linear_disappearance;
    float portal_smooth_disappearance;
    Color portal_color;
    float portal_timer;

    float scale_multiplier;
    float delta_time;
} State;

#if DEBUG
Vector2 to_pixel_position(State *state, Vector2 game_position);
Vector2 to_pixel_size(State *state, Vector2 game_size);
#endif

Rectangle game_rectangle(State *state);
bool has_flag(int flags, int flag);
float randf(float min, float max);

Vector2 vec2_normalized(float x, float y);
Vector2 vec2_direction(Vector2 from, Vector2 to);
float vec2_distance(Vector2 a, Vector2 b);

void atlas_draw(State *state, Tex tex, Vector2 position, float rotation, float scale, Color color);
void atlas_draw_raw(State *state, Tex tex, Vector2 position, float rotation, float scale, Vector2 origin, Color color);

Vector2 portal_get_position(State *state);
float portal_distance_to_center_ratio(State *state, Vector2 position);
bool portal_inhale_object(float *obj_position, float obj_step, float portal_position);

bool bird_try_destroy(State *state, Bird *bird, Vector2 from);

#endif