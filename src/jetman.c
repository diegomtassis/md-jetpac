/*
 * jetman.c
 *
 *  Created on: Apr 16, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../inc/levels.h"
#include "../res/sprite.h"

#define ANIM_STAND    	0
#define ANIM_STEP_SHORT	1
#define ANIM_STEP_LONG	2
#define ANIM_FLY		3
#define STEPPING_SPEED    	6	// 0 Maximum

#define SPEED_ZERO    	FIX16(0)
#define SPEED_H_NORMAL  FIX16(1.5)
#define SPEED_V_UP    	FIX16(-1.5)
#define SPEED_V_DOWN    FIX16(1.5)

#define MIN_POS_H_PX       -8
#define MAX_POS_H_PX       248
#define MAX_POS_V_PX 		32

#define MIN_POS_H_PX_F16    FIX16(MIN_POS_H_PX)
#define MAX_POS_H_PX_F16    FIX16(MAX_POS_H_PX)
#define MAX_POS_V_PX_F16 	FIX16(MAX_POS_V_PX)

static void handleInputJetman();

typedef struct {
	s16 walk_idx;
	u8 walk_step_counter;
} JetmanAnimation;

static Jetman* createPlayer1(const Level*);
static void moveJetman(Jetman*, const Level*);
static void calculateMovement(Jetman*);
static void updatePosition(Jetman*, const Level*);
static Vect2D_f16 targetPosition(const Jetman*);
static Box targetBox(Vect2D_f16);
static u8 landed(Box, const Level*);
static u8 reachedTop(Box, const Level*);
static u8 blockedByLeft(Box, const Level*);
static u8 blockedByRight(Box, const Level*);

static void updateJetmanAnim(const Jetman*, JetmanAnimation*);

Sprite* sprites[1];

Jetman* player1;
JetmanAnimation p1_anim;

s16 walk_anim[4] = { ANIM_STAND, ANIM_STEP_SHORT, ANIM_STEP_LONG, ANIM_STEP_SHORT };

s16 floor_pos_v_px;
fix16 jetman_floor_f16;

void startJetman(const Level* level) {

	player1 = createPlayer1(level);
	sprites[0] = SPR_addSprite(&jetman_sprite, fix16ToInt(player1->object.pos.x), fix16ToInt(player1->object.pos.y),
			TILE_ATTR(PAL0, TRUE, FALSE, FALSE));

	SPR_update();

	while (TRUE) {

		handleInputJetman();

		moveJetman(player1, level);
		updateJetmanAnim(player1, &p1_anim);

		SPR_update();
		VDP_waitVSync();
	}
}

static Jetman* createPlayer1(const Level* level) {

	Jetman* p1 = MEM_alloc(sizeof(Jetman));

	p1->object.pos.x = FIX16(124);
	floor_pos_v_px = level->floor->pos_t.y * 8;
	jetman_floor_f16 = FIX16(floor_pos_v_px - 24);
	p1->object.pos.y = jetman_floor_f16;

	p1->object.mov.x = SPEED_ZERO;
	p1->object.mov.y = SPEED_ZERO;
	p1->object.size.x = 16;
	p1->object.size.y = 24;
	p1->object.mov.y = SPEED_ZERO;
	p1->order.x = 0;
	p1->order.y = 0;

	Box_f16 box = { .x = p1->object.pos.x, .y = p1->object.pos.y, .w = 16, .h = 24 };
	p1->object.box = &box;

	return p1;
}

static void moveJetman(Jetman* player, const Level* level) {

	calculateMovement(player);
	updatePosition(player, level);

	SPR_setPosition(sprites[0], fix16ToInt(player->object.pos.x), fix16ToInt(player->object.pos.y));
}

static void calculateMovement(Jetman* player) {

	// horizontal movement
	if (player->order.x > 0) {
		player->object.mov.x = SPEED_H_NORMAL;

	} else if (player->order.x < 0) {
		player->object.mov.x = -SPEED_H_NORMAL;

	} else {
		player->object.mov.x = SPEED_ZERO;
	}

	// vertical movement
	if (player->order.y < 0) {
		player->object.mov.y = SPEED_V_UP;

	} else {
		player->object.mov.y = SPEED_V_DOWN;
	}
}

static void updatePosition(Jetman* player, const Level* level) {

	Vect2D_f16 target_pos = targetPosition(player);
	Box target_box = targetBox(target_pos);

	// horizontal position
	if (target_pos.x > MAX_POS_H_PX_F16) {
		player->object.pos.x = MIN_POS_H_PX_F16;

	} else if (target_pos.x < MIN_POS_H_PX_F16) {
		player->object.pos.x = MAX_POS_H_PX_F16;

	} else if (!blockedByLeft(target_box, level) && !blockedByRight(target_box, level)) {
		player->object.pos.x = target_pos.x;
	}

	// vertical position
	if (landed(target_box, level)) {
		player->object.pos.y = jetman_floor_f16;
		player->object.mov.y = SPEED_ZERO;

	} else if (reachedTop(target_box, level)) {
		player->object.pos.y = MAX_POS_V_PX_F16;

	} else {
		player->object.pos.y = target_pos.y;
	}
}

static Vect2D_f16 targetPosition(const Jetman* player) {

	fix16 target_x = player->object.pos.x + player->object.mov.x;
	fix16 target_y = player->object.pos.y + player->object.mov.y;

	Vect2D_f16 newPos = { .x = target_x, .y = target_y };

	return newPos;
}

static Box targetBox(Vect2D_f16 pos) {

	Box box;

	box.x = fix16ToInt(pos.x);
	box.y = fix16ToInt(pos.y);
	box.w = 16;
	box.h = 24;

	return box;
}

static u8 landed(Box box, const Level* level) {

	if (box.y + box.h >= floor_pos_v_px) {
		return 1;
	}

	return 0;
}

static u8 reachedTop(Box box, const Level* level) {

	if (box.y <= MAX_POS_V_PX) {
		return 1;
	}

	return 0;
}

static u8 blockedByLeft(Box box, const Level* level) {

	return 0;
}

static u8 blockedByRight(Box box, const Level* level) {

//	for (u8 i = 0; i < level->num_platforms; i++) {
//		Platform* platform = level->platforms[i];
//		fix16 platform_H_px = FIX16(platform->xPos * 8);
//		fix16 platform_V_px = FIX16(platform->yPos * 8);
//		if (platform_H_px <= pos.x && platform->yPos * 8) {
//			return 1;
//		}
//	}

	return 0;
}

static void updateJetmanAnim(const Jetman* player, JetmanAnimation* p_anim) {

	s16 anim_idx;

	if (player->object.mov.y) {
		// somewhere in the air
		anim_idx = ANIM_FLY;
		p_anim->walk_idx = 0;

	} else {
		if (player->object.mov.x) {
			// walking
			p_anim->walk_step_counter++;
			if (p_anim->walk_step_counter == STEPPING_SPEED) {
				// controlling the animation speed
				p_anim->walk_idx++;
				if (p_anim->walk_idx > 3) {
					p_anim->walk_idx = 0;
				}
				p_anim->walk_step_counter = 0;
			}
		}
		anim_idx = walk_anim[p_anim->walk_idx];
	}

	SPR_setAnim(sprites[0], anim_idx);

	if (player->object.mov.x > 0)
		SPR_setHFlip(sprites[0], FALSE);
	else if (player->object.mov.x < 0)
		SPR_setHFlip(sprites[0], TRUE);
}

static void handleInputJetman() {

	u16 value = JOY_readJoypad(JOY_1);

	if (value & BUTTON_B) {
		player1->order.y = -1;
	} else {
		player1->order.y = 0;
	}

	if (value & BUTTON_LEFT) {
		player1->order.x = -1;
	} else if (value & BUTTON_RIGHT) {
		player1->order.x = +1;
	} else {
		player1->order.x = 0;
	}
}
