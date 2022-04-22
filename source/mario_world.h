#include<citro2d.h>

// Screens dimensions
#define TOP_SCREEN_WIDTH 400
#define TOP_SCREEN_HEIGHT 240
#define BOTTOM_SCREEN_WIDTH 320
#define BOTTOM_SCREEN_HEIGHT 240

#define FPS (268123480/60)
#define BUFFER_SIZE 160
#define TIME_BUFFER_SIZE 80

// Mario constants
#define JUMP_INDEX 0
#define FALL_INDEX 1
#define MARIO_SPEED 3.2
#define MARIO_JUMP_SPEED 4.8
#define MARIO_AIR_SPEED 2.8
#define MARIO_FALL_SPEED 3
#define MARIO_INITIAL_POS_Y 173
#define MARIO_INITIAL_POS_X 10
#define WAlK_ANIMATION_SIZE 3
#define SMALL_WAlK_ANIMATION_SIZE 2
#define MARIO_UPWARDS_DEAD_DELAY 15000
#define BIG_JUMP 65000000
#define SMALL_JUMP 70000000
#define MARIO_SMALL_Y 190
#define SPRITE_REFRESH 2800
#define INVINCIBILITY_TIME 12000
#define DAMAGE_ANIM_DELAY 12000
enum Mario_Sprites {
	LEFT_WALK_2,
    LEFT_WALK_3,
    BACK,
    FRONT,
    LEFT_WALK_1,
    RIGHT_WALK_1,
    FRONT_2,
    BACK_2,
    RIGHT_WALK_3,
    RIGHT_WALK_2,
    STOP_AND_MOVE_LEFT,
    SUCCESS_LEFT_HAND,
    CROUCH_LEFT,
    FALL_LEFT,
    JUMP_LEFT,
    JUMP_RIGHT,
    FALL_RIGHT,
    CROUCH_RIGHT,
    SUCCESS_RIGHT_HAND,
    STOP_AND_MOVE_RIGHT,
    UNKNOWN,
    FLY_LEFT,
    EQUILIBRIUM_LEFT_1,
    EQUILIBRIUM_LEFT_2,
    EQUILIBRIUM_LEFT_3,
    EQUILIBRIUM_RIGHT_3,
    EQUILIBRIUM_RIGHT_2,
    EQUILIBRIUM_RIGHT_1,
    FLY_RIGHT,
    UNKNOWN_2,
    MARIODEAD,
    SMALL_RIGHT_FALL,
    SMALL_RIGHT_IDLE,
    SMALL_RIGHT_JUMP,
    SMALL_RIGHT_WALK,
    SMALL_LEFT_FALL,
    SMALL_LEFT_IDLE,
    SMALL_LEFT_JUMP,
    SMALL_LEFT_WALK
};

int jumpRightAnim[] =  {
    JUMP_RIGHT,
    FALL_RIGHT
};
int jumpLeftAnim[] =  {
    JUMP_LEFT,
    FALL_LEFT
};
int smallJumpRightAnim[] =  {
    SMALL_RIGHT_JUMP,
    SMALL_RIGHT_FALL
};
int smallJumpLeftAnim[] =  {
    SMALL_LEFT_JUMP,
    SMALL_LEFT_FALL
};
int smallRightWalk[] = {
    SMALL_RIGHT_IDLE,
    SMALL_RIGHT_WALK,
};
int smallLeftWalk[] = {
    SMALL_LEFT_IDLE,
    SMALL_LEFT_WALK
};
int rightWalk[] = {
    RIGHT_WALK_1,
    RIGHT_WALK_2,
    RIGHT_WALK_3
};
int leftWalk[] = {
    LEFT_WALK_1,
    LEFT_WALK_2,
    LEFT_WALK_3,
};

#define BLACK C2D_Color32f(0.0f, 0.0f, 0.0f, 1.0f)

enum MarioState {
    walking,
    jumping,
    falling,
    dead
};

typedef struct {
    /* data */
    float dx, dy; 
    float base_y;
    float speed;
    float air_speed;
    bool alive;
    float jump_speed;
    float fall_speed;
    int current_sprite;
    float anim_elapsed_time;
    float upwards_dead_anim_delay;
    float dead_elapsed_time;
    float damage_anim_delay;
    float damage_anim_elapsed_time;
    bool damage_anim_direction;
    bool can_move;
    int max_jump_time;
    int sprite_refresh;
    bool small;
    bool can_jump;
    int* left_walk_anim;
    int* right_walk_anim;
    int anim_num_sprites;
    int* left_jump_anim;
    int* right_jump_anim;
    int right_collision_margin;
    int bottom_collision_margin;
    int left_collision_margin;
    int top_collision_margin;
    int lifes;
    int coins;
    u64 jump_start;
    u64 small_invincibility;
    u64 invincibility_elapsed_ms;
    C2D_Sprite sprite;
    MarioState state;
} Mario;

typedef struct {
    C2D_Sprite sprite;
} Background;

//Coin constants
#define COIN_ANIMATION_TIME 3000

typedef struct {
    C2D_Sprite sprite;
    bool visible;
    float elapsed_time;
    float dx, dy;
    float animation_time;
} Coin;

//TOAD constants
#define TOAD_CRY_TIME 8000
#define TOAD_INITIAL_POS_Y 183
#define TOAD_ANIMATION_SPRITES 2
#define TOAD_INITIAL_POS_X 350

enum ToadSprites {
    TOAD_CRY1,
    TOAD_CRY2
};

int toadAnimation[] = { TOAD_CRY1, TOAD_CRY2 };

typedef struct {
    float animation_delay;
    double elapsed_time;
    int current_sprite;
    C2D_Sprite sprite;
} Toad;

enum TEXTS {
    TEXT1
};

int texts[] = {TEXT1};

typedef struct {
    int current_text;
    bool visible;
    C2D_Sprite sprite;
} ToadText;


//GOOMBA constants 
#define GOOMBA_ANIMATION_DELAY 3800
#define GOOMBA_DIRECTION_TIME 70000
#define GOOMBA_INITIAL_POS_X 150
#define GOOMBA_INITIAL_POS_Y 195
#define GOOMBA_SPEED 0.45
#define DIRECTION_RIGHT true
#define DIRECTION_LEFT false
#define GOOMBA_ANIMATION_SPRITES 3

typedef struct {
    float dx, dy;
    float speed;
    float animation_delay;
    bool alive;
    float direction_delay;
    C2D_Sprite sprite;
    int current_sprite;
    int rotation;
    float animation_elapsed_time;
    float direction_elapsed_time;
    bool current_direction;
    Coin coin;
} Goomba;

enum GoombaSprites {
    GOOMBA1,
    GOOMBA2,
    GOOMBA3,
    GOOMBADEAD
};

int goomba_sprites[] = { GOOMBA1, GOOMBA2, GOOMBA3, GOOMBADEAD };

// Block constants
#define BLOCK_INITIAL_POS_Y 110
#define BLOCK_INITIAL_POS_X 150
#define BLOCK_UNTOUCHED 0
#define BLOCK_TOUCHED 1

typedef struct {
    float dx, dy; 
    bool broken;
    C2D_Sprite sprite;
    int current_sprite;
    Coin coin;
} Block;

//Button constants
typedef struct {
    C2D_Sprite sprite;
    bool visible;
} Button;

typedef struct {
    C2D_Sprite sprite;
    bool visible;
} Title;
 
typedef struct {
    C2D_Sprite sprite;
} Scoreboard;

typedef struct {
    C2D_Sprite sprite;
	bool visible;
} Credits;

#define RESET true
#define GAMETIME false

typedef struct {
    u64 initial_time;
    u64 current_seconds;
} TimeState;
