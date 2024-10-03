#ifndef MAIN_H
#define MAIN_H

#include "raylib.h"

#define GAME_WIDTH_RATIO 3.0f
#define GAME_HEIGHT_RATIO 2.0f
#define GAME_MIN_SIZE 2
#define GAME_GROUND_Y -0.87f
#define GAME_CEILING_Y 0.97f
#define GAME_OUT_OF_BOUNDS_COLOR BLACK

#define ATLAS_PADDING 1

#define LEVEL_ENV_TEX_SIZE 128.0f
#define LEVEL_BASE_BIRD_TOTAL 40
#define LEVEL_MIN_RANDOM_BIRD_AMOUNT 0
#define LEVEL_MAX_RANDOM_BIRD_AMOUNT 10
#define LEVEL_BIRD_AMOUNT_MULTIPLIER 5
#define LEVEL_MIN_BIRD_FREQUENCY 0.1f
#define LEVEL_MAX_BIRD_FREQUENCY 0.3f
#define LEVELS_BEFORE_MIN_FREQUENCY 31
#define LEVEL_MIN_RANDOM_BIRD_FREQUENCY 0.0f
#define LEVEL_MAX_RANDOM_BIRD_FREQUENCY 0.1f
#define LEVEL_CURRENT_ROUND_TEXT_BG_COLOR ((Color) { 0, 0, 0, 255 })
#define LEVEL_CURRENT_ROUND_TEXT_COLOR ((Color) { 255, 255, 255, 255 })
#define LEVEL_SCORE_BG_COLOR ((Color) { 0, 0, 0, 128 })
#define LEVEL_SCORE_NOT_REACHED_FG_COLOR ((Color) { 0, 255, 0, 128 })
#define LEVEL_SCORE_REACHED_FG_COLOR ((Color) { 0, 128, 255, 128 })
#define LEVEL_SCORE_TEXT_COLOR ((Color) { 255, 255, 255, 255 })
#define LEVEL_MULTIPLIER_TEXT_BG_COLOR ((Color) { 255, 0, 0, 255 })
#define LEVEL_MULTIPLIER_TEXT_COLOR ((Color) { 255, 255, 255, 255 })

#define PLAYER_VERTICAL_SPEED 1.5f
#define PLAYER_HORIZONTAL_SPEED 2.0f
#define PLAYER_ROTATION_SPEED 500.0f
#define PLAYER_ROTATION_SPEED_FAST 2500.0f
#define PLAYER_SMALLEST_VALID_MULTIPLIER 2
#define PLAYER_MULTIPLIER_DISPLAY_TIME 1.0f

#define BIRD_CAPACITY 32
#define BIRD_DEATH_BODY_PARTS 6 // (head) + (body) + (2 wings) + (2 eyes)
#define BIRD_DEATH_GORE_PARTS 8
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
#define BIRD_DEATH_MAX_RANDOM_VELOCITY 1.4f
#define BIRD_DEATH_GROUND_BOUNCE_VELOCITY_THRESHOLD 2.0f
#define BIRD_DEATH_GROUND_BOUNCE_VELOCITY -0.4f
#define BIRD_DEATH_MAX_RANDOM_ANGULAR_VELOCITY 1400.0f
#define BIRD_RESET_RIGHT 1.2f
#define BIRD_RESET_LEFT -BIRD_RESET_RIGHT
#define BIRD_VERTICAL_FREEDOM 0.7f
#define BIRD_GIANT_SPLASH_RADIUS 1.2f

#define BIRD_COMPUTER_FONT "bios.ttf"
#define BIRD_COMPUTER_FONT_SIZE 250
#define BIRD_COMPUTER_BG_COLOR ((Color) { 0, 0, 192, 255 })
#define BIRD_COMPUTER_CURSOR_BG_COLOR ((Color) { 255, 0, 0, 255 })
#define BIRD_COMPUTER_FG_COLOR ((Color) { 255, 255, 255, 255 })
#define BIRD_COMPUTER_TEXT_COLOR ((Color) { 255, 255, 0, 255 })
#define BIRD_COMPUTER_ACTIVE_LEVEL_COLOR ((Color) { 0, 255, 0, 255 })
#define BIRD_COMPUTER_HEADER_SIZE 0.1
#define BIRD_COMPUTER_LINE_COUNT 8
#define BIRD_COMPUTER_OPTION_NAME_MAX_LENGTH 48
#define BIRD_COMPUTER_SHOP_CAPACITY 32

#define FADER_MAX 255
#define FADER_MIN 0
#define FADER_SPEED 500.0f

#define OPAQUE 255
#define DEATH_SOUND_AMOUNT 8

#ifdef DEBUG
#include "stdio.h"
#define dbgi(i) printf("DEATHBIRD: %i\n", i)
#define dbgf(f) printf("DEATHBIRD: %f\n", f)
#define dbgs(s) printf("DEATHBIRD: %s\n", s)
#else
#define dbgi(i)
#define dbgf(f)
#define dbgs(s)
#endif

typedef char int8;
typedef short int16;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint;

typedef enum Game_State {
    GAME_STATE_BIRD_COMPUTER = 0,
    GAME_STATE_BIRD_COMPUTER_FADE_OUT,
    GAME_STATE_DEATHBIRD_FADE_IN,
    GAME_STATE_DEATHBIRD,
    GAME_STATE_DEATHBIRD_FADE_OUT,
    GAME_STATE_BIRD_COMPUTER_FADE_IN,
} Game_State;

typedef enum Tex {
    TEX_ENV_NIGHT_SKY,
    TEX_ENV_OCEAN,
    TEX_ENV_HILLS,
    TEX_ENV_TREES,
    TEX_ENV_DAY_SKY,
    TEX_ENV_CLOUDS,
    TEX_ENV_MEADOWS,
    TEX_ENV_FENCE,
    TEX_ENV_WINTER_SKY,
    TEX_ENV_MOUNTAINS,
    TEX_ENV_WINTER_HILLS,
    TEX_ENV_WINTER_TREES,
    TEX_ENV_SNOW,
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
    TEX_BIRD_BLOOD_1,
    TEX_BIRD_BLOOD_2,
    TEX_BIRD_BLOOD_3,
    TEX_BIRD_BLOOD_4,
    TEX_BIRD_EYE,
    TEX_BIRD_GORE_1,
    TEX_BIRD_GORE_2,
    TEX_PLAYER_1,
    TEX_PLAYER_2,
    TEX_PLAYER_3,
    TEX_TOTAL,
} Tex;

typedef enum Level_Environment {
    LEVEL_ENVIRONMENT_NONE = 0,
    LEVEL_ENVIRONMENT_FOREST,
    LEVEL_ENVIRONMENT_MEADOWS,
    LEVEL_ENVIRONMENT_MOUNTAINS,
    LEVEL_ENVIRONMENT_TOTAL,
} Level_Environment;

typedef enum Player_State {
    PLAYER_STATE_GROUNDED,
    PLAYER_STATE_UP,
    PLAYER_STATE_DOWN,
} Player_State;

typedef enum Bird_State {
    BIRD_STATE_AVAILABLE,
    BIRD_STATE_ALIVE,
    BIRD_STATE_DEAD,
} Bird_State;

typedef enum Bird_Type {
    BIRD_TYPE_WHITE,
    BIRD_TYPE_GIANT,
    BIRD_TYPE_YELLOW,
    BIRD_TYPE_BROWN,
    BIRD_TYPES_TOTAL,
} Bird_Type;

typedef enum Bird_Computer_State {
    BIRD_COMPUTER_STATE_DEFAULT,
    BIRD_COMPUTER_STATE_SHOP,
    BIRD_COMPUTER_STATE_INFO_BOX,
} Bird_Computer_State;

typedef enum Shop_Item_Type {
    SHOP_ITEM_SIMPLE,
    SHOP_ITEM_PLAYABLE_1,
    SHOP_ITEM_PLAYABLE_2,
    SHOP_ITEM_PLAYABLE_3,
} Shop_Item_Type;

typedef enum Fader_State {
    FADER_STATE_NONE = 0,
    FADER_STATE_IN,
    FADER_STATE_IN_COMPLETE,
    FADER_STATE_OUT,
    FADER_STATE_OUT_COMPLETE,
} Fader_State;

typedef struct Tex_Atlas_Offset {
    int16 x;
    int16 y;
    int16 size;
} Tex_Atlas_Offset;

typedef struct Scroll_Env {
    Tex tex;
    uint8 horizontal_textures;
    float horizontal_speed;
    uint8 vertical_textures;
    float vertical_speed;
    Vector2 scroll;
    uint8 opacity;
} Scroll_Env;

typedef struct Level_Data {
    Level_Environment environment;
    uint8 scroll_env_amount;
    uint required_score;
    uint total_birds;
    float bird_frequency;
    float bird_timer;
    uint requested_birds;
    uint spawned_birds;
    uint passed_birds;
} Level_Data;

typedef struct Player {
    Player_State state;
    Vector2 position;
    float rotation;
    uint8 damage;
    uint16 current_input_key;
    uint level_score;
    Bird_Type obliterated_birds[BIRD_TYPES_TOTAL];
    uint bird_multiplier;
    uint bird_multiplier_display;
    float bird_multiplier_timer;
    uint highest_multipliers[BIRD_COMPUTER_LINE_COUNT]; // exclude 1x multiplier
} Player;

typedef struct Bird {
    Bird_Type type;
    Bird_State state;
    uint8 health;
    uint8 blood_idx;
    uint8 current_tex;
    float collision_radius;
    float move_speed;
    float anim_time;
    float damage_timer;
    Vector2 position;
    Vector2 death_velocities[BIRD_DEATH_PARTS];
    Vector2 death_positions[BIRD_DEATH_PARTS];
    float death_angular_velocities[BIRD_DEATH_PARTS];
    float death_rotations[BIRD_DEATH_PARTS];
} Bird;

typedef struct Bird_Computer_Dimensions {
    float font_size;
    float x_fract;
    float y_fract;
    float header_size;
    float line_section_size;
    float footer_size;
    float line_size;
} Bird_Computer_Dimensions;

typedef struct Bird_Computer {
    Bird_Computer_State state;
    Font font;
    uint8 option_idx;
    uint8 option_count;
    uint8 option_area_offset;
    uint8 option_idx_results;
    uint8 option_idx_continue;
    uint8 option_idx_multipliers;
    uint16 discovered_levels;
} Bird_Computer;

typedef struct Fader {
    Fader_State state;
    float fade_level;
} Fader;

typedef struct State {
    uint screen_width;
    uint screen_height;

    Game_State game_state;
    uint game_width;
    uint game_height;
    uint game_left;
    uint game_right;
    uint game_top;
    uint game_bottom;
    uint game_center_x;
    uint game_center_y;

    uint current_round;
    Level_Data current_level_data;
    Level_Data next_level_data;
    Scroll_Env scroll_envs[8];

    Bird_Computer bird_computer;

    Texture tex_atlas;
    Shader bird_damage_shader;
    Tex_Atlas_Offset tex_atlas_offsets[TEX_TOTAL];

    Sound sounds_death_splats[DEATH_SOUND_AMOUNT];

    Player player;

    Bird birds[BIRD_CAPACITY];
    uint birds_in_use;

    Fader fader;

    float scale_multiplier;
    float delta_time;
} State;

float randf(float min, float max);
Vector2 vec2_normalized(float x, float y);
void tex_atlas_init(State *state);
void tex_atlas_draw(State *state, Tex tex, Vector2 position, float rotation, unsigned char opacity);
bool bird_try_destroy(State *state, Bird *bird, Vector2 from);

#endif