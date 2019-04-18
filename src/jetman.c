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

#define SPEED_ZERO    	FIX32(0)
#define H_SPEED_NORMAL  FIX32(1.5)
#define V_SPEED_UP    	FIX32(-1.5)
#define V_SPEED_DOWN    FIX32(1.5)

#define MIN_H_POS        FIX32(-8)
#define MAX_H_POS        FIX32(248)
#define FLOOR_JETMAN_V_POS 		FIX32(200 - 8*3)
#define TOP_V_POS 		FIX32(32)

static void handleInputJetman();

typedef struct {
	fix32 h_pos;
	fix32 v_pos;
	fix32 h_mov;
	fix32 v_mov;
	s16 h_order;
	s16 v_order;
} JetmanPhysics;

typedef struct {
	s16 walk_idx;
	u8 walk_step_counter;
} JetmanAnimation;

static void initJetmanPhysics(JetmanPhysics*);
static void updateJetmanPhysics(JetmanPhysics*);
static void updateJetmanAnim(const JetmanPhysics*, JetmanAnimation*);

Sprite* sprites[1];

JetmanPhysics p1_physics;
JetmanAnimation p1_anim;

s16 walk_anim[4] = { ANIM_STAND, ANIM_STEP_SHORT, ANIM_STEP_LONG, ANIM_STEP_SHORT };

int startJetman(const Level * level) {

	initJetmanPhysics(&p1_physics);
	sprites[0] = SPR_addSprite(&jetman_sprite, fix32ToInt(p1_physics.h_pos), fix32ToInt(p1_physics.v_pos),
			TILE_ATTR(PAL0, TRUE, FALSE, FALSE));

	SPR_update();

	while (TRUE) {

		handleInputJetman();

		updateJetmanPhysics(&p1_physics);
		updateJetmanAnim(&p1_physics, &p1_anim);

		SPR_update();
		VDP_waitVSync();
	}

	return 0;
}

static void initJetmanPhysics(JetmanPhysics* jetman_physics) {

	jetman_physics->h_pos = FIX32(145);
	jetman_physics->v_pos = FLOOR_JETMAN_V_POS;
	jetman_physics->h_mov = SPEED_ZERO;
	jetman_physics->v_mov = SPEED_ZERO;
	jetman_physics->h_order = 0;
	jetman_physics->v_order = 0;
}

static void updateJetmanPhysics(JetmanPhysics* jetman_physics) {

	// Horizontal movement
	if (jetman_physics->h_order > 0) {
		jetman_physics->h_mov = H_SPEED_NORMAL;

	} else if (jetman_physics->h_order < 0) {
		jetman_physics->h_mov = -H_SPEED_NORMAL;

	} else {
		jetman_physics->h_mov = SPEED_ZERO;
	}
	jetman_physics->h_pos += jetman_physics->h_mov;

	if (jetman_physics->h_pos >= MAX_H_POS) {
		jetman_physics->h_pos = MIN_H_POS;

	} else if (jetman_physics->h_pos <= MIN_H_POS) {
		jetman_physics->h_pos = MAX_H_POS;
	}

	// Vertical movement
	if (jetman_physics->v_order < 0) {
		jetman_physics->v_mov = V_SPEED_UP;

	} else {
		jetman_physics->v_mov = V_SPEED_DOWN;
	}
	jetman_physics->v_pos += jetman_physics->v_mov;

	if (jetman_physics->v_pos > FLOOR_JETMAN_V_POS) {
		jetman_physics->v_pos = FLOOR_JETMAN_V_POS;
		jetman_physics->v_mov = SPEED_ZERO;

	} else if (jetman_physics->v_pos < TOP_V_POS) {
		jetman_physics->v_pos = TOP_V_POS;
	}

	SPR_setPosition(sprites[0], fix32ToInt(jetman_physics->h_pos), fix32ToInt(jetman_physics->v_pos));
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
