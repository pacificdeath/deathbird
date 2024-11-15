#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#include "raylib.h"

#define GAME_WIDTH_RATIO 3.0f
#define GAME_HEIGHT_RATIO 2.0f
#define GAME_MIN_SIZE 4
#define GAME_GROUND_Y -0.87f
#define GAME_CEILING_Y 0.97f
#define GAME_OUT_OF_BOUNDS_COLOR BLACK
#define GAME_ANIM_SPEED 0.1f

#define ATLAS_PADDING 1

#define BIRD_CAPACITY 32
#define BIRD_DEATH_BODY_PARTS 6 // (head) + (body) + (2 wings) + (2 eyes)
#define BIRD_DEATH_GORE_PARTS 8
#define BIRD_DEATH_PARTS (BIRD_DEATH_BODY_PARTS + BIRD_DEATH_GORE_PARTS)

#define MENU_FONT "bios.ttf"
#define MENU_FONT_SIZE 250
#define MENU_BG_OVERLAY_COLOR ((Color) { 0, 0, 0, 128 })
#define MENU_BG_COLOR ((Color) { 0, 0, 192, 192 })
#define MENU_CURSOR_BG_COLOR ((Color) { 255, 0, 0, 192 })
#define MENU_HIGHLIGHT_COLOR ((Color) { 0, 255, 0, 192 })
#define MENU_FG_COLOR ((Color) { 255, 255, 255, 192 })
#define MENU_TEXT_COLOR ((Color) { 255, 255, 0, 192 })
#define MENU_ACTIVE_LEVEL_COLOR ((Color) { 0, 255, 0, 192 })
#define MENU_LINE_COUNT 8

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

typedef int8_t int8;
typedef int16_t int16;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint;

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
    TEX_AREA_RED_SKY,
    TEX_AREA_BIG_TREES_1,
    TEX_AREA_BIG_TREES_2,
    TEX_AREA_BIG_TREES_3,
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
    TEX_UMBRELLA_BIRD_ABOVE_1,
    TEX_UMBRELLA_BIRD_ABOVE_2,
    TEX_UMBRELLA_BIRD_ABOVE_3,
    TEX_UMBRELLA_BIRD_ABOVE_4,
    TEX_UMBRELLA_BIRD_UNDER_1,
    TEX_UMBRELLA_BIRD_UNDER_2,
    TEX_UMBRELLA_BIRD_UNDER_3,
    TEX_UMBRELLA_BIRD_UNDER_4,
    TEX_UMBRELLA,
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
    TEX_BLACK_BIRD_1,
    TEX_BLACK_BIRD_2,
    TEX_BLACK_BIRD_3,
    TEX_BLACK_BIRD_4,
    TEX_BLACK_BIRD_HEAD,
    TEX_BLACK_BIRD_BODY,
    TEX_BLACK_BIRD_WING,
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
    AREA_DEEP_FOREST,
    AREA_TOTAL,
} Area;

typedef enum Shop_Item {
    SHOP_ITEM_BIRD_FOOD,
    SHOP_ITEM_BOOST_OF_HASTE,
    SHOP_ITEM_TOTAL
} Shop_Item;

typedef enum Shop_Item_State {
    SHOP_ITEM_STATE_UNAVAILABLE = 0,
    SHOP_ITEM_STATE_AVAILABLE,
    SHOP_ITEM_STATE_OWNED,
} Shop_Item_State;

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
    ),
    PORTAL_BIT_BLACK = 0,
} Portal_Bits;

typedef enum Global_State {
    GLOBAL_STATE_MENU = 0,
    GLOBAL_STATE_FADE_OUT,
    GLOBAL_STATE_FADE_IN,
    GLOBAL_STATE_PRE_GAME_PORTAL_APPEAR,
    GLOBAL_STATE_PRE_GAME_PORTAL_EXHALE,
    GLOBAL_STATE_PRE_GAME_PORTAL_DISAPPEAR,
    GLOBAL_STATE_GAME,
    GLOBAL_STATE_POST_GAME_PORTAL_APPEAR,
    GLOBAL_STATE_POST_GAME_PORTAL_INHALE,
    GLOBAL_STATE_POST_GAME_PORTAL_DISAPPEAR,
} Global_State;

typedef enum Fader_State {
    FADER_STATE_NONE = 0,
    FADER_STATE_IN,
    FADER_STATE_IN_COMPLETE,
    FADER_STATE_OUT,
    FADER_STATE_OUT_COMPLETE,
} Fader_State;

typedef enum Menu_State {
    MENU_STATE_NOTIFICATION = 0,
    MENU_STATE_DEFAULT,
    MENU_STATE_LEVEL_SELECT,
    MENU_STATE_SHOP,
    MENU_STATE_SHOP_BUY_NO,
    MENU_STATE_SHOP_BUY_YES,
    MENU_STATE_SHOP_TOO_EXPENSIVE,
    MENU_STATE_GAME_OVER,
} Menu_State;

typedef enum Player_State {
    PLAYER_STATE_NONE = 0,
    PLAYER_STATE_EXHALED_BY_PORTAL,
    PLAYER_STATE_GROUNDED,
    PLAYER_STATE_UP,
    PLAYER_STATE_DOWN,
    PLAYER_STATE_INHALED_BY_PORTAL,
    PLAYER_STATE_INSIDE_PORTAL,
} Player_State;

typedef enum Bird_State {
    BIRD_STATE_NONE = 0,
    BIRD_STATE_AVAILABLE,
    BIRD_STATE_ALIVE,
    BIRD_STATE_DEAD,
    BIRD_STATE_INHALED_BY_PORTAL,
    BIRD_STATE_INSIDE_PORTAL,
    BIRD_STATE_EXHALED_BY_PORTAL,
} Bird_State;

typedef enum Bird_Type {
    BIRD_TYPE_NONE,
    BIRD_TYPE_WHITE,
    BIRD_TYPE_GIANT,
    BIRD_TYPE_YELLOW,
    BIRD_TYPE_UMBRELLA_ABOVE,
    BIRD_TYPE_UMBRELLA_UNDER,
    BIRD_TYPE_BROWN,
    BIRD_TYPE_RED,
    BIRD_TYPES_TOTAL,
} Bird_Type;

typedef enum Menu_Notification {
    MENU_NOTIFICATION_NONE,
    MENU_NOTIFICATION_RED_BIRD_DESTROYED,
    MENU_NOTIFICATION_TOTAL
} Menu_Notification;

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

typedef struct Menu {
    uint16 width;
    uint16 height;
    uint16 left;
    uint16 right;
    uint16 top;
    uint16 bottom;
    uint16 center_x;
    uint16 center_y;

    float font_size;
    float x_fract;
    float y_fract;
    float header_size;
    float line_section_size;
    float line_size;
    float footer_size;
    float decor_line_size;

    Menu_State state;
    Font font;
    Menu_Notification notifications;
    Tex anim[4];
    uint16 anim_frame;
    float anim_timer;
    uint available_options;
    uint8 option_idx;
    uint8 option_count;
    uint8 option_offset;
    uint8 suboption_idx;
    uint8 suboption_offset;
    uint8 suboption_count;
    Shop_Item_State shop_items[SHOP_ITEM_TOTAL];
    uint8 boost_of_haste_level;
    uint8 bird_food_level;
    uint uranium;
} Menu;

typedef struct Player {
    Player_State state;
    float home_position;
    Vector2 spinner_position;
    float rotation;
    uint16 current_input;
    float last_turnaround_y;
    int16 may_not_interact_with_this_bird_idx;
    float vertical_speed;
    float horizontal_speed;
} Player;

typedef struct Bird {
    Bird_State state;
    Bird_Type type;
    Vector2 position;
    float anim_time;
    union {
        struct {
            int health;
            int current_tex;
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

    uint16 window_width;
    uint16 window_height;

    uint16 game_width;
    uint16 game_height;
    uint16 game_left;
    uint16 game_right;
    uint16 game_top;
    uint16 game_bottom;
    uint16 game_center_x;
    uint16 game_center_y;

    Sound sounds_death_splats[DEATH_SOUND_AMOUNT];

    Texture atlas;
    Atlas_Texture atlas_textures[TEX_TOTAL];

    uint16 areas_discovered_bits;

    Texture_Scroller tex_scrollers[8];

    uint level_idx;
    uint level_score;

    struct {
        Area area;
        uint8 tex_scroller_amount;
        float bird_frequency;
        float bird_timer;
    } level_current_data, next_level_data;

    Menu menu;

    Player player;

    Bird birds[BIRD_CAPACITY];
    Shader bird_damage_shader;
    uint birds_requested;
    uint birds_in_use;
    uint bird_multiplier;
    uint bird_multiplier_display;
    float bird_multiplier_timer;
    Bird_Type birds_destroyed[BIRD_TYPES_TOTAL];
    uint bird_highest_multipliers[MENU_LINE_COUNT];

    uint red_bird_spawn_idx;
    uint8 red_birds_destroyed_bits;

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

bool bird_try_destroy_by_player(State *state, Bird *bird);

#endif