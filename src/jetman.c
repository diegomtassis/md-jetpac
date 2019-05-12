/*
 * jetman.c
 *
 *  Created on: Apr 16, 2019
 *      Author: diegomtassis
 */

#include "../inc/jetman.h"

#include <genesis.h>

#include "../inc/commons.h"
#include "../inc/physics.h"
#include "../inc/explosions.h"
#include "../res/sprite.h"

#define ANIM_WALK		0
#define ANIM_FLY		1
#define STEPPING_SPEED	6 // 0 Maximum

#define SPEED_ZERO		FIX16_0
#define SPEED_H_NORMAL	FIX16(1.5)
#define SPEED_V_UP		FIX16(-1.5)
#define SPEED_V_DOWN	FIX16(1.5)

#define UP			0x01
#define DOWN		0x02
#define RIGHT		0x04
#define LEFT		0x10
#define BOOST		0x20

#define JETMAN_HEIGHT 24
#define JETMAN_WIDTH 16

#define MIN_POS_H_PX_S16	LEFT_POS_H_PX_S16 - 8
#define MAX_POS_H_PX_S16	RIGHT_POS_H_PX_S16 - 8
#define MIN_POS_V_PX_S16	TOP_POS_V_PX_S16

#define MIN_POS_H_PX_F16	FIX16(MIN_POS_H_PX_S16)
#define MAX_POS_H_PX_F16	FIX16(MAX_POS_H_PX_S16)
#define MIN_POS_V_PX_F16	FIX16(MIN_POS_V_PX_S16)

static void createPlayer1(Level*);
static void handleInputJetman(Jetman*);

static void moveToStart(Jetman* jetman, const Level* level);
static void moveJetman(Jetman*, Level*);
static u8 calculateNextMovement(Jetman*);
static void updatePosition(Jetman*, Level*);
static fix16 landed(Box_s16, const Level*);
static fix16 reachedTop(Box_s16, const Level*);
static fix16 blockedByLeft(Box_s16, const Level*);
static fix16 blockedByRight(Box_s16, const Level*);

static void drawJetman(Jetman*);

void startJetman(Level* level) {

	createPlayer1(level);
	level->jetman->sprite = SPR_addSprite(&jetman_sprite, fix16ToInt(level->jetman->object.pos.x),
			fix16ToInt(level->jetman->object.pos.y), TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
}

void releaseJetman(Jetman* jetman) {

	SPR_setVisibility(jetman->sprite, FALSE);
	SPR_releaseSprite(jetman->sprite);
	jetman->sprite = 0;
	MEM_free(jetman);
}

void resetJetman(Level* level) {

	moveToStart(level->jetman, level);
	level->jetman->health = ALIVE;
}

void killJetman(Level* level, u8 exploding) {

	if (exploding) {
		explode(level->jetman->object.box, level);
	}

	level->jetman->health = DEAD;
}

void jetmanActs(Level* level) {

	Jetman* jetman = level->jetman;
	if (jetman->health & ALIVE) {
		handleInputJetman(jetman);
		moveJetman(level->jetman, level);
		drawJetman(level->jetman);
	}
}

static void createPlayer1(Level* level) {

	Jetman* jetman = MEM_alloc(sizeof(Jetman));

	jetman->object.size.x = JETMAN_WIDTH;
	jetman->object.size.y = JETMAN_HEIGHT;

	jetman->object.box.w = JETMAN_WIDTH;
	jetman->object.box.h = JETMAN_HEIGHT;

	moveToStart(jetman, level);
	jetman->health = ALIVE;

	level->jetman = jetman;
}

static void moveToStart(Jetman* jetman, const Level* level) {

	if (level->def.jetman_init_pos) {
		jetman->object.pos.x = FIX16(level->def.jetman_init_pos->x);
		jetman->object.pos.y = FIX16(level->def.jetman_init_pos->y - 8 * 3);
	} else {
		jetman->object.pos.x = FIX16(124);
		jetman->object.pos.y = fix16Sub(level->floor->object.pos.y, FIX16(8*3));
	}

	jetman->object.mov.x = SPEED_ZERO;
	jetman->object.mov.y = SPEED_ZERO;

	updateBox(&jetman->object);
}

static void moveJetman(Jetman* jetman, Level* level) {

	if (BOOST & calculateNextMovement(jetman)) {
		boost(jetman->object.box, level);
	}
	updatePosition(jetman, level);
	SPR_setPosition(jetman->sprite, fix16ToInt(jetman->object.pos.x), fix16ToInt(jetman->object.pos.y));
}

static u8 calculateNextMovement(Jetman* jetman) {

	u8 movement = 0;

	// horizontal movement
	if (jetman->order.x > 0) {
		jetman->object.mov.x = SPEED_H_NORMAL;
		movement |= RIGHT;

	} else if (jetman->order.x < 0) {
		jetman->object.mov.x = -SPEED_H_NORMAL;
		movement |= LEFT;

	} else {
		jetman->object.mov.x = SPEED_ZERO;
	}

	// vertical movement
	if (jetman->order.y < 0) {
		if (jetman->object.mov.y == FIX16_0) {
			movement |= BOOST;
		}

		jetman->object.mov.y = SPEED_V_UP;
		movement |= UP;

	} else {
		jetman->object.mov.y = SPEED_V_DOWN;
		movement |= DOWN;
	}

	return movement;
}

static void updatePosition(Jetman* jetman, Level* level) {

	// horizontal position
	Box_s16 target_h = targetHBox(jetman->object, JETMAN_WIDTH, JETMAN_HEIGHT);
	if (target_h.pos.x > MAX_POS_H_PX_S16) {
		jetman->object.pos.x = MIN_POS_H_PX_F16;

	} else if (target_h.pos.x < MIN_POS_H_PX_S16) {
		jetman->object.pos.x = MAX_POS_H_PX_F16;

	} else {

		fix16 blockedHorizontally = blockedByLeft(target_h, level);
		if (!blockedHorizontally) {
			blockedHorizontally = blockedByRight(target_h, level);
		}

		if (blockedHorizontally) {
			jetman->object.pos.x = blockedHorizontally;

		} else {
			jetman->object.pos.x += jetman->object.mov.x;
		}
	}

	// vertical position
	Box_s16 target_v = targetVBox(jetman->object, JETMAN_WIDTH, JETMAN_HEIGHT);
	fix16 landed_pos_y = landed(target_v, level);
	if (landed_pos_y) {
		jetman->object.pos.y = landed_pos_y;
		jetman->object.mov.y = SPEED_ZERO;

	} else {
		fix16 top_pos_y = reachedTop(target_v, level);
		if (top_pos_y) {
			jetman->object.pos.y = top_pos_y;

		} else {
			jetman->object.pos.y += jetman->object.mov.y;
		}
	}

	// update box
	updateBox(&jetman->object);
}

static fix16 landed(Box_s16 subject_box, const Level* level) {

	if (hitAbove(subject_box, level->floor->object.box)) {
		return FIX16(adjacentYAbove(subject_box, level->floor->object.box));
	}

	for (u8 i = 0; i < level->num_platforms; i++) {
		Box_s16 object_box = level->platforms[i]->object.box;
		if (hitAbove(subject_box, object_box)) {
			return FIX16(adjacentYAbove(subject_box, object_box));
		}
	}

	return FIX16_0;
}

static fix16 reachedTop(Box_s16 subject_box, const Level* level) {

	if (subject_box.pos.y <= MIN_POS_V_PX_S16) {
		return MIN_POS_V_PX_F16;
	}

	for (u8 i = 0; i < level->num_platforms; i++) {
		Box_s16 object_box = level->platforms[i]->object.box;
		if (hitUnder(subject_box, object_box)) {
			return FIX16(adjacentYUnder(subject_box, object_box));
		}
	}

	if (hitUnder(subject_box, level->floor->object.box)) {
		return FIX16(adjacentYUnder(subject_box, level->floor->object.box));
	}

	return FIX16_0;
}

static fix16 blockedByLeft(Box_s16 target_box, const Level* level) {

	if (hitLeft(target_box, level->floor->object.box)) {
		return FIX16(adjacentXOnTheLeft(target_box, level->floor->object.box));
	}

	for (u8 i = 0; i < level->num_platforms; i++) {
		Box_s16 object_box = level->platforms[i]->object.box;
		if (hitLeft(target_box, object_box)) {
			return FIX16(adjacentXOnTheLeft(target_box, object_box));
		}
	}

	return FIX16_0;
}

static fix16 blockedByRight(Box_s16 target_box, const Level* level) {

	if (hitRight(target_box, level->floor->object.box)) {
		return FIX16(adjacentXOnTheRight(target_box, level->floor->object.box));
	}

	for (u8 i = 0; i < level->num_platforms; i++) {
		Box_s16 object_box = level->platforms[i]->object.box;
		if (hitRight(target_box, object_box)) {
			return FIX16(adjacentXOnTheRight(target_box, object_box));
		}
	}

	return FIX16_0;
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
