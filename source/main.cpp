#include <citro2d.h>
#include <cwav.h>
#include <inttypes.h>
#include <3ds.h>
#include "mario_world.h"
#include <algorithm>
#include <iterator>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ncsnd.h>
#include <time.h>
#include "stb/stretchy_buffer.h"

/* C2D_Text Declaration Variables */
C2D_TextBuf g_dynamicBuf; // Buffer Declaratation

static Mario mario;
static Background background;
static Toad toad;
static ToadText toadText;
static Goomba goomba;
static Button button;
static Button button_title;
static Block block;
static Title title;
static Scoreboard scoreboard;
static TimeState timeState;
static Credits credits;

static C2D_SpriteSheet mario_spriteSheet;
static C2D_SpriteSheet background_spriteSheet;
static C2D_SpriteSheet block_spriteSheet;
static C2D_SpriteSheet toad_spriteSheet;
static C2D_SpriteSheet toadText_spriteSheet;
static C2D_SpriteSheet goomba_spriteSheet;
static C2D_SpriteSheet button_spriteSheet;
static C2D_SpriteSheet coin_spriteSheet;
static C2D_SpriteSheet title_spriteSheet;
static C2D_SpriteSheet scoreboard_spriteSheet;
static C2D_SpriteSheet credits_spriteSheet;

Mario *mario_pointer = &mario;
Toad *toad_pointer = &toad;
ToadText *toadText_pointer = &toadText;
Goomba *goomba_pointer = &goomba;
Block *block_pointer = &block;
Button *button_pointer = &button;
Button *button_title_pointer = &button_title;
Coin *coin_block_pointer = &block.coin;
Coin *coin_goomba_pointer = &goomba.coin;
Title *title_pointer = &title;
Scoreboard *scoreboard_pointer = &scoreboard;
Credits *credits_pointer = &credits;

u64 start_loop_time = svcGetSystemTick();
u64 now = svcGetSystemTick();

static CWAV* ost = (CWAV*) malloc(sizeof(CWAV));
static CWAV* toadSound = (CWAV*) malloc(sizeof(CWAV));

void initTimeState() {
    timeState.initial_time = svcGetSystemTick();
    timeState.current_seconds = 0;
}

void advanceTimeState()
{
    timeState.current_seconds = (svcGetSystemTick() - timeState.initial_time) / (CPU_TICKS_PER_MSEC * 1000);
}

bool array_contains(int val, int array[], int* pos, int num_elems) {
    //printf("SIZE OF ARRAY IS %d\n", sizeof(array) / sizeof(array[0]));
     int i;
    for(i = 0; i < num_elems; i++)
    {
        if(array[i] == val) {
			*pos = i;
            return 1;
		}
    }

	return 0;
} 

void controllerSprites_credits()
{
	C2D_SpriteFromSheet(&credits_pointer->sprite, credits_spriteSheet, 0);
	C2D_SpriteSetCenter(&credits_pointer->sprite, 0.f, 0.f);
	C2D_SpriteSetPos(&credits_pointer->sprite, TOAD_INITIAL_POS_X - 3, TOAD_INITIAL_POS_Y - 30);
	C2D_SpriteSetRotationDegrees(&credits_pointer->sprite, 0); 
	C2D_DrawSprite(&credits_pointer->sprite);
}

void controllerSprites_scoreboard()
{
	// Position, rotation and SPEED
	C2D_SpriteFromSheet(&scoreboard_pointer->sprite, scoreboard_spriteSheet, 0);
	C2D_SpriteSetCenter(&scoreboard_pointer->sprite, 0.f, 0.f);
	C2D_SpriteSetPos(&scoreboard_pointer->sprite, TOAD_INITIAL_POS_X - 3, TOAD_INITIAL_POS_Y - 30);
	C2D_SpriteSetRotationDegrees(&scoreboard_pointer->sprite, 0); 
	C2D_DrawSprite(&scoreboard_pointer->sprite);
}

void controllerSprites_mario()
{
	// Sprite
	C2D_SpriteFromSheet(&mario_pointer->sprite, mario_spriteSheet, mario_pointer->current_sprite);
 	C2D_SpriteSetCenter(&mario_pointer->sprite, 0.f, 0.f);
	C2D_SpriteSetPos(&mario_pointer->sprite, mario_pointer->dx, mario_pointer->dy);
	C2D_SpriteSetRotationDegrees(&mario_pointer->sprite, 0); 
    C2D_DrawSprite(&mario_pointer->sprite);
}

void controllerSprites_Toad()
{
	C2D_SpriteFromSheet(&toad_pointer->sprite, toad_spriteSheet, toadAnimation[toad_pointer->current_sprite]);
    C2D_SpriteSetCenter(&toad_pointer->sprite, 0.f, 0.f);
	C2D_SpriteSetPos(&toad_pointer->sprite, TOAD_INITIAL_POS_X, TOAD_INITIAL_POS_Y);
	C2D_SpriteSetRotationDegrees(&toad_pointer->sprite, 0); 
}

void controllerSprites_Button()
{
	C2D_SpriteFromSheet(&button_pointer->sprite, button_spriteSheet, 0);
    C2D_SpriteSetCenter(&button_pointer->sprite, 0.f, 0.f);
	C2D_SpriteSetPos(&button_pointer->sprite, TOAD_INITIAL_POS_X - 3, TOAD_INITIAL_POS_Y - 30);
	C2D_SpriteSetRotationDegrees(&button_pointer->sprite, 0); 
}

void controllerSprites_Goomba()
{
	C2D_SpriteFromSheet(&goomba_pointer->sprite, goomba_spriteSheet, goomba_sprites[goomba_pointer->current_sprite]);
    C2D_SpriteSetCenter(&goomba_pointer->sprite, 0.f, 0.f);
	C2D_SpriteSetPos(&goomba_pointer->sprite, goomba_pointer->dx, goomba_pointer->dy);
	C2D_SpriteSetRotationDegrees(&goomba_pointer->sprite, goomba_pointer->rotation); 

	C2D_SpriteFromSheet(&coin_goomba_pointer->sprite, coin_spriteSheet, 0);
 	C2D_SpriteSetCenter(&coin_goomba_pointer->sprite, 0.f, 0.f);
	C2D_SpriteSetPos(&coin_goomba_pointer->sprite, coin_goomba_pointer->dx, coin_goomba_pointer->dy);
	C2D_SpriteSetRotationDegrees(&coin_goomba_pointer->sprite, 0); 
}

void controllerSprites_block()
{
	C2D_SpriteFromSheet(&block_pointer->sprite, block_spriteSheet, block_pointer->current_sprite);
 	C2D_SpriteSetCenter(&block_pointer->sprite, 0.f, 0.f);
	C2D_SpriteSetPos(&block_pointer->sprite, block_pointer->dx, block_pointer->dy);
	C2D_SpriteSetRotationDegrees(&block_pointer->sprite, 0); 

	C2D_SpriteFromSheet(&coin_block_pointer->sprite, coin_spriteSheet, 0);
 	C2D_SpriteSetCenter(&coin_block_pointer->sprite, 0.f, 0.f);
	C2D_SpriteSetPos(&coin_block_pointer->sprite, coin_block_pointer->dx, coin_block_pointer->dy);
	C2D_SpriteSetRotationDegrees(&coin_block_pointer->sprite, 0); 
}

bool isInCollissionWithBlock() {
    bool colission = mario_pointer->dx >= BLOCK_INITIAL_POS_X - mario_pointer->right_collision_margin 
    && mario_pointer->dx <= BLOCK_INITIAL_POS_X
    && mario_pointer->dy <= BLOCK_INITIAL_POS_Y + mario_pointer->top_collision_margin
    && mario_pointer->dy >= BLOCK_INITIAL_POS_Y - mario_pointer->top_collision_margin;
    if (colission && block_pointer->current_sprite != BLOCK_TOUCHED) {
        block_pointer->current_sprite = BLOCK_TOUCHED;
        controllerSprites_block();
        coin_block_pointer->visible = true;
    }  //Aquí el bloque cambia al otro sprite y suelta moneda
    return colission;
}

//Solo comprueba si está en colisión desde el eje X, si lo hace desde falling (que se comprobará desde otro sitio) se mata al bicho y consigue una moneda
bool isInCollissionWithGoomba() {
    bool colission = mario_pointer->dx >= goomba_pointer-> dx - mario_pointer->right_collision_margin
    && mario_pointer->dx <= goomba_pointer->dx + mario_pointer->left_collision_margin
    && mario_pointer->dy + mario_pointer->bottom_collision_margin >= goomba_pointer->dy;
    return colission;
}

void characterAnimations() {
    u64 chars_now = svcGetSystemTick();
    toad_pointer->elapsed_time += (chars_now - start_loop_time);
    goomba_pointer->animation_elapsed_time += (chars_now - start_loop_time);
    goomba_pointer->direction_elapsed_time += (chars_now - start_loop_time);

    //TOAD ANIMATION
    if (toad_pointer->elapsed_time >= toad_pointer->animation_delay)  {
           toad_pointer->elapsed_time = 0;
           toad_pointer->current_sprite = ++toad_pointer->current_sprite % TOAD_ANIMATION_SPRITES;
    }

    //GOOMBA ANIMATION
    if (goomba_pointer->current_sprite != GOOMBADEAD) {
        if (goomba_pointer->current_direction == DIRECTION_RIGHT) {
            goomba_pointer->dx += goomba_pointer->speed;
        } else {
            goomba_pointer->dx -= goomba_pointer->speed;
        }
        if (goomba_pointer->animation_elapsed_time >= goomba_pointer->animation_delay) {
            goomba_pointer->current_sprite = ++goomba_pointer->current_sprite % GOOMBA_ANIMATION_SPRITES;
            goomba_pointer->animation_elapsed_time = 0;
        }
        if (goomba_pointer->direction_elapsed_time >= goomba_pointer->direction_delay) {
            goomba_pointer->current_direction = !goomba_pointer->current_direction;
            goomba_pointer->direction_elapsed_time = 0;
        }
    } else {
        if (goomba_pointer->dy >= TOP_SCREEN_HEIGHT) {
            goomba_pointer->alive = false;
        } else {
            goomba_pointer->dy += 1.5;
            goomba_pointer->rotation += 4;
        }
    }

    if (coin_block_pointer->visible) {
        coin_block_pointer->elapsed_time += (chars_now - start_loop_time);
        if (coin_block_pointer->elapsed_time <= coin_block_pointer->animation_time) {
            coin_block_pointer->dy -= 3;
        } else {
            coin_block_pointer->visible = false;
			mario_pointer->coins +=1;
        }
    }

    if (coin_goomba_pointer->visible) {
        coin_goomba_pointer->elapsed_time += (chars_now - start_loop_time);
        if (coin_goomba_pointer->elapsed_time <= coin_goomba_pointer->animation_time) {
            coin_goomba_pointer->dy -= 3;
        } else {
            coin_goomba_pointer->visible = false;
			mario_pointer->coins +=1;
        }
    }
    
    
    controllerSprites_Toad();
    controllerSprites_Goomba();
    controllerSprites_block();
}

void marioPhysics() {
    float new_y = mario_pointer->dy;
    float new_x = mario_pointer->dx;
    now = svcGetSystemTick();
    int pos;

    if (mario_pointer->state == MarioState::falling) {
        //printf("MARIO IS FALLING\n");
        new_y += mario_pointer->fall_speed;

        if (mario_pointer->dy >= mario_pointer->base_y) {
//        printf("MARIO SHOULD BE WALKING\n");
            new_y = mario_pointer->base_y;
            mario_pointer->state = MarioState::walking;
            if (array_contains(mario_pointer->current_sprite, mario_pointer->left_jump_anim, &pos, 2)) {
                mario_pointer->current_sprite = mario_pointer->left_walk_anim[0];
            } else {
                mario_pointer->current_sprite = mario_pointer->right_walk_anim[0];
            }
        }
    } else if (mario_pointer->state == MarioState::jumping && (now - mario_pointer->jump_start) >= mario_pointer->max_jump_time) {
        mario_pointer->can_jump = false;
        mario_pointer->state = MarioState::falling;
        if (mario_pointer->current_sprite == mario_pointer->left_jump_anim[JUMP_INDEX]) {
            mario_pointer->current_sprite = mario_pointer->left_jump_anim[FALL_INDEX];
        } else {
            mario_pointer->current_sprite = mario_pointer->right_jump_anim[FALL_INDEX];
        }
    }

    C2D_SpriteMove(&mario_pointer->sprite, mario_pointer->dx = new_x, mario_pointer->dy = new_y);
    controllerSprites_mario();
}

bool isInDialogPos() {
   return mario_pointer->dx >= TOAD_INITIAL_POS_X - 40 
    && mario_pointer->dx <= TOAD_INITIAL_POS_X + 40
    && mario_pointer->dy >= TOAD_INITIAL_POS_Y - 30;
}

void moveMario(u32 kHeld)
{
	int pos;
    now = svcGetSystemTick();
    mario_pointer->anim_elapsed_time += (now - start_loop_time);
    float new_y = mario_pointer->dy;
    float new_x = mario_pointer->dx;
    float speed;


        if ((kHeld & KEY_A) && mario_pointer->can_jump) {
            if (mario_pointer->state == MarioState::walking) {
                if (!isInDialogPos()) {
                    mario_pointer->jump_start = svcGetSystemTick();
                    mario_pointer->state = MarioState::jumping;
                    new_y -= mario_pointer->jump_speed;

                    if (array_contains(mario_pointer->current_sprite, mario_pointer->right_walk_anim, &pos, mario_pointer->anim_num_sprites)) {
                        //printf("CONTAINS %d in POS %d JUMP RIGHT\n", mario_pointer->current_sprite, pos);
                        mario_pointer->current_sprite = mario_pointer->right_jump_anim[JUMP_INDEX];
                    } else {
                        //printf("CONTAINS %d in POS %d JUMP LEFT\n", mario_pointer->current_sprite, pos);
                        mario_pointer->current_sprite = mario_pointer->left_jump_anim[JUMP_INDEX];
                    }
                }  
            } else if (mario_pointer->state == MarioState::jumping) {
                if ((kHeld & KEY_A)) { 
                    if (isInCollissionWithBlock()) {
                        mario_pointer->state = MarioState::falling;
                    } else {
                        new_y -= mario_pointer->jump_speed;
                    }
                }
            }
        } 


    if (mario_pointer->state == walking) {
        speed = mario_pointer->speed;
    } else {
        speed = mario_pointer->air_speed;
    }

	if (kHeld & KEY_LEFT)
	{
        if (!isInCollissionWithBlock()) {
            new_x -= speed;
        }
        if (mario_pointer->anim_elapsed_time >= mario_pointer->sprite_refresh) {
		    if (array_contains(mario_pointer->current_sprite, mario_pointer->left_walk_anim, &pos, mario_pointer->anim_num_sprites)) {
			    mario_pointer->current_sprite = mario_pointer->left_walk_anim[(pos + 1) % mario_pointer->anim_num_sprites];
            //printf("ADVANCING LEFT\n");
                mario_pointer->anim_elapsed_time = 0;
            } else {
            //printf("START TO LOOK LEFT\n");
                switch(mario_pointer->state) {
                    case MarioState::walking: 
			            mario_pointer->current_sprite = mario_pointer->left_walk_anim[0];
                        break;
                    case MarioState::jumping:
                        mario_pointer->current_sprite = mario_pointer->left_jump_anim[JUMP_INDEX];
                        break;
                    case MarioState::falling:
                        mario_pointer->current_sprite = mario_pointer->left_jump_anim[FALL_INDEX];
                        break;
                    case MarioState::dead:
                        break;
                }
		    } 	
        }
    }
	
	if (kHeld & KEY_RIGHT)
	{

        if (!isInCollissionWithBlock()) {
            new_x += speed;
        }
        if (mario_pointer->anim_elapsed_time >= mario_pointer->sprite_refresh) {
		    if (array_contains(mario_pointer->current_sprite, mario_pointer->right_walk_anim, &pos, mario_pointer->anim_num_sprites)) {
			    mario_pointer->current_sprite = mario_pointer->right_walk_anim[(pos + 1) % mario_pointer->anim_num_sprites];
                mario_pointer->anim_elapsed_time = 0;
            //printf("ADVANCING RIGHT\n");
            } else {
                switch(mario_pointer->state) {
             //                   printf("START TO LOOK RIGHT\n");
                    case MarioState::walking: 
			            mario_pointer->current_sprite = mario_pointer->right_walk_anim[0];
                        break;
                    case MarioState::jumping:
                        mario_pointer->current_sprite = mario_pointer->right_jump_anim[JUMP_INDEX];
                        break;
                    case MarioState::falling:
                        mario_pointer->current_sprite = mario_pointer->right_jump_anim[FALL_INDEX];
                        break;
                    case MarioState::dead:
                        break;
                }
    		}
        } 	
    }	 
	//if(isInCollissionWithGoomba());
	//En los bordes no avanza
	if(new_x < -10.0) new_x = -10.0;
	if(new_x > 366.0) new_x = 366.0;
    C2D_SpriteMove(&mario_pointer->sprite, mario_pointer->dx = new_x, mario_pointer->dy = new_y);
    controllerSprites_mario();
}

void setIdleMario(int kUp) {
    if (kUp & KEY_A) {
        if (mario_pointer->state == MarioState::jumping) {
            mario_pointer->state = MarioState::falling;
        }
        mario_pointer->can_jump = true;
    }
    int pos = -1;
    if (mario_pointer->state == MarioState::walking) {
        if (array_contains(mario_pointer->current_sprite, mario_pointer->right_walk_anim, &pos, mario_pointer->anim_num_sprites)) {
            //printf("AQUI RIGHT WALK EN KEY UP\n");
            mario_pointer->current_sprite = mario_pointer->right_walk_anim[0];
        } else {
            //printf("AQUI LEFT WALK EN KEY UP sprite id %d  POS %d\n", mario_pointer->current_sprite, pos);
            mario_pointer->current_sprite = mario_pointer->left_walk_anim[0];
        }
    }
    controllerSprites_mario();
}

void prepare_sprites() {
	
	credits_spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/end.t3x");
    if (!credits_spriteSheet) {
        svcBreak(USERBREAK_PANIC);
    }
	scoreboard_spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/scoreboard.t3x");
    if (!scoreboard_spriteSheet) {
        svcBreak(USERBREAK_PANIC);
    }
    //Characters
    mario_spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/mario.t3x");
    if (!mario_spriteSheet) {
        svcBreak(USERBREAK_PANIC);
    }
    toad_spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/toad.t3x");
    if (!toad_spriteSheet) {
        svcBreak(USERBREAK_PANIC);
    }
    goomba_spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/goomba.t3x");
    if (!goomba_spriteSheet) {
        svcBreak(USERBREAK_PANIC);
    }

    //Prompts 
    toadText_spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/toadtext.t3x");
    if (!toadText_spriteSheet) {
        svcBreak(USERBREAK_PANIC);
    }
    button_spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/button.t3x");
    if (!button_spriteSheet) {
        svcBreak(USERBREAK_PANIC);
    }
    title_spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/title.t3x");
    if (!title_spriteSheet) {
        svcBreak(USERBREAK_PANIC);
    }

    //Objects
    block_spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/Blocks.t3x");
	if (!block_spriteSheet) {
        svcBreak(USERBREAK_PANIC);
    }
    coin_spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/coin.t3x");
    if (!coin_spriteSheet) {
        svcBreak(USERBREAK_PANIC);
    }

    //Screens
    background_spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/map.t3x");
    if (!background_spriteSheet) {
        svcBreak(USERBREAK_PANIC);
    }
}

void setDefaultMarioValues() {
    mario_pointer->alive = true;
    mario_pointer->base_y = MARIO_INITIAL_POS_Y;
    mario_pointer->dx = MARIO_INITIAL_POS_X;
    mario_pointer->dy = MARIO_INITIAL_POS_Y;
    mario_pointer->speed = MARIO_SPEED;
    mario_pointer->air_speed = MARIO_AIR_SPEED;
    mario_pointer->jump_speed = MARIO_JUMP_SPEED;
    mario_pointer->fall_speed = MARIO_FALL_SPEED;
    mario_pointer->state = MarioState::walking;
    mario_pointer->anim_elapsed_time = 0;
    mario_pointer->dead_elapsed_time = 0;
    mario_pointer->current_sprite = RIGHT_WALK_1;
    mario_pointer->upwards_dead_anim_delay = MARIO_UPWARDS_DEAD_DELAY;
    mario_pointer->sprite_refresh = SPRITE_REFRESH;
    mario_pointer->max_jump_time = BIG_JUMP;
    mario_pointer->can_jump = true;
    mario_pointer->left_walk_anim = leftWalk;
    mario_pointer->right_walk_anim = rightWalk;
    mario_pointer->right_jump_anim = jumpRightAnim;
    mario_pointer->left_jump_anim = jumpLeftAnim;
    mario_pointer->anim_num_sprites = WAlK_ANIMATION_SIZE;
    mario_pointer->small = false;
    mario_pointer->right_collision_margin = 25;
    mario_pointer->bottom_collision_margin = 30;
    mario_pointer->left_collision_margin = 1;
    mario_pointer->top_collision_margin = 10;
    mario_pointer->small_invincibility = INVINCIBILITY_TIME;
    mario_pointer->invincibility_elapsed_ms = 0;
    mario_pointer->lifes = 2;
    mario_pointer->coins = 0;
}

void setDefaultGoombaValues() {
	
    C2D_SpriteFromSheet(&goomba_pointer->sprite, goomba_spriteSheet, 0);
    C2D_SpriteSetCenter(&goomba_pointer->sprite, 0.f, 0.f);
    C2D_SpriteSetPos(&goomba_pointer->sprite, GOOMBA_INITIAL_POS_X, GOOMBA_INITIAL_POS_Y);
    C2D_SpriteSetRotation(&goomba_pointer->sprite, C3D_Angle(0));
	
    goomba_pointer->animation_delay = GOOMBA_ANIMATION_DELAY;
    goomba_pointer->current_sprite = 0;
    goomba_pointer->rotation = 0;
    goomba_pointer->alive = true;
    goomba_pointer->direction_delay = GOOMBA_DIRECTION_TIME;
    goomba_pointer->dx = GOOMBA_INITIAL_POS_X;
    goomba_pointer->dy = GOOMBA_INITIAL_POS_Y;
    goomba_pointer->speed = GOOMBA_SPEED;
    goomba_pointer->current_direction = DIRECTION_LEFT;


    C2D_SpriteFromSheet(&coin_goomba_pointer->sprite, coin_spriteSheet, 0);
    C2D_SpriteSetCenter(&coin_goomba_pointer->sprite, 0.f, 0.f);
    C2D_SpriteSetPos(&coin_goomba_pointer->sprite, GOOMBA_INITIAL_POS_X, GOOMBA_INITIAL_POS_Y);
    C2D_SpriteSetRotation(&coin_goomba_pointer->sprite, C3D_Angle(0));
    coin_goomba_pointer->visible = false;
    coin_goomba_pointer->animation_time = COIN_ANIMATION_TIME;
    coin_goomba_pointer->dx = GOOMBA_INITIAL_POS_X;
    coin_goomba_pointer->dy = GOOMBA_INITIAL_POS_Y;
    coin_goomba_pointer->elapsed_time = 0;

}

void prepare_mario(int posX, int posY) {
    C2D_SpriteFromSheet(&mario_pointer->sprite, mario_spriteSheet, 4);
    C2D_SpriteSetCenter(&mario_pointer->sprite, 0.f, 0.f);
    C2D_SpriteSetPos(&mario_pointer->sprite, posX, posY);
    C2D_SpriteSetRotation(&mario_pointer->sprite,C3D_Angle(0));
    setDefaultMarioValues();
}

void prepare_game_screen() {
    Background *screenSprite = &background;
    C2D_SpriteFromSheet(&screenSprite->sprite, background_spriteSheet,0);
    C2D_SpriteSetCenter(&screenSprite->sprite, 0.5f, 1.0f);
    C2D_SpriteSetPos(&screenSprite->sprite, TOP_SCREEN_WIDTH / 2, TOP_SCREEN_HEIGHT);
}

void prepare_title() {
    C2D_SpriteFromSheet(&title_pointer->sprite, title_spriteSheet,0);
    C2D_SpriteSetCenter(&title_pointer->sprite, 0.5f, 1.0f);
    C2D_SpriteSetPos(&title_pointer->sprite, TOP_SCREEN_WIDTH / 2, TOP_SCREEN_HEIGHT - 100);
    C2D_SpriteSetRotation(&title_pointer->sprite, C3D_Angle(0));
    title_pointer->visible = true;

	C2D_SpriteFromSheet(&button_title_pointer->sprite, button_spriteSheet, 0);
    C2D_SpriteSetCenter(&button_title_pointer->sprite, 0.f, 0.f);
	C2D_SpriteSetPos(&button_title_pointer->sprite, TOP_SCREEN_WIDTH - 215, TOP_SCREEN_HEIGHT - 80);
	C2D_SpriteSetRotationDegrees(&button_title_pointer->sprite, 0); 
}

void prepare_credits() {
    C2D_SpriteFromSheet(&credits_pointer->sprite, credits_spriteSheet,0);
    C2D_SpriteSetCenter(&credits_pointer->sprite, 0.5f, 1.0f);
    C2D_SpriteSetPos(&credits_pointer->sprite, TOP_SCREEN_WIDTH / 2, TOP_SCREEN_HEIGHT);
    C2D_SpriteSetRotation(&credits_pointer->sprite, C3D_Angle(0));
}

void prepare_scoreboard() {
    C2D_SpriteFromSheet(&scoreboard_pointer->sprite, scoreboard_spriteSheet,0);
    C2D_SpriteSetCenter(&scoreboard_pointer->sprite, 0.5f, 1.0f);
    C2D_SpriteSetPos(&scoreboard_pointer->sprite, BOTTOM_SCREEN_WIDTH / 2, BOTTOM_SCREEN_HEIGHT - 100);
    C2D_SpriteSetRotation(&scoreboard_pointer->sprite, C3D_Angle(0));
}


void prepare_block() {
    C2D_SpriteFromSheet(&block_pointer->sprite, block_spriteSheet, 0);
    C2D_SpriteSetCenter(&block_pointer->sprite, 0.f, 0.f);
    C2D_SpriteSetPos(&block_pointer->sprite, BLOCK_INITIAL_POS_X, BLOCK_INITIAL_POS_Y);
    C2D_SpriteSetRotation(&block_pointer->sprite,C3D_Angle(0));
    block_pointer->broken = false;
    block_pointer->dx = BLOCK_INITIAL_POS_X;
    block_pointer->dy = BLOCK_INITIAL_POS_Y;
    block_pointer->current_sprite = BLOCK_UNTOUCHED;

    C2D_SpriteFromSheet(&coin_block_pointer->sprite, coin_spriteSheet, 0);
    C2D_SpriteSetCenter(&coin_block_pointer->sprite, 0.f, 0.f);
    C2D_SpriteSetPos(&coin_block_pointer->sprite, BLOCK_INITIAL_POS_X, BLOCK_INITIAL_POS_Y);
    C2D_SpriteSetRotation(&coin_block_pointer->sprite, C3D_Angle(0));
    coin_block_pointer->visible = false;
    coin_block_pointer->animation_time = COIN_ANIMATION_TIME;
    coin_block_pointer->dx = BLOCK_INITIAL_POS_X;
    coin_block_pointer->dy = BLOCK_INITIAL_POS_Y;
    coin_block_pointer->elapsed_time = 0;
}

void prepare_goomba() {
    C2D_SpriteFromSheet(&goomba_pointer->sprite, goomba_spriteSheet, 0);
    C2D_SpriteSetCenter(&goomba_pointer->sprite, 0.f, 0.f);
    C2D_SpriteSetPos(&goomba_pointer->sprite, GOOMBA_INITIAL_POS_X, GOOMBA_INITIAL_POS_Y);
    C2D_SpriteSetRotation(&goomba_pointer->sprite, C3D_Angle(0));
    goomba_pointer->animation_delay = GOOMBA_ANIMATION_DELAY;
    goomba_pointer->current_sprite = 0;
    goomba_pointer->rotation = 0;
    goomba_pointer->alive = true;
    goomba_pointer->direction_delay = GOOMBA_DIRECTION_TIME;
    goomba_pointer->dx = GOOMBA_INITIAL_POS_X;
    goomba_pointer->dy = GOOMBA_INITIAL_POS_Y;
    goomba_pointer->speed = GOOMBA_SPEED;
    goomba_pointer->current_direction = DIRECTION_RIGHT;


    C2D_SpriteFromSheet(&coin_goomba_pointer->sprite, coin_spriteSheet, 0);
    C2D_SpriteSetCenter(&coin_goomba_pointer->sprite, 0.f, 0.f);
    C2D_SpriteSetPos(&coin_goomba_pointer->sprite, GOOMBA_INITIAL_POS_X, GOOMBA_INITIAL_POS_Y);
    C2D_SpriteSetRotation(&coin_goomba_pointer->sprite, C3D_Angle(0));
    coin_goomba_pointer->visible = false;
    coin_goomba_pointer->animation_time = COIN_ANIMATION_TIME;
    coin_goomba_pointer->dx = GOOMBA_INITIAL_POS_X;
    coin_goomba_pointer->dy = GOOMBA_INITIAL_POS_Y;
    coin_goomba_pointer->elapsed_time = 0;
}

void prepare_toad() {
    C2D_SpriteFromSheet(&toad_pointer->sprite, toad_spriteSheet, 0);
    C2D_SpriteSetCenter(&toad_pointer->sprite, 0.f, 0.f);
    C2D_SpriteSetPos(&toad_pointer->sprite, TOAD_INITIAL_POS_X, TOAD_INITIAL_POS_Y);
    C2D_SpriteSetRotation(&toad_pointer->sprite, C3D_Angle(0));
    toad_pointer->animation_delay = TOAD_CRY_TIME;
    toad_pointer->current_sprite = 0;

    C2D_SpriteFromSheet(&toadText_pointer->sprite, toadText_spriteSheet, 0);
    C2D_SpriteSetCenter(&toadText_pointer->sprite, 0.f, 0.f);
    C2D_SpriteSetPos(&toadText_pointer->sprite, TOAD_INITIAL_POS_X - 150, TOAD_INITIAL_POS_Y - 70);
    C2D_SpriteSetRotation(&toadText_pointer->sprite, C3D_Angle(0));
    toadText_pointer->visible = false;
    toadText_pointer->current_text = 0;

    C2D_SpriteFromSheet(&button_pointer->sprite, button_spriteSheet, 0);
    C2D_SpriteSetCenter(&button_pointer->sprite, 0.f, 0.f);
    C2D_SpriteSetPos(&button_pointer->sprite, TOAD_INITIAL_POS_X - 3, TOAD_INITIAL_POS_Y - 30);
    C2D_SpriteSetRotation(&button_pointer->sprite, C3D_Angle(0));
    button_pointer->visible = false;
}

void draw_title() {
    C2D_DrawSprite(&title.sprite);
    C2D_DrawSprite(&button_title.sprite);
}   

void draw_credits() {
    C2D_DrawSprite(&credits.sprite);
}   

void draw_mario() {
    if (mario_pointer->alive) {
        C2D_DrawSprite(&mario.sprite);
    }
}

void draw_characters() {
    C2D_DrawSprite(&toad.sprite);
    if (toadText_pointer->visible) {
        C2D_DrawSprite(&toadText.sprite);
    } else if (button_pointer->visible) {
        C2D_DrawSprite(&button.sprite);
    }
    if (goomba_pointer->alive) {
        C2D_DrawSprite(&goomba.sprite);
    }
    if (coin_goomba_pointer->visible) {
        C2D_DrawSprite(&goomba.coin.sprite);
    }
}

void draw_scoreboard() {
    C2D_DrawSprite(&scoreboard.sprite);
}

void draw_scenery() {
    C2D_DrawSprite(&background.sprite);
    C2D_DrawSprite(&block.sprite);
    if (coin_block_pointer->visible) {
        C2D_DrawSprite(&block.coin.sprite);
    }
}

void sceneInit() {
    g_dynamicBuf = C2D_TextBufNew(4096);
}

void scenesExit() {
    C2D_TextBufDelete(g_dynamicBuf);

    C2D_SpriteSheetFree(background_spriteSheet);
    C2D_SpriteSheetFree(mario_spriteSheet);
    C2D_SpriteSheetFree(toad_spriteSheet);
    C2D_SpriteSheetFree(goomba_spriteSheet);
    C2D_SpriteSheetFree(block_spriteSheet);
    C2D_SpriteSheetFree(coin_spriteSheet);
    C2D_SpriteSheetFree(title_spriteSheet);
	C2D_SpriteSheetFree(scoreboard_spriteSheet);
	C2D_SpriteSheetFree(credits_spriteSheet);
}

void initGame() {
    prepare_title();
    prepare_mario(MARIO_INITIAL_POS_X, MARIO_INITIAL_POS_Y);
    prepare_block();
    prepare_goomba();
    prepare_toad();
    prepare_game_screen();
	prepare_scoreboard();
	prepare_credits();
}

void dynamic_scoreboard() {
	C2D_TextBufClear(g_dynamicBuf);

 	char lifesBuffer[BUFFER_SIZE], coinsBuffer[BUFFER_SIZE], timeBuffer[BUFFER_SIZE];
	C2D_Text dynText_lifes, dynText_coins, dynText_time;

	snprintf(lifesBuffer, sizeof(lifesBuffer), "LIFES: %d", mario_pointer->lifes);
	snprintf(coinsBuffer, sizeof(coinsBuffer), "COINS: %d", mario_pointer->coins);
	snprintf(timeBuffer, sizeof(timeBuffer), "TIME: %llu s", timeState.current_seconds);

	C2D_TextParse(&dynText_lifes, g_dynamicBuf, lifesBuffer);
	C2D_TextParse(&dynText_coins, g_dynamicBuf, coinsBuffer);
	C2D_TextParse(&dynText_time, g_dynamicBuf, timeBuffer);

	C2D_TextOptimize(&dynText_lifes);
	C2D_TextOptimize(&dynText_coins);
	C2D_TextOptimize(&dynText_time);
	
	C2D_DrawText(&dynText_coins,  C2D_AlignLeft | C2D_WithColor, 50.0f, 20.0f, 0.5f, 0.5f, 0.5f, C2D_Color32f(1, 1, 1, 1));
	C2D_DrawText(&dynText_lifes,  C2D_AlignLeft | C2D_WithColor, 50.0f, 40.0f, 0.5f, 0.5f, 0.5f, C2D_Color32f(1, 1, 1, 1));
	C2D_DrawText(&dynText_time, C2D_AlignLeft | C2D_WithColor, 50.0f, 60.0f, 0.5f, 0.5f, 0.5f, C2D_Color32f(1, 1, 1, 1));

	
}

void drawerBottomScreenController() {
	draw_scoreboard();
	dynamic_scoreboard();
}


void drawerTopScreenController() {
    draw_scenery();
	if(credits_pointer->visible){
		draw_credits();
	} else if (title_pointer->visible) {
		draw_title();
	} else {
		draw_characters();
		draw_mario();
	}
}

void menuController(u32 kDown) {
    if (kDown & KEY_A) {
        title_pointer->visible = false;
        initTimeState();
    }
}

void restart() {
    block_pointer->current_sprite = BLOCK_UNTOUCHED;
	credits_pointer->visible = false;
	title_pointer->visible = true;
    setDefaultMarioValues();
	setDefaultGoombaValues();
}

void manageKeyPress(u32 kDown) {
    if ((kDown & KEY_A) && isInDialogPos()){
        if (toadText_pointer->visible) {
            toadText_pointer->visible = false;
			if(mario_pointer->coins == 2){
				credits_pointer->visible = true;
				}
        } else {
            toadText_pointer->visible = true;
            cwavPlay(toadSound, 0, -1);
        }
    }
}

void manageCredits(u32 kDown) {
	if (kDown & KEY_A){
		draw_credits();
		restart();
	}
}


void handleMarioDead() {
    float now_dead = svcGetSystemTick();
    mario_pointer->dead_elapsed_time += (now_dead - start_loop_time);

    if (mario_pointer->dead_elapsed_time <= mario_pointer->upwards_dead_anim_delay) {
        mario_pointer->dy -= 2;
    } else {
        mario_pointer->dy += 3;
        if (mario_pointer->dy > TOP_SCREEN_HEIGHT) {
            mario_pointer->alive = false;
            restart();
        }
    }

    controllerSprites_mario();
}

void gameInputController(u32 kDown, u32 kHeld, u32 kUp) {
    if (mario_pointer->state != MarioState::dead) {
        if (kDown) {
            manageKeyPress(kDown);
        }
        if (kUp) {
            setIdleMario(kUp);
        } else if (kHeld) {
            moveMario(kHeld);
        }
    } else {
        handleMarioDead();
    }
}

void handleToadCollision() {
    if (isInDialogPos()) {
        button_pointer->visible = true;
    } else {
        button_pointer->visible = false;
        toadText_pointer->visible = false;
    }
}

void makeMarioSmall() {
    mario_pointer->current_sprite = SMALL_RIGHT_IDLE;
    mario_pointer->left_walk_anim = smallLeftWalk;
    mario_pointer->max_jump_time = SMALL_JUMP;
    mario_pointer->base_y = MARIO_SMALL_Y;
    mario_pointer->right_walk_anim = smallRightWalk;
    mario_pointer->anim_num_sprites = SMALL_WAlK_ANIMATION_SIZE;
    mario_pointer->dy = MARIO_SMALL_Y;
    mario_pointer->left_jump_anim = smallJumpLeftAnim;
    mario_pointer->right_jump_anim = smallJumpRightAnim;
    mario_pointer->right_collision_margin = 15;
    mario_pointer->bottom_collision_margin = 30;
    mario_pointer->left_collision_margin = 10;
    mario_pointer->top_collision_margin = 15;
    mario_pointer->sprite_refresh = 3800;
    mario_pointer->small = true;
	mario_pointer->lifes = 1;
}


void handleGoombaCollision() {
    u64 now_collission = svcGetSystemTick();

    if (goomba_pointer->alive && isInCollissionWithGoomba() && goomba_pointer->dy == GOOMBA_INITIAL_POS_Y) {
        if(mario_pointer->state != MarioState::walking && mario_pointer->state != MarioState::dead){
            //printf("Mario ha matado a Goomba!\n");
            mario_pointer->dy = mario_pointer->dy - 5;
            coin_goomba_pointer->visible = true;
            coin_goomba_pointer->dx = goomba_pointer->dx;
            coin_goomba_pointer->dy = goomba_pointer->dy;
            goomba_pointer->current_sprite = GOOMBADEAD;
        } else {
            //printf("Mario ha sido asesinado por Goomba\n");
            if (mario_pointer->small) {

                mario_pointer->invincibility_elapsed_ms += (now_collission - start_loop_time);

                if (mario_pointer->invincibility_elapsed_ms >= mario_pointer->small_invincibility) {
                    mario_pointer->state = MarioState::dead;
                    mario_pointer->current_sprite = MARIODEAD;
                }

            } else {    
                makeMarioSmall();
            }
        }			
    }
    
    controllerSprites_mario();
}


void handleCollisions() {
    handleToadCollision();
    handleGoombaCollision();
} 

/****************************
 ***********DEBUG************
 ***************************/
void printDebugData(u32 kDown, u32 kHeld) {
}

void prepareSound(char* soundPath, CWAV* cwav) {
    FILE* file = fopen(soundPath, "rb");
    if (!file) {
        cwavFree(cwav);
        free(cwav);
        return;
    }
    fseek(file, 0, SEEK_END);
    u32 fileSize = ftell(file);
    void* buffer = linearAlloc(fileSize);
    if (!buffer) // This should never happen (unless we load a file too big to fit)
        svcBreak(USERBREAK_PANIC);
    fseek(file, 0, SEEK_SET); 
    fread(buffer, 1, fileSize, file);
    fclose(file);
    cwavLoad(cwav, buffer, 1);
    cwav->dataBuffer = buffer; 
    if (cwav->loadStatus != CWAV_SUCCESS) {
        cwavFree(cwav);
        linearFree(cwav->dataBuffer);

        free(cwav);
    }


}

/****************************
 **********SOUND*************
 ***************************/
void prepareSounds() {
    prepareSound("romfs:/ostd.cwav", ost);
    ost->volume = .5;
    cwavPlay(ost, 0, -1);

    prepareSound("romfs:/toad.cwav", toadSound);
    toadSound->volume = 1;
    //cwavPlay(ost, 0, -1);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    romfsInit();
    gfxInitDefault();
    //DEBUG
    //consoleInit(GFX_BOTTOM, NULL);

    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    cwavUseEnvironment(CWAV_ENV_DSP);
    ndspInit();

    prepareSounds();

    C3D_RenderTarget *top_screen = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	C3D_RenderTarget *bottom_screen = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
    prepare_sprites();

    initGame();

    sceneInit();

    while(aptMainLoop()) {
        //last_time = svcGetSystemTick ();
        /* Do your game loop here and swap buffers */
        if (!cwavIsPlaying(ost)) {
            cwavPlay(ost, 0 , -1);
        }
		
        start_loop_time = svcGetSystemTick();       

        hidScanInput();

		u32 kDown = hidKeysDown();
		u32 kHeld = hidKeysHeld();
        u32 kUp = hidKeysUp();
		if (credits_pointer->visible){
			manageCredits(kDown);
		} else if (title_pointer->visible) {
			menuController(kDown);
		} else {
            advanceTimeState();
			characterAnimations();
			marioPhysics();
			gameInputController(kDown, kHeld, kUp);
			handleCollisions();
		}

		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

		C2D_TargetClear(top_screen, BLACK);
		C2D_SceneBegin(top_screen);

        drawerTopScreenController();
		
		//gameInputController(kDown, kHeld);
		
        //printDebugData(kDown, kHeld);
		C2D_TargetClear(bottom_screen, BLACK);
		C2D_SceneBegin(bottom_screen);

		//Drawer BOTTOM Sprites
		drawerBottomScreenController();
		
        //C2D_Flush();
		

        C3D_FrameEnd(0);
    }

    scenesExit();

	C2D_Fini();
	C3D_Fini();
	gfxExit();
	return 0;
}