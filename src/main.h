/*
 * 1. Forest Night      (Regular)                   White
 * 2. Meadows           (Umbrella)                  Yellow
 * 3. Beach             (Umbrella 2)                Blue
 * 4. Deep forest       (???)                       Black
 * 5. Icy Mountain      (Giant bird)                Brown
 * 6. Toxic             (Bomb bird + Umbrella)      Red
 * 7. Castle            (???)                       Green
 * 8. Blackness         (???)                       Lines
 * */

#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#include "../raylib/include/raylib.h"

#ifdef DEBUG
    #include <stdio.h>
    #define ASSERT(condition)\
        do {\
            if (!(condition)) {\
                printf("You are a horrible person\n -> %s:%i", __FILE__, __LINE__);\
                exit(1);\
            }\
        } while (0)
#else
    #define ASSERT(condition)
#endif

#define GAME_WIDTH_RATIO 3.0f
#define GAME_HEIGHT_RATIO 2.0f
#define GAME_MIN_SIZE 4
#define GAME_GROUND_Y -0.9f
#define GAME_CEILING_Y 0.99f
#define GAME_OUT_OF_BOUNDS_COLOR BLACK
#define GAME_ANIM_SPEED 0.1f

#define SPRITE_SCROLLER_CAP 8

#define BIRD_CAPACITY 1024
#define BIRD_DEATH_BODY_PARTS 6 // (head) + (body) + (2 wings) + (2 eyes)
#define BIRD_DEATH_GORE_PARTS 16
#define BIRD_DEATH_PARTS (BIRD_DEATH_BODY_PARTS + BIRD_DEATH_GORE_PARTS)
#define BIRD_Y_SECTIONS 7

#define TERMINAL_LINE_CAPACITY 32
#define TERMINAL_LINE_MAX_LENGTH 64
#define TERMINAL_BG_COLOR ((Color){0,0,0,255})
#define TERMINAL_FG_COLOR ((Color){255,255,255,255})
#define TERMINAL_FUZZY_COLOR ((Color){0,128,128,255})

#define MENU_FONT "bios.ttf"
#define MENU_FONT_SIZE 300
#define MENU_BG_COLOR ((Color) { 0, 0, 192, 255 })
#define MENU_CURSOR_BG_COLOR ((Color) { 255, 0, 0, 255 })
#define MENU_FG_COLOR ((Color) { 255, 255, 255, 255 })
#define MENU_OPTION_COLOR ((Color) { 255, 255, 0, 255 })
#define MENU_OPTION_SELECTED_COLOR ((Color) { 255, 255, 255, 255 })
#define MENU_OPTION_COUNT 16

#define PORTAL_RADIUS 0.4f
#define PORTAL_APPEAR_SPEED 20.0f
#define PORTAL_DISAPPEAR_SPEED PORTAL_APPEAR_SPEED
#define PORTAL_EXHALE_RATE 0.01f
#define PORTAL_ATTRACT_MULTIPLIER 5.0f
#define PORTAL_EMIT_MULTIPLIER 5.0f

#define OPAQUE (Color){255,255,255,255}
#define DEATH_SOUND_AMOUNT 8

#define BOSS_NORMAL_BULLET_CAPACITY 32

typedef int8_t int8;
typedef int16_t int16;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint;

typedef enum Area {
    AREA_NONE = 0,
    AREA_FOREST,
    AREA_MEADOWS,
    AREA_MOUNTAINS,
    AREA_INDUSTRIAL,
    AREA_CASTLE,
    AREA_TOTAL,
} Area;

typedef enum PortalBits {
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
} PortalBits;

typedef enum GlobalState {
    GLOBAL_STATE_MENU,
    GLOBAL_STATE_TERMINAL,
    GLOBAL_STATE_FADE_OUT,
    GLOBAL_STATE_FADE_IN,
    GLOBAL_STATE_PRE_GAME_PORTAL_APPEAR,
    GLOBAL_STATE_PRE_GAME_PORTAL_EXHALE,
    GLOBAL_STATE_PRE_GAME_PORTAL_DISAPPEAR,
    GLOBAL_STATE_GAME,
    GLOBAL_STATE_POST_GAME_PORTAL_APPEAR,
    GLOBAL_STATE_POST_GAME_PORTAL_INHALE,
    GLOBAL_STATE_POST_GAME_PORTAL_DISAPPEAR,
    GLOBAL_STATE_BOSSBATTLE,
    GLOBAL_STATE_WIN,
    GLOBAL_STATE_GAME_OVER,
} GlobalState;

typedef enum FaderState {
    FADER_STATE_NONE = 0,
    FADER_STATE_IN,
    FADER_STATE_IN_COMPLETE,
    FADER_STATE_OUT,
    FADER_STATE_OUT_COMPLETE,
} FaderState;

typedef enum MenuState {
    MENU_STATE_DEFAULT,
    MENU_STATE_FREEPLAY,
    MENU_STATE_YOU_ARE_A_HORRIBLE_PERSON,
    MENU_STATE_GAME_OVER,
} MenuState;

typedef enum PlayerState {
    PLAYER_STATE_NONE = 0,
    PLAYER_STATE_EXHALED_BY_PORTAL,
    PLAYER_STATE_GROUNDED,
    PLAYER_STATE_UP,
    PLAYER_STATE_DOWN,
    PLAYER_STATE_INHALED_BY_PORTAL,
    PLAYER_STATE_INSIDE_PORTAL,
    PLAYER_STATE_LOST_LIFE,
} PlayerState;

typedef enum PlayerFlags {
    PLAYER_FLAG_HIDDEN = 1 << 0,
    PLAYER_FLAG_DRILL_UP = 1 << 1,
    PLAYER_FLAG_DRILL_DOWN = 1 << 2,
} PlayerFlags;

typedef enum BirdState {
    BIRD_STATE_AVAILABLE,
    BIRD_STATE_ALIVE,
    BIRD_STATE_DEAD,
    BIRD_STATE_INHALED_BY_PORTAL,
    BIRD_STATE_INSIDE_PORTAL,
    BIRD_STATE_EXHALED_BY_PORTAL,
} BirdState;

typedef enum BirdType {
    BIRD_TYPE_NONE,
    BIRD_TYPE_REGULAR,
    BIRD_TYPE_GIANT,
    BIRD_TYPE_UMBRELLA_ABOVE,
    BIRD_TYPE_UMBRELLA_UNDER,
    BIRD_TYPE_TINY,
    BIRD_TYPE_BOMB,
    BIRD_TYPES_TOTAL,
} BirdType;

typedef enum SpawnType {
    SPAWN_TYPE_REGULAR,
    SPAWN_TYPE_GIANT,
    SPAWN_TYPE_UMBRELLA,
    SPAWN_TYPE_BOMB,
    SPAWN_TYPES_TOTAL,
} SpawnType;

typedef enum BirdPalette {
    BIRD_PALETTE_WHITE,
    BIRD_PALETTE_YELLOW,
    BIRD_PALETTE_BROWN,
    BIRD_PALETTE_RED,
    BIRD_PALETTE_BLACK,
    BIRD_PALETTES_TOTAL,
} BirdPalette;

typedef enum BirdPaletteIdx {
    BIRD_PALETTE_IDX_OUTLINE,
    BIRD_PALETTE_IDX_BODY,
    BIRD_PALETTE_IDX_BEAK1,
    BIRD_PALETTE_IDX_BEAK2,
    BIRD_PALETTE_IDX_EYES,
    BIRD_PALETTE_IDX_GORE1,
    BIRD_PALETTE_IDX_GORE2,
    BIRD_PALETTE_IDX_GORE3,
    BIRD_PALETTE_IDX_UMBRELLA_HANDLE,
    BIRD_PALETTE_IDX_UMBRELLA_CANOPY1,
    BIRD_PALETTE_IDX_UMBRELLA_CANOPY2,
    BIRD_PALETTE_IDX_EXPLOSION1,
    BIRD_PALETTE_IDX_EXPLOSION2,
    BIRD_PALETTE_IDX_EXPLOSION3,
    BIRD_PALETTE_IDX_EXPLOSION4,
    BIRD_PALETTE_IDX_TOTAL,
} BirdPaletteIdx;

typedef enum BirdHit {
    BIRD_HIT_NONE = 0,
    BIRD_HIT_DESTROYED = 1 << 0,
    BIRD_HIT_BOUNCE_ALLOWED = 1 << 1,
    BIRD_HIT_PASS_THROUGH_ALLOWED = 1 << 2,
} BirdHit;

typedef struct SpriteScroller {
    Sprite sprite;
    uint8 horizontal_sprites;
    float horizontal_speed;
    uint8 vertical_sprites;
    float vertical_speed;
    Vector2 scroll;
    uint8 opacity;
} SpriteScroller;

enum {
    COMMAND_PLAY,
    COMMAND_MENU,
    COMMAND_GAME,
    COMMAND_BOSSBATTLE,
    COMMAND_TOGGLECOLLISIONBOUNDS,
    COMMAND_TOTAL,
};

struct State;

typedef struct Command {
    int command_idx;
    char *name;
    void (*function)(void *data);
} Command;

typedef struct Terminal {
    Rectangle rectangle;
    char lines[TERMINAL_LINE_CAPACITY][TERMINAL_LINE_MAX_LENGTH];
    int current_line;
    int current_char;
    Command commands[COMMAND_TOTAL];
    char fuzzy[TERMINAL_LINE_MAX_LENGTH];
} Terminal;

typedef struct Menu {
    Rectangle outer_rectangle;
    Rectangle inner_rectangle;
    uint16 center_x;

    float font_size;
    float header_size;
    float line_section_size;
    float line_size;
    float instruction_size;
    float description_size;
    float line_thickness;

    MenuState state;
    Font font;
    Sprite anim[4];
    uint16 anim_frame;
    float anim_timer;
    uint available_options;
    uint8 option_idx;
} Menu;

typedef struct Player {
    int lives;
    PlayerState state;
    PlayerFlags flags;
    float home_position;
    Vector2 kicker_position;
    Vector2 spinner_position;
    float rotation;
    uint16 current_input;
    float last_turnaround_y;
    int16 may_not_interact_with_this_bird_idx;
    float vertical_speed;
    float horizontal_speed;
    float reset_flicker_timer;
    float reset_flicker_count;
} Player;

typedef enum BirdFlags {
    BIRD_FLAG_NONE = 0,
    BIRD_FLAG_SHOW_COLLISION_BOUNDS = 1 << 0,
} BirdFlags;

typedef struct Bird {
    BirdState state;
    BirdType type;
    Vector2 position;
    float anim_time;
    union {
        struct {
            int health;
            int current_sprite;
            Vector2 collision_bounds;
            float move_speed;
            float damage_timer;
        } alive;
        struct {
            uint8 anim_start_sprite;
            uint8 anim_sprite_amount;
            uint8 anim_idx;
            Vector2 velocities[BIRD_DEATH_PARTS];
            Vector2 positions[BIRD_DEATH_PARTS];
            float angular_velocities[BIRD_DEATH_PARTS];
            float rotations[BIRD_DEATH_PARTS];
        } dead;
    };
} Bird;

typedef struct Level {
    uint8 sprite_scroller_amount;
    float bird_frequency;
    float bird_timer;
    uint16 required_fuel;
} Level;

typedef struct Boss {
    uint state;
    Vector2 position;
    Vector2 normal_bullets[BOSS_NORMAL_BULLET_CAPACITY];
} Boss;

typedef struct State {
    GlobalState global_state;

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
    Sound sound_explosion;

    Texture atlas;

    Area area;
    uint16 areas_discovered_bits;

    SpriteScroller sprite_scrollers[SPRITE_SCROLLER_CAP];

    uint level_idx;
    uint level_score;

    uint8 portal_fuel;

    Level current_level;

    Terminal terminal;

    Menu menu;

    Player player;

    BirdFlags bird_flags;
    Bird birds[BIRD_CAPACITY];
    Vector3 bird_palette[BIRD_PALETTES_TOTAL][BIRD_PALETTE_IDX_TOTAL];
    Shader bird_shader;
    int bird_shader_palette_location;
    Shader bird_damage_shader;
    uint birds_requested;
    uint bird_multiplier;
    uint bird_multiplier_display;
    float bird_multiplier_timer;
    BirdType birds_destroyed[BIRD_TYPES_TOTAL];
    uint bird_highest_multipliers[MENU_OPTION_COUNT];

    uint8 bird_random_heights[BIRD_Y_SECTIONS];
    uint8 bird_height_idx;
    uint8 bird_spawn_weights[SPAWN_TYPES_TOTAL];

    Shader cartoon_transition_shader;
    int cartoon_transition_shader_location_resolution;
    int cartoon_transition_shader_location_size;
    float cartoon_transition_size;

    FaderState fader_state;
    float fader_level;

    Shader portal_shader;
    PortalBits portal_bits;
    Texture portal_texture;
    float portal_linear_disappearance;
    float portal_smooth_disappearance;
    Color portal_color;
    float portal_timer;

    float scale_multiplier;
    float delta_time;

    Boss boss;
} State;

Vector2 to_pixel_position(State *state, Vector2 game_position);
Vector2 to_pixel_size(State *state, Vector2 game_size);

Rectangle game_rectangle(State *state);
bool has_flag(int flags, int flag);
float randf(float min, float max);

Vector2 vec2_normalized(float x, float y);
Vector2 vec2_direction(Vector2 from, Vector2 to);
float vec2_distance(Vector2 a, Vector2 b);

void atlas_draw(State *state, Sprite sprite, Vector2 position, float rotation, float scale, Color color);

Vector2 portal_get_position(State *state);
float portal_distance_to_center_ratio(State *state, Vector2 position);
bool portal_inhale_object(float *obj_position, float obj_step, float portal_position);

BirdHit bird_try_destroy_by_player(State *state, Bird *bird);

void terminal_setup(State *state); // TODO: bad code

inline static void draw_text(Menu *menu, char *text, Vector2 position, Color color) {
    Vector2 dimensions = MeasureTextEx(menu->font, text, menu->font_size, 0.0f);
    Vector2 origin = { 0, (dimensions.y / 2) };
    DrawTextPro(menu->font, text, position, origin, 0.0f, menu->font_size, 0.0f, color);
}

#endif
