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

static C2D_SpriteSheet mario_spriteSheet;
static C2D_SpriteSheet background_spriteSheet;
static C2D_SpriteSheet block_spriteSheet;
static C2D_SpriteSheet toad_spriteSheet;
static C2D_SpriteSheet toadText_spriteSheet;
static C2D_SpriteSheet goomba_spriteSheet;
static C2D_SpriteSheet button_spriteSheet;
static C2D_SpriteSheet coin_spriteSheet;
static C2D_SpriteSheet title_spriteSheet;

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


int sprite_id = RIGHT_WALK_1;
u64 start_loop_time = svcGetSystemTick();
u64 now = svcGetSystemTick();
u64 ms_elapsed_time = now - start_loop_time;
static u64 sprite_refresh = 2800;
static u64 max_jump = 65000000;
u64 jump_start;
bool can_jump = true;

static CWAV* ost = (CWAV*) malloc(sizeof(CWAV));
static CWAV* toadSound = (CWAV*) malloc(sizeof(CWAV));

bool array_contains(int val, int array[], int* pos) {
    //printf("SIZE OF ARRAY IS %d\n", sizeof(array) / sizeof(array[0]));
     int i;
    for(i = 0; i < WAlK_ANIMATION_SIZE; i++)
    {
        if(array[i] == val) {
			*pos = i;
            return 1;
		}
    }

	return 0;
} 

void controllerSprites_mario(int spriteid)
{
	// Sprite
	C2D_SpriteFromSheet(&mario_pointer->sprite, mario_spriteSheet, spriteid);
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
    bool colission = mario_pointer->dx >= BLOCK_INITIAL_POS_X - 25 
    && mario_pointer->dx <= BLOCK_INITIAL_POS_X
    && mario_pointer->dy <= BLOCK_INITIAL_POS_Y + 10
    && mario_pointer->dy >= BLOCK_INITIAL_POS_Y - 10;
    if (colission && block_pointer->current_sprite != BLOCK_TOUCHED) {
        block_pointer->current_sprite = BLOCK_TOUCHED;
        controllerSprites_block();
        coin_block_pointer->visible = true;
    }  //Aquí el bloque cambia al otro sprite y suelta moneda
    return colission;
}

//Solo comprueba si está en colisión desde el eje X, si lo hace desde falling (que se comprobará desde otro sitio) se mata al bicho y consigue una moneda
bool isInCollissionWithGoomba() {
    bool colission = mario_pointer->dx >= goomba_pointer-> dx - 23
    && mario_pointer->dx <= goomba_pointer->dx + 1
    && mario_pointer->dy + 45 >= goomba_pointer->dy;
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
            //Sumar contador monedas
        }
    }

    if (coin_goomba_pointer->visible) {
        coin_goomba_pointer->elapsed_time += (chars_now - start_loop_time);
        if (coin_goomba_pointer->elapsed_time <= coin_goomba_pointer->animation_time) {
            coin_goomba_pointer->dy -= 3;
        } else {
            coin_goomba_pointer->visible = false;
            //Sumar contador monedas
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

    if (mario_pointer->state == MarioState::falling) {
        //printf("MARIO IS FALLING\n");
        new_y += mario_pointer->fall_speed;

        if (mario_pointer->dy >= MARIO_INITIAL_POS_Y) {
//        printf("MARIO SHOULD BE WALKING\n");
            new_y = MARIO_INITIAL_POS_Y;
            mario_pointer->state = MarioState::walking;
            if (sprite_id == FALL_LEFT || sprite_id == JUMP_LEFT) {
                sprite_id = LEFT_WALK_1;
            } else {
                sprite_id = RIGHT_WALK_1;
            }
        }
    } else if (mario_pointer->state == MarioState::jumping && (now - jump_start) >= max_jump) {
        can_jump = false;
        mario_pointer->state = MarioState::falling;
        if (sprite_id == JUMP_LEFT) {
            sprite_id = FALL_LEFT;
        } else {
            sprite_id = FALL_RIGHT;
        }
    }

    C2D_SpriteMove(&mario_pointer->sprite, mario_pointer->dx = new_x, mario_pointer->dy = new_y);
    controllerSprites_mario(sprite_id);
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
    ms_elapsed_time += (now - start_loop_time);
    float new_y = mario_pointer->dy;
    float new_x = mario_pointer->dx;
    float speed;


        if ((kHeld & KEY_A) && can_jump) {
            if (mario_pointer->state == MarioState::walking) {
                if (!isInDialogPos()) {
                    jump_start = svcGetSystemTick();
                    mario_pointer->state = MarioState::jumping;
                    new_y -= mario_pointer->jump_speed;

                    if (array_contains(sprite_id, rightWalk, &pos)) {
                        //printf("CONTAINS %d in POS %d JUMP RIGHT\n", sprite_id, pos);
                        sprite_id = JUMP_RIGHT;
                    } else {
                        //printf("CONTAINS %d in POS %d JUMP LEFT\n", sprite_id, pos);
                        sprite_id = JUMP_LEFT;
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
        if (ms_elapsed_time >= sprite_refresh) {
		    if (array_contains(sprite_id, leftWalk, &pos)) {
			    sprite_id = leftWalk[(pos + 1) % 3];
            //printf("ADVANCING LEFT\n");
                ms_elapsed_time = 0;
            } else {
            //printf("START TO LOOK LEFT\n");
                switch(mario_pointer->state) {
                    case MarioState::walking: 
			            sprite_id = LEFT_WALK_1;
                        break;
                    case MarioState::jumping:
                        sprite_id = JUMP_LEFT;
                        break;
                    case MarioState::falling:
                        sprite_id = FALL_LEFT;
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
        if (ms_elapsed_time >= sprite_refresh) {
		    if (array_contains(sprite_id, rightWalk, &pos)) {
			    sprite_id = rightWalk[(pos + 1) % 3];
                ms_elapsed_time = 0;
            //printf("ADVANCING RIGHT\n");
            } else {
                switch(mario_pointer->state) {
             //                   printf("START TO LOOK RIGHT\n");
                    case MarioState::walking: 
			            sprite_id = RIGHT_WALK_1;
                        break;
                    case MarioState::jumping:
                        sprite_id = JUMP_RIGHT;
                        break;
                    case MarioState::falling:
                        sprite_id = FALL_RIGHT;
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
    controllerSprites_mario(sprite_id);
}

void setIdleMario(int kUp) {
    if (kUp & KEY_A) {
        if (mario_pointer->state == MarioState::jumping) {
            mario_pointer->state = MarioState::falling;
        }
        can_jump = true;
    }
    int pos = -1;
    if (mario_pointer->state == MarioState::walking) {
        if (array_contains(sprite_id, rightWalk, &pos)) {
            //printf("AQUI RIGHT WALK EN KEY UP\n");
            sprite_id = RIGHT_WALK_1;
        } else {
            //printf("AQUI LEFT WALK EN KEY UP sprite id %d  POS %d\n", sprite_id, pos);
            sprite_id = LEFT_WALK_1;
        }
    }
    controllerSprites_mario(sprite_id);
}

void prepare_sprites() {
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

void prepare_mario(int posX, int posY) {
    C2D_SpriteFromSheet(&mario_pointer->sprite, mario_spriteSheet, 4);
    C2D_SpriteSetCenter(&mario_pointer->sprite, 0.f, 0.f);
    C2D_SpriteSetPos(&mario_pointer->sprite, posX, posY);
    C2D_SpriteSetRotation(&mario_pointer->sprite,C3D_Angle(0));
    mario_pointer->alive = true;
    mario_pointer->dx = posX;
    mario_pointer->dy = posY;
    mario_pointer->speed = MARIO_SPEED;
    mario_pointer->air_speed = MARIO_AIR_SPEED;
    mario_pointer->jump_speed = MARIO_JUMP_SPEED;
    mario_pointer->fall_speed = MARIO_FALL_SPEED;
    mario_pointer->state = MarioState::walking;
    mario_pointer->dead_elapsed_time = 0;
    mario_pointer->upwards_dead_anim_delay = MARIO_UPWARDS_DEAD_DELAY;
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
}

void initGame() {
    prepare_title();
    prepare_mario(MARIO_INITIAL_POS_X, MARIO_INITIAL_POS_Y);
    prepare_block();
    prepare_goomba();
    prepare_toad();
    prepare_game_screen();
}


void drawerTopScreenController() {
    draw_scenery();

    if (title_pointer->visible) {
        draw_title();
    } else {
        draw_characters();
        draw_mario();
    }
}

void manageKeyPress(u32 kDown) {
    if ((kDown & KEY_A) && isInDialogPos()){
        if (toadText_pointer->visible) {
            toadText_pointer->visible = false;
        } else {
            toadText_pointer->visible = true;
            cwavPlay(toadSound, 0, -1);
        }
    }
}

void hideTitle(u32 kDown) {
    if (kDown & KEY_A) {
        title_pointer->visible = false;
    }
}

void restart() {
    mario_pointer->dx = MARIO_INITIAL_POS_X;
    mario_pointer->dy = MARIO_INITIAL_POS_Y;
    block_pointer->current_sprite = BLOCK_UNTOUCHED;
    mario_pointer->state = MarioState::walking;
    mario_pointer->dead_elapsed_time = 0;
    mario_pointer->alive = true;
    title_pointer->visible = true;
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

    controllerSprites_mario(sprite_id);
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

void handleGoombaCollision() {
    if (goomba_pointer->alive && isInCollissionWithGoomba() && goomba_pointer->dy == GOOMBA_INITIAL_POS_Y) {
        if(mario_pointer->state != MarioState::walking && mario_pointer->state != MarioState::dead){
            printf("Mario ha matado a Goomba!\n");
            mario_pointer->dy = mario_pointer->dy - 5;
            coin_goomba_pointer->visible = true;
            coin_goomba_pointer->dx = goomba_pointer->dx;
            coin_goomba_pointer->dy = goomba_pointer->dy;
            goomba_pointer->current_sprite = GOOMBADEAD;
            controllerSprites_mario(sprite_id);
        } else {
            printf("Mario ha sido asesinado por Goomba\n");
            mario_pointer->state = MarioState::dead;
            sprite_id = MARIODEAD;
        }			
    }
}


void handleCollisions() {
    handleToadCollision();
    handleGoombaCollision();
} 

/****************************
 ***********DEBUG************
 ***************************/
void printDebugData(u32 kDown, u32 kHeld) {

    int pos;
    //printf("ARRAY CONTAINS 8 ???? %d\n", array_contains(8, rightWalk, &pos));

/*     consoleClear();
    int pos;
    printf("IS LEFTWALK 1 %d  in rightWalk? %d\n", LEFT_WALK_1, array_contains(LEFT_WALK_1, rightWalk, &pos));
    printf("IS LEFTWALK 2 %d  in rightWalk? %d\n", LEFT_WALK_2, array_contains(LEFT_WALK_2, rightWalk, &pos));
    printf("IS LEFTWALK 3 %d  in rightWalk? %d\n", LEFT_WALK_3, array_contains(LEFT_WALK_3, rightWalk, &pos));
    printf("IS RIGHTWALK 1 %d  in leftWalk? %d\n", RIGHT_WALK_1, array_contains(RIGHT_WALK_1, leftWalk, &pos));
    printf("IS RIGHTWALK 2 %d  in leftWalk? %d\n", RIGHT_WALK_2, array_contains(RIGHT_WALK_2, leftWalk, &pos));
    printf("IS RIGHTWALK 3 %d  in leftWalk? %d\n", RIGHT_WALK_3, array_contains(RIGHT_WALK_3, leftWalk, &pos));  */


/*     printf("\nLEFTWALK\n ---------------------- \n[");
    for (int i = 0; i < sizeof(leftWalk); i++) {
        printf("%d, ", leftWalk[i]);
    }
    printf("]\n\n\n");
    printf("RIGHTWALK\n ---------------------- \n[");
    for (int i = 0; i < sizeof(rightWalk); i++) {
        printf("%d, ", rightWalk[i]);
    }
    printf("]\n"); */
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
    consoleInit(GFX_BOTTOM, NULL);

    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    cwavUseEnvironment(CWAV_ENV_DSP);
    ndspInit();

    prepareSounds();

    C3D_RenderTarget *top_screen = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

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
		
        if (title_pointer->visible) {
            hideTitle(kDown);
        } else {
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

        printDebugData(kDown, kHeld);
		
        //C2D_Flush();

        C3D_FrameEnd(0);


        //if (now_time < last_time + FPS) svcSleepThread (last_time + FPS - now_time);
    }

    scenesExit();

	C2D_Fini();
	C3D_Fini();
	gfxExit();
	return 0;
}