/*
 * jetman.c
 *
 *  Created on: Apr 16, 2019
 *      Author: diegomtassis
 */

#include "../inc/jetman.h"

#include <genesis.h>

#include "../inc/physics.h"
#include "../res/sprite.h"

#define ANIM_WALK		0
#define ANIM_FLY		1
#define STEPPING_SPEED	6 // 0 Maximum

#define SPEED_ZERO		FIX16(0)
#define SPEED_H_NORMAL	FIX16(1.5)
#define SPEED_V_UP		FIX16(-1.5)
#define SPEED_V_DOWN	FIX16(1.5)

#define JETMAN_HEIGHT 24
#define JETMAN_WIDTH 16

#define MIN_POS_H_PX_F16	LEFT_POS_H_PX_F16 - FIX16(-8)
#define MAX_POS_H_PX_F16	RIGHT_POS_H_PX_F16 - FIX16(8)
#define MIN_POS_V_PX_F16	TOP_POS_V_PX_F16

static Jetman* createPlayer1(const Level*);
static void handleInputJetman(Jetman*);

static void moveToStart(Jetman* jetman, const Level* level);
static void moveJetman(Jetman*, const Level*);
static void calculateNextMovement(Jetman*);
static void updatePosition(Jetman*, const Level*);
static fix16 landed(Box_f16, const Level*);
static fix16 reachedTop(Box_f16, const Level*);
static fix16 blockedByLeft(Box_f16, const Level*);
static fix16 blockedByRight(Box_f16, const Level*);

static void drawJetman(Jetman*);

void startJetman(Level* level) {

	level->jetman = createPlayer1(level);
	level->jetman->sprite = SPR_addSprite(&jetman_sprite, fix16ToInt(level->jetman->object.pos.x),
			fix16ToInt(level->jetman->object.pos.y), TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
}

void releaseJetman(Jetman* jetman) {

	SPR_setVisibility(jetman->sprite, FALSE);
	SPR_releaseSprite(jetman->sprite);
	MEM_free(jetman->sprite);
	MEM_free(jetman->object.box);
	MEM_free(jetman);
}

void resetJetman(Level* level) {

	moveToStart(level->jetman, level);
	level->jetman->alive = TRUE;
}

void jetmanActs(const Level* level) {

	handleInputJetman(level->jetman);

	moveJetman(level->jetman, level);
	drawJetman(level->jetman);
}

static Jetman* createPlayer1(const Level* level) {

	Jetman* jetman = MEM_alloc(sizeof(Jetman));

	jetman->object.size.x = JETMAN_WIDTH;
	jetman->object.size.y = JETMAN_HEIGHT;

	jetman->object.box = MEM_alloc(sizeof(Box_f16));
	jetman->object.box->x = JETMAN_WIDTH;
	jetman->object.box->y = JETMAN_HEIGHT;

	moveToStart(jetman, level);
	jetman->alive = TRUE;

	return jetman;
}

static void moveToStart(Jetman* jetman, const Level* level) {

	jetman->object.pos.x = FIX16(124);
	jetman->object.pos.y = fix16Sub(level->floor->object.pos.y, FIX16(8*3));
	jetman->object.mov.x = SPEED_ZERO;
	jetman->object.mov.y = SPEED_ZERO;

	jetman->object.box->x = jetman->object.pos.x;
	jetman->object.box->y = jetman->object.pos.y;
}

static void moveJetman(Jetman* jetman, const Level* level) {

	calculateNextMovement(jetman);
	updatePosition(jetman, level);

	SPR_setPosition(jetman->sprite, fix16ToInt(jetman->object.pos.x), fix16ToInt(jetman->object.pos.y));
}

static void calculateNextMovement(Jetman* jetman) {

	// horizontal movement
	if (jetman->order.x > 0) {
		jetman->object.mov.x = SPEED_H_NORMAL;

	} else if (jetman->order.x < 0) {
		jetman->object.mov.x = -SPEED_H_NORMAL;

	} else {
		jetman->object.mov.x = SPEED_ZERO;
	}

	// vertical movement
	if (jetman->order.y < 0) {
		jetman->object.mov.y = SPEED_V_UP;

	} else {
		jetman->object.mov.y = SPEED_V_DOWN;
	}
}

static void updatePosition(Jetman* jetman, const Level* level) {

	// horizontal position
	Box_f16 target_h = targetHBox(&jetman->object, JETMAN_WIDTH, JETMAN_HEIGHT);
	if (target_h.x > MAX_POS_H_PX_F16) {
		jetman->object.pos.x = MIN_POS_H_PX_F16;

	} else if (target_h.x < MIN_POS_H_PX_F16) {
		jetman->object.pos.x = MAX_POS_H_PX_F16;

	} else {

		fix16 blockedHorizontally = blockedByLeft(target_h, level);
		if (!blockedHorizontally) {
			blockedHorizontally = blockedByRight(target_h, level);
		}

		if (blockedHorizontally) {
			jetman->object.pos.x = blockedHorizontally;

		} else {
			jetman->object.pos.x = target_h.x;
		}
	}

	// vertical position
	Box_f16 target_v = targetVBox(&jetman->object, JETMAN_WIDTH, JETMAN_HEIGHT);
	fix16 landed_pos_y = landed(target_v, level);
	if (landed_pos_y) {
		jetman->object.pos.y = landed_pos_y;
		jetman->object.mov.y = SPEED_ZERO;

	} else {
		fix16 top_pos_y = reachedTop(target_v, level);
		if (top_pos_y) {
			jetman->object.pos.y = top_pos_y;

		} else {
			jetman->object.pos.y = target_v.y;
		}
	}

	// update box
	jetman->object.box->x = jetman->object.pos.x;
	jetman->object.box->y = jetman->object.pos.y;
}

static fix16 landed(Box_f16 subject_box, const Level* level) {

	fix16 landedInPlatform = hitAbove(subject_box, *level->floor->object.box);
	if (landedInPlatform) {
		return landedInPlatform;
	}

	for (u8 i = 0; i < level->num_platforms; i++) {
		landedInPlatform = hitAbove(subject_box, *level->platforms[i]->object.box);
		if (landedInPlatform) {
			return landedInPlatform;
		}
	}

	return 0;
}

static fix16 reachedTop(Box_f16 subject_box, const Level* level) {

	if (subject_box.y <= MIN_POS_V_PX_F16) {
		return MIN_POS_V_PX_F16;
	}

	fix16 reachedPlatformTop;
	for (u8 i = 0; i < level->num_platforms; i++) {
		reachedPlatformTop = hitUnder(subject_box, *level->platforms[i]->object.box);
		if (reachedPlatformTop) {
			return reachedPlatformTop;
		}
	}

	reachedPlatformTop = hitUnder(subject_box, *level->floor->object.box);
	if (reachedPlatformTop) {
		return reachedPlatformTop;
	}

	return 0;
}

static fix16 blockedByLeft(Box_f16 target_box, const Level* level) {

	fix16 blocked = hitLeft(target_box, *level->floor->object.box);
	if (blocked) {
		return blocked;
	}

	for (u8 i = 0; i < level->num_platforms; i++) {
		blocked = hitLeft(target_box, *level->platforms[i]->object.box);
		if (blocked) {
			return blocked;
		}
	}

	return 0;
}

static fix16 blockedByRight(Box_f16 target_box, const Level* level) {

	fix16 blocked = hitRight(target_box, *level->floor->object.box);
	if (blocked) {
		return blocked;
	}

	for (u8 i = 0; i < level->num_platforms; i++) {
		blocked = hitRight(target_box, *level->platforms[i]->object.box);
		if (blocked) {
			return blocked;
		}
	}

	return 0;
}

static void drawJetman(Jetman* jetman) {

	Sprite* sprite = jetman->sprite;

	if (jetman->object.mov.y) {
		// somewhere in the air
		SPR_setAnim(sprite, ANIM_FLY);

	} else {
		SPR_setAnim(sprite, ANIM_WALK);
		if (jetman->object.mov.x) {
			// walking
			jetman->walk_step_counter++;
			jetman->walk_step_counter %= STEPPING_SPEED;
			if (!jetman->walk_step_counter) {
				// controlling the animation speed
				SPR_nextFrame(sprite);
			}
		}
	}

	if (jetman->object.mov.x < 0) {
		SPR_setHFlip(sprite, TRUE);
	} else if (jetman->object.mov.x > 0) {
		SPR_setHFlip(sprite, FALSE);
	}
}

static void handleInputJetman(Jetman* jetman) {

	u16 value = JOY_readJoypad(JOY_1);

	if (value & BUTTON_B) {
		jetman->order.y = -1;
	} else {
		jetman->order.y = 0;
	}

	if (value & BUTTON_LEFT) {
		jetman->order.x = -1;
	} else if (value & BUTTON_RIGHT) {
		jetman->order.x = +1;
	} else {
		jetman->order.x = 0;
	}
}
