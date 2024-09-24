#ifndef MAIN_H
#define MAIN_H

#include "raylib.h"

// def game
#define GAME_WIDTH_RATIO 3.0f
#define GAME_HEIGHT_RATIO 2.0f
#define GAME_MIN_SIZE 2

#define PLAYER_ANIM_TEX_AMOUNT 1
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

#define OUT_OF_BOUNDS_COLOR BLACK
#define GROUND_Y -0.97f
#define CEILING_Y 0.97f
#define ENV_TEX_SIZE 128.0f

// def tex atlas
#define ATLAS_BLOCK_SIZE 128
#define ATLAS_PADDING 1

// def scroll env
#define SCROLL_ENV_AMOUNT 4

// def player
#define PLAYER_GRAVITY 3.0f
#define PLAYER_GROUND_LOSS 0.8f
#define PLAYER_BIRD_BOUNCE 3.0f
#define PLAYER_HORIZONTAL_SPEED 2.0f
#define PLAYER_ROTATION_SPEED 5.0f
#define PLAYER_ANIM_SPEED 0.1f

// def bird
#define BIRD_AMOUNT 100
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
#define BIRD_COMPUTER_FONT "raster.ttf"
#define BIRD_COMPUTER_FONT_SIZE 15
#define BIRD_COMPUTER_BG_COLOR (Color) { 0, 0, 192, 255 }
#define BIRD_COMPUTER_FG_COLOR (Color) { 255, 255, 255, 255 }
#define BIRD_COMPUTER_TEXT_COLOR (Color) { 255, 255, 0, 255 }
#define BIRD_COMPUTER_HEADER_SIZE 0.1
#define BIRD_COMPUTER_OPTIONS 7
#define BIRD_COMPUTER_OPTION_NAME_MAX_LENGTH 48

typedef enum Game_State {
    GAME_STATE_NEXT_LEVEL = 0,
    GAME_STATE_DEATHBIRD,
    GAME_STATE_BIRD_COMPUTER,
} Game_State;

typedef enum Game_Level {
    GAME_LEVEL_FOREST,
    GAME_LEVEL_FIELD,
} Game_Level;

typedef enum Tex {
    TEX_ENV_NIGHT_SKY,
    TEX_ENV_OCEAN,
    TEX_ENV_HILLS,
    TEX_ENV_TREES,
    TEX_ENV_DAY_SKY,
    TEX_ENV_CLOUDS,
    TEX_ENV_FIELD,
    TEX_ENV_FENCE,
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
    TEX_BIRD_BLOOD_1,
    TEX_BIRD_BLOOD_2,
    TEX_BIRD_BLOOD_3,
    TEX_BIRD_BLOOD_4,
    TEX_BIRD_EYE,
    TEX_BIRD_GORE_1,
    TEX_BIRD_GORE_2,
    TEX_TOTAL,
} Tex;

typedef enum Bird_Computer_State {
    BIRD_COMPUTER_STATE_INACTIVE,
    BIRD_COMPUTER_STATE_ACTIVE,
    BIRD_COMPUTER_STATE_CONTINUE_TO_NEXT_LEVEL,
} Bird_Computer_State;

typedef enum Scroll_Type {
    SCROLL_TYPE_RIGHT,
    SCROLL_TYPE_LEFT,
    SCROLL_TYPE_DOWN,
    SCROLL_TYPE_UP,
} Scroll_Type;

typedef enum Bird_Type {
    BIRD_TYPE_WHITE,
    BIRD_TYPE_GIANT,
    BIRD_TYPE_YELLOW,
} Bird_Type;

typedef enum Bird_State {
    BIRD_STATE_NONE,
    BIRD_STATE_ALIVE,
    BIRD_STATE_DYING,
    BIRD_STATE_DEAD,
    BIRD_STATE_RESET,
} Bird_State;

typedef struct Tex_Atlas_Offset {
    int x;
    int y;
    int size;
} Tex_Atlas_Offset;

typedef struct State {
    Game_State game_state;
    Game_Level game_level;
    Texture tex_atlas;
    Tex_Atlas_Offset tex_atlas_offsets[TEX_TOTAL];
    float scale_multiplier;
    int game_width;
    int game_height;
    int game_left;
    int game_right;
    int game_top;
    int game_bottom;
    int game_center_x;
    int game_center_y;
    int screen_width;
    int screen_height;
    Sound sounds_death_splats[DEATH_SOUND_AMOUNT];
    int bird_amount;
    int bird_counter;
    int birds_passed;
    float bird_rate;
    float bird_timer;
} State;

typedef struct Bird_Computer_Option {
    char name[BIRD_COMPUTER_OPTION_NAME_MAX_LENGTH];
    int name_len;
} Bird_Computer_Option;

typedef struct Bird_Computer {
    Font font;
    int option_line;
    int option_line_count;
    Bird_Computer_Option options[BIRD_COMPUTER_OPTIONS];
} Bird_Computer;

typedef struct Scroll_Env {
    Tex tex;
    Scroll_Type type;
    float speed;
    int vertical_amount;
    int horizontal_amount;
    Vector2 offset;
    Vector2 scroll;
} Scroll_Env;

typedef struct Player {
    Vector2 position;
    float velocity;
    int damage;
    int current_key;
    int anim_step;
    float anim_time;
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

float randf(float min, float max);
Vector2 vec2_normalized(float x, float y);
void tex_atlas_setup(State *state);
void tex_atlas_draw(State *state, Tex tex, Vector2 position, float rotation);
void scroll_env_render(State *state, Scroll_Env *env);

#endif // MAIN_H