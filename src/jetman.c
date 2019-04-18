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

#define MIN_H_POS       FIX16(-8)
#define MAX_H_POS       FIX16(248)
#define TOP_V_POS 		FIX16(32)

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
static u8 landed(Vect2D_s16, const Level*);
static u8 reachedTop(Vect2D_s16, const Level*);
static u8 blockedOnTheLeft(Vect2D_s16, const Level*);
static u8 blockedOnTheRight(Vect2D_s16, const Level*);
static void updatePosition(JetmanPhysics*, const Level*);

static void updateJetmanAnim(const JetmanPhysics*, JetmanAnimation*);

Sprite* sprites[1];

JetmanPhysics p1_physics;
JetmanAnimation p1_anim;

s16 walk_anim[4] = { ANIM_STAND, ANIM_STEP_SHORT, ANIM_STEP_LONG, ANIM_STEP_SHORT };

fix16 jetman_floor;

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
	jetman_floor = FIX16((level->floor->yPos - 3) * 8);
	jetman_physics->v_pos = jetman_floor;
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

static Vect2D_f16 targetPosition(JetmanPhysics* jetman_physics) {

	fix16 target_x = jetman_physics->h_pos + jetman_physics->h_mov;
	fix16 target_y = jetman_physics->v_pos + jetman_physics->v_mov;

	Vect2D_f16 newPos = { .x = target_x, .y = target_y };

	return newPos;
}

static void updatePosition(JetmanPhysics* jetman_physics, const Level* level) {

	Vect2D_f16 target = targetPosition(jetman_physics);

	// horizontal position
	if (target.x > MAX_H_POS) {
		jetman_physics->h_pos = MIN_H_POS;

	} else if (target.x < MIN_H_POS) {
		jetman_physics->h_pos = MAX_H_POS;

	} else {
		jetman_physics->h_pos = target.x;
	}

	// vertical position
	if (target.y >= jetman_floor) {
		jetman_physics->v_pos = jetman_floor;
		jetman_physics->v_mov = SPEED_ZERO;

	} else if (target.y < TOP_V_POS) {
		jetman_physics->v_pos = TOP_V_POS;

	} else {
		jetman_physics->v_pos = target.y;
	}
}

static u8 landed(Vect2D_s16 pos, const Level* level) {

	if (pos.y >= jetman_floor) {
		return 1;
	}

	return 0;
}

static u8 reachedTop(Vect2D_s16 pos, const Level* level) {

	if (pos.y <= TOP_V_POS) {
		return 1;
	}

	return 0;
}

static u8 blockedOnTheLeft(Vect2D_s16 pos, const Level* level) {
	return 0;
}

static u8 blockedOnTheRight(Vect2D_s16 pos, const Level* level) {
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
