#ifndef MAIN_H
#define MAIN_H

#include "raylib.h"

// def game
#define GAME_WIDTH_RATIO 3.0f
#define GAME_HEIGHT_RATIO 2.0f
#define GAME_MIN_SIZE 2

#define LEVEL_BASE_BIRD_TOTAL 10
#define LEVEL_MIN_RANDOM_BIRD_AMOUNT 0
#define LEVEL_MAX_RANDOM_BIRD_AMOUNT 10
#define LEVEL_BIRD_AMOUNT_MULTIPLIER 5
#define LEVEL_MIN_BIRD_FREQUENCY 0.1f
#define LEVEL_MAX_BIRD_FREQUENCY 0.3f
#define LEVELS_BEFORE_MIN_FREQUENCY 31
#define LEVEL_MIN_RANDOM_BIRD_FREQUENCY 0.0f
#define LEVEL_MAX_RANDOM_BIRD_FREQUENCY 0.1f

#define PLAYER_ANIM_TEX_AMOUNT 6
#define DEATH_SOUND_AMOUNT 8

// def debug
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

#define OPAQUE 255
#define OUT_OF_BOUNDS_COLOR BLACK
#define GROUND_Y -0.87f
#define CEILING_Y 0.97f
#define ENV_TEX_SIZE 128.0f

// def tex atlas
#define ATLAS_BLOCK_SIZE 128
#define ATLAS_PADDING 1

// def scroll env
#define SCROLL_ENV_CAPACITY 8

// def player
#define PLAYER_VERTICAL_SPEED 2.0f
#define PLAYER_HORIZONTAL_SPEED 2.0f
#define PLAYER_ROTATION_SPEED 500.0f
#define PLAYER_ROTATION_SPEED_FAST 2500.0f
#define PLAYER_SMALLEST_VALID_MULTIPLIER 2
#define PLAYER_MULTIPLIER_DISPLAY_TIME 1.0f

// def bird
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
#define BIRD_DEATH_VELOCITY_MULTIPLIER 0.8f
#define BIRD_DEATH_MAX_RANDOM_VELOCITY 1.4f
#define BIRD_DEATH_GROUND_BOUNCE_VELOCITY_THRESHOLD 2.0f
#define BIRD_DEATH_GROUND_BOUNCE_VELOCITY -0.4f
#define BIRD_DEATH_MAX_RANDOM_ANGULAR_VELOCITY 1400.0f
#define BIRD_RESET_RIGHT 1.2f
#define BIRD_RESET_LEFT -BIRD_RESET_RIGHT
#define BIRD_VERTICAL_FREEDOM 0.8f

// def bird computer
#define BIRD_COMPUTER_FONT "consolas.ttf"
#define BIRD_COMPUTER_FONT_SIZE 350
#define BIRD_COMPUTER_BG_COLOR (Color) { 0, 0, 192, 255 }
#define BIRD_COMPUTER_CURSOR_BG_COLOR (Color) { 255, 0, 0, 255 }
#define BIRD_COMPUTER_FG_COLOR (Color) { 255, 255, 255, 255 }
#define BIRD_COMPUTER_TEXT_COLOR (Color) { 255, 255, 0, 255 }
#define BIRD_COMPUTER_HEADER_SIZE 0.1
#define BIRD_COMPUTER_LINE_COUNT 8
#define BIRD_COMPUTER_OPTION_NAME_MAX_LENGTH 48
#define BIRD_COMPUTER_SHOP_MAX_ITEM_COUNT 16

typedef enum Game_State {
    GAME_STATE_BIRD_COMPUTER = 0,
    GAME_STATE_NEXT_LEVEL,
    GAME_STATE_DEATHBIRD,
} Game_State;

typedef enum Level_Environment {
    LEVEL_ENVIRONMENT_NONE = 0,
    LEVEL_ENVIRONMENT_FOREST,
    LEVEL_ENVIRONMENT_MEADOWS,
    LEVEL_ENVIRONMENT_MOUNTAINS,
    LEVEL_ENVIRONMENT_TOTAL,
} Level_Environment;

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
    TEX_TOTAL,
} Tex;

typedef enum Bird_Computer_State {
    BIRD_COMPUTER_STATE_DEFAULT,
    BIRD_COMPUTER_STATE_SHOP,
    BIRD_COMPUTER_STATE_INFO_BOX,
} Bird_Computer_State;

typedef enum Shop_Item {
    SHOP_ITEM_NONE = 0,
    SHOP_ITEM_PLAYER_1,
} Shop_Item;

typedef enum Player_State {
    PLAYER_STATE_GROUNDED,
    PLAYER_STATE_UP,
    PLAYER_STATE_DOWN,
} Player_State;

typedef enum Bird_Type {
    BIRD_TYPE_WHITE,
    BIRD_TYPE_GIANT,
    BIRD_TYPE_YELLOW,
    BIRD_TYPE_BROWN,
    BIRD_TYPES_TOTAL,
} Bird_Type;

typedef enum Bird_State {
    BIRD_STATE_AVAILABLE,
    BIRD_STATE_ALIVE,
    BIRD_STATE_DYING,
    BIRD_STATE_DEAD,
} Bird_State;

typedef struct Tex_Atlas_Offset {
    int x;
    int y;
    int size;
} Tex_Atlas_Offset;

typedef struct Shop {
    Shop_Item items
        [LEVEL_ENVIRONMENT_TOTAL]
        [BIRD_COMPUTER_SHOP_MAX_ITEM_COUNT];
} Shop;

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
    Shop shop;
    int option_idx;
    int option_count;
    int option_area_offset;
    int option_idx_results;
    int option_idx_continue;
    int option_idx_shop;
    int option_idx_multipliers;
} Bird_Computer;

typedef struct Scroll_Env {
    Tex tex;
    int horizontal_textures;
    float horizontal_speed;
    int vertical_textures;
    float vertical_speed;
    Vector2 scroll;
    unsigned char opacity;
} Scroll_Env;

typedef struct Player {
    Player_State state;
    Vector2 position;
    float rotation;
    int damage;
    int current_input_key;
    int anim_step;
    float anim_time;
    int level_score;
    Bird_Type obliterated_birds[BIRD_TYPES_TOTAL];
    int bird_multiplier;
    int bird_multiplier_display;
    float bird_multiplier_timer;
    int highest_multipliers[BIRD_COMPUTER_LINE_COUNT]; // exclude 1x multiplier
} Player;

typedef struct Bird {
    Bird_Type type;
    Bird_State state;
    int health;
    float collision_radius;
    float move_speed;
    int current_tex;
    float anim_time;
    Vector2 position;
    Vector2 death_master_velocity;
    Vector2 death_velocities[BIRD_DEATH_PARTS];
    Vector2 death_positions[BIRD_DEATH_PARTS];
    float death_angular_velocities[BIRD_DEATH_PARTS];
    float death_rotations[BIRD_DEATH_PARTS];
    int blood_idx;
} Bird;

typedef struct Level_Data {
    Level_Environment environment;
    int scroll_env_amount;
    int total_birds;
    float bird_frequency;
    float bird_timer;
    int requested_birds;
    int spawned_birds;
    int passed_birds;
} Level_Data;

typedef struct State {
    int screen_width;
    int screen_height;

    Game_State game_state;
    int game_width;
    int game_height;
    int game_left;
    int game_right;
    int game_top;
    int game_bottom;
    int game_center_x;
    int game_center_y;

    int current_round;
    Level_Data current_level_data;
    Level_Data next_level_data;
    Scroll_Env scroll_envs[SCROLL_ENV_CAPACITY];

    Bird_Computer bird_computer;

    Texture tex_atlas;
    Tex_Atlas_Offset tex_atlas_offsets[TEX_TOTAL];

    Sound sounds_death_splats[DEATH_SOUND_AMOUNT];

    Player player;

    Bird birds[BIRD_CAPACITY];
    int birds_in_use;

    float scale_multiplier;
    float delta_time;
} State;

float randf(float min, float max);
Vector2 vec2_normalized(float x, float y);
void tex_atlas_init(State *state);
void tex_atlas_draw(State *state, Tex tex, Vector2 position, float rotation, unsigned char opacity);
void level_render(State *state);

#endif // MAIN_H