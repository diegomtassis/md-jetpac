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
#define H_SPEED_NORMAL  FIX16(1.5)
#define V_SPEED_UP    	FIX16(-1.5)
#define V_SPEED_DOWN    FIX16(1.5)

#define MIN_H_POS_PX       -8
#define MAX_H_POS_PX       248
#define TOP_V_POS_PX 		32

#define MIN_H_POS_PX_F16    FIX16(MIN_H_POS_PX)
#define MAX_H_POS_PX_F16    FIX16(MAX_H_POS_PX)
#define TOP_V_POS_PX_F16 	FIX16(TOP_V_POS_PX)

static void handleInputJetman();

typedef struct {
	fix16 h_pos;
	fix16 v_pos;
	fix16 h_mov;
	fix16 v_mov;
	s16 h_order;
	s16 v_order;
} JetmanPhysics;

typedef struct {
	s16 walk_idx;
	u8 walk_step_counter;
} JetmanAnimation;

static void initJetman(JetmanPhysics*, const Level*);
static void moveJetman(JetmanPhysics*, const Level*);
static void calculateMovement(JetmanPhysics*);
static void updatePosition(JetmanPhysics*, const Level*);
static Vect2D_f16 targetPosition(const JetmanPhysics*);
static Box targetBox(Vect2D_f16);
static u8 landed(Box, const Level*);
static u8 reachedTop(Box, const Level*);
static u8 blockedByLeft(Box, const Level*);
static u8 blockedByRight(Box, const Level*);

static void updateJetmanAnim(const JetmanPhysics*, JetmanAnimation*);

Sprite* sprites[1];

JetmanPhysics p1_physics;
JetmanAnimation p1_anim;

s16 walk_anim[4] = { ANIM_STAND, ANIM_STEP_SHORT, ANIM_STEP_LONG, ANIM_STEP_SHORT };

s16 floor_v_pos_px;
fix16 jetman_floor_f16;

void startJetman(const Level* level) {

	initJetman(&p1_physics, level);
	sprites[0] = SPR_addSprite(&jetman_sprite, fix16ToInt(p1_physics.h_pos), fix16ToInt(p1_physics.v_pos),
			TILE_ATTR(PAL0, TRUE, FALSE, FALSE));

	SPR_update();

	while (TRUE) {

		handleInputJetman();

		moveJetman(&p1_physics, level);
		updateJetmanAnim(&p1_physics, &p1_anim);

		SPR_update();
		VDP_waitVSync();
	}
}

static void initJetman(JetmanPhysics* jetman_physics, const Level* level) {

	jetman_physics->h_pos = FIX16(124);
	floor_v_pos_px = level->floor->yPos * 8;
	jetman_floor_f16 = FIX16(floor_v_pos_px - 24);
	jetman_physics->v_pos = FIX16(floor_v_pos_px);
	jetman_physics->h_mov = SPEED_ZERO;
	jetman_physics->v_mov = SPEED_ZERO;
	jetman_physics->h_order = 0;
	jetman_physics->v_order = 0;
}

static void moveJetman(JetmanPhysics* jetman_physics, const Level* level) {

	calculateMovement(jetman_physics);
	updatePosition(jetman_physics, level);

	SPR_setPosition(sprites[0], fix16ToInt(jetman_physics->h_pos), fix16ToInt(jetman_physics->v_pos));
}

static void calculateMovement(JetmanPhysics* jetman_physics) {

	// horizontal movement
	if (jetman_physics->h_order > 0) {
		jetman_physics->h_mov = H_SPEED_NORMAL;

	} else if (jetman_physics->h_order < 0) {
		jetman_physics->h_mov = -H_SPEED_NORMAL;

	} else {
		jetman_physics->h_mov = SPEED_ZERO;
	}

	// vertical movement
	if (jetman_physics->v_order < 0) {
		jetman_physics->v_mov = V_SPEED_UP;

	} else {
		jetman_physics->v_mov = V_SPEED_DOWN;
	}
}

static void updatePosition(JetmanPhysics* jetman_physics, const Level* level) {

	Vect2D_f16 target_pos = targetPosition(jetman_physics);
	Box target_box = targetBox(target_pos);

	// horizontal position
	if (target_pos.x > MAX_H_POS_PX_F16) {
		jetman_physics->h_pos = MIN_H_POS_PX_F16;

	} else if (target_pos.x < MIN_H_POS_PX_F16) {
		jetman_physics->h_pos = MAX_H_POS_PX_F16;

	} else if (!blockedByLeft(target_box, level) && !blockedByRight(target_box, level)) {
		jetman_physics->h_pos = target_pos.x;
	}

	// vertical position
	if (landed(target_box, level)) {
		jetman_physics->v_pos = jetman_floor_f16;
		jetman_physics->v_mov = SPEED_ZERO;

	} else if (reachedTop(target_box, level)) {
		jetman_physics->v_pos = TOP_V_POS_PX_F16;

	} else {
		jetman_physics->v_pos = target_pos.y;
	}
}

static Vect2D_f16 targetPosition(const JetmanPhysics* jetman_physics) {

	fix16 target_x = jetman_physics->h_pos + jetman_physics->h_mov;
	fix16 target_y = jetman_physics->v_pos + jetman_physics->v_mov;

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

	if (box.y + box.h >= floor_v_pos_px) {
		return 1;
	}

	return 0;
}

static u8 reachedTop(Box box, const Level* level) {

	if (box.y <= TOP_V_POS_PX) {
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

static void updateJetmanAnim(const JetmanPhysics* jetman_physics, JetmanAnimation* jetman_anim) {

	s16 anim_idx;

	if (jetman_physics->v_mov) {
		// somewhere in the air
		anim_idx = ANIM_FLY;
		jetman_anim->walk_idx = 0;

	} else {
		if (jetman_physics->h_mov) {
			// walking
			jetman_anim->walk_step_counter++;
			if (jetman_anim->walk_step_counter == STEPPING_SPEED) {
				// controlling the animation speed
				jetman_anim->walk_idx++;
				if (jetman_anim->walk_idx > 3) {
					jetman_anim->walk_idx = 0;
				}
				jetman_anim->walk_step_counter = 0;
			}
		}
		anim_idx = walk_anim[jetman_anim->walk_idx];
	}

	SPR_setAnim(sprites[0], anim_idx);

	if (jetman_physics->h_mov > 0)
		SPR_setHFlip(sprites[0], FALSE);
	else if (jetman_physics->h_mov < 0)
		SPR_setHFlip(sprites[0], TRUE);
}

static void handleInputJetman() {

	u16 value = JOY_readJoypad(JOY_1);

	if (value & BUTTON_B) {
		p1_physics.v_order = -1;
	} else {
		p1_physics.v_order = 0;
	}

	if (value & BUTTON_LEFT) {
		p1_physics.h_order = -1;
	} else if (value & BUTTON_RIGHT) {
		p1_physics.h_order = +1;
	} else {
		p1_physics.h_order = 0;
	}
}
