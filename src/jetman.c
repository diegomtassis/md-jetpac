/*
 * jetman.c
 *
 *  Created on: Apr 16, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

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

static void updateJetmanPhysics();
static void updateJetmanAnim();

Sprite* sprites[1];

fix32 h_pos;
fix32 v_pos;
fix32 h_mov;
fix32 v_mov;
s16 h_order;
s16 v_order;

s16 walk_anim[4] = { ANIM_STAND, ANIM_STEP_SHORT, ANIM_STEP_LONG, ANIM_STEP_SHORT };
s16 walk_idx = 0;
u8 walk_step_counter = 0;

int startJetman() {

	SPR_init(16, 256, 256);

	h_pos = FIX32(145);
	v_pos = FLOOR_JETMAN_V_POS;
	h_mov = SPEED_ZERO;
	v_mov = SPEED_ZERO;
	h_order = 0;
	v_order = 0;

	sprites[0] = SPR_addSprite(&jetman_sprite, fix32ToInt(h_pos), fix32ToInt(v_pos),
			TILE_ATTR(PAL0, TRUE, FALSE, FALSE));

	SPR_update();

	while (TRUE) {

		handleInputJetman();

		updateJetmanPhysics();
		updateJetmanAnim();

		SPR_update();

		VDP_waitVSync();
	}

	return 0;
}

static void updateJetmanPhysics() {

	// Horizontal movement
	if (h_order > 0) {
		h_mov = H_SPEED_NORMAL;

	} else if (h_order < 0) {
		h_mov = -H_SPEED_NORMAL;

	} else {
		h_mov = SPEED_ZERO;
	}
	h_pos += h_mov;

	if (h_pos >= MAX_H_POS) {
		h_pos = MIN_H_POS;

	} else if (h_pos <= MIN_H_POS) {
		h_pos = MAX_H_POS;
	}

	// Vertical movement
	if (v_order < 0) {
		v_mov = V_SPEED_UP;

	} else {
		v_mov = V_SPEED_DOWN;
	}
	v_pos += v_mov;

	if (v_pos > FLOOR_JETMAN_V_POS) {
		v_pos = FLOOR_JETMAN_V_POS;
		v_mov = SPEED_ZERO;

	} else if (v_pos < TOP_V_POS) {
		v_pos = TOP_V_POS;
	}

	SPR_setPosition(sprites[0], fix32ToInt(h_pos), fix32ToInt(v_pos));
}

static void updateJetmanAnim() {

	s16 anim;

	if (v_mov) {
		// somewhere in the air
		anim = ANIM_FLY;
		walk_idx = 0;

	} else {
		if (h_mov) {
			// walking
			walk_step_counter++;
			if (walk_step_counter == STEPPING_SPEED) {
				// controlling the animation speed
				walk_idx++;
				if (walk_idx > 3) {
					walk_idx = 0;
				}
				walk_step_counter = 0;
			}
		}
		anim = walk_anim[walk_idx];
	}

	SPR_setAnim(sprites[0], anim);

	if (h_mov > 0)
		SPR_setHFlip(sprites[0], FALSE);
	else if (h_mov < 0)
		SPR_setHFlip(sprites[0], TRUE);
}

static void handleInputJetman() {

	u16 value = JOY_readJoypad(JOY_1);

	if (value & BUTTON_B) {
		v_order = -1;
	} else {
		v_order = 0;
	}

	if (value & BUTTON_LEFT) {
		h_order = -1;
	} else if (value & BUTTON_RIGHT) {
		h_order = +1;
	} else {
		h_order = 0;
	}
}
