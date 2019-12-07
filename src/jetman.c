/*
 * jetman.c
 *
 *  Created on: Apr 16, 2019
 *      Author: diegomtassis
 */

#include "../inc/jetman.h"

#include <genesis.h>

#include "../inc/constants.h"
#include "../inc/explosions.h"
#include "../inc/fwk/commons.h"
#include "../inc/fwk/physics.h"
#include "../inc/planet.h"
#include "../inc/shooting.h"
#include "../res/sprite.h"

#define ANIM_WALK		0
#define ANIM_FLY		1
#define STEPPING_SPEED	6 // 0 Maximum

#define SPEED_ZERO		FIX16_0
#define SPEED_H_WALK	FIX16(1)
#define SPEED_H_FLY		FIX16(1.5)
#define SPEED_V_UP		FIX16(-1.5)
#define SPEED_V_DOWN	FIX16(1.5)

#define UP			0x01
#define DOWN		0x02
#define RIGHT		0x04
#define LEFT		0x10
#define BOOST		0x20

#define JETMAN_HEIGHT 24
#define JETMAN_WIDTH 16

static void createPlayer1(Planet planet[static 1]);
static void handleInputJetman(Jetman*);

static void moveToStart(Jetman* jetman, const Planet planet[static 1]);
static void moveJetman(Jetman*, Planet*);
static u8 calculateNextMovement(Jetman*);
static void updatePosition(Jetman*, Planet*);
static f16 reachedTop(Box_s16, const Planet*);
static f16 blockedByLeft(Box_s16, const Planet*);
static f16 blockedByRight(Box_s16, const Planet*);

static void drawJetman(Jetman*);

bool shoot_pushed;
bool shoot_order;

void startJetman(Planet planet[static 1], bool limit_ammo) {

	createPlayer1(planet);
	planet->jetman->sprite = SPR_addSprite(&jetman_sprite, fix16ToInt(planet->jetman->object.pos.x),
			fix16ToInt(planet->jetman->object.pos.y), TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
	shoot_pushed = FALSE;
	shoot_order = FALSE;
	planet->jetman->limited_ammo = limit_ammo;
	planet->jetman->ammo = planet->def.ammo;
}

void releaseJetman(Planet planet[static 1]) {

	Jetman* jetman = planet->jetman;
	if (!jetman) {
		return;
	}

	SPR_setVisibility(jetman->sprite, FALSE);
	SPR_releaseSprite(jetman->sprite);
	jetman->sprite = 0;
	MEM_free(jetman);
	planet->jetman = 0;
}

void resetJetman(Planet planet[static 1]) {

	moveToStart(planet->jetman, planet);
	planet->jetman->health = ALIVE;
}

void killJetman(Planet planet[static 1], bool exploding) {

	if (exploding) {
		explode(planet->jetman->object.box, planet);
	}

	planet->jetman->health = DEAD;
}

void jetmanActs(Planet planet[static 1]) {

	Jetman* jetman = planet->jetman;
	if (jetman->health & ALIVE) {
		handleInputJetman(jetman);
		moveJetman(planet->jetman, planet);
		drawJetman(planet->jetman);

		if (shoot_order && (!jetman->limited_ammo || jetman->ammo)) {

			V2s16 where = { 0 };
			where.x = planet->jetman->object.box.pos.x + (planet->jetman->head_back ? 0 : 16);
			where.y = planet->jetman->object.box.pos.y + 11;

			shoot(where, planet->jetman->head_back, planet);
			jetman->ammo--;
			shoot_order = FALSE;
		}
	}
}

static void createPlayer1(Planet planet[static 1]) {

	Jetman* jetman = MEM_calloc(sizeof *jetman);

	jetman->object.size.x = JETMAN_WIDTH;
	jetman->object.size.y = JETMAN_HEIGHT;

	jetman->object.box.w = JETMAN_WIDTH;
	jetman->object.box.h = JETMAN_HEIGHT;

	moveToStart(jetman, planet);
	jetman->health = ALIVE;

	planet->jetman = jetman;
}

static void moveToStart(Jetman* jetman, const Planet planet[static 1]) {

	if (planet->def.jetman_init_pos) {
		jetman->object.pos.x = FIX16(planet->def.jetman_init_pos->x);
		jetman->object.pos.y = FIX16(planet->def.jetman_init_pos->y - 8 * 3);
	} else {
		jetman->object.pos.x = FIX16(124);
		jetman->object.pos.y = fix16Sub(planet->floor->object.pos.y, FIX16(8*3));
	}

	jetman->object.mov.x = SPEED_ZERO;
	jetman->object.mov.y = SPEED_ZERO;

	updateBox(&jetman->object);
}

static void moveJetman(Jetman* jetman, Planet planet[static 1]) {

	if (BOOST & calculateNextMovement(jetman)) {
		boost(jetman->object.box, planet);
	}
	updatePosition(jetman, planet);
	SPR_setPosition(jetman->sprite, fix16ToInt(jetman->object.pos.x), fix16ToInt(jetman->object.pos.y));
}

static u8 calculateNextMovement(Jetman* jetman) {

	u8 movement = 0;

	// horizontal movement
	if (jetman->order.x > 0) {
		jetman->object.mov.x = jetman->airborne ? SPEED_H_FLY : SPEED_H_WALK;
		jetman->head_back = FALSE;
		movement |= RIGHT;

	} else if (jetman->order.x < 0) {
		jetman->object.mov.x = jetman->airborne ? -SPEED_H_FLY : -SPEED_H_WALK;
		jetman->head_back = TRUE;
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

static void updatePosition(Jetman* jetman, Planet planet[static 1]) {

	// horizontal position
	Box_s16 target_h = targetHBox(jetman->object);
	if (target_h.pos.x > MAX_POS_H_PX_S16) {
		jetman->object.pos.x = MIN_POS_H_PX_F16;

	} else if (target_h.pos.x < MIN_POS_H_PX_S16) {
		jetman->object.pos.x = MAX_POS_H_PX_F16;

	} else {

		f16 blockedHorizontally = blockedByLeft(target_h, planet);
		if (!blockedHorizontally) {
			blockedHorizontally = blockedByRight(target_h, planet);
		}

		if (blockedHorizontally) {
			jetman->object.pos.x = blockedHorizontally;

		} else {
			jetman->object.pos.x += jetman->object.mov.x;
		}
	}

	// vertical position
	Box_s16 target_v = targetVBox(jetman->object);
	f16 landed_pos_y = landed(target_v, planet);
	if (landed_pos_y) {
		jetman->object.pos.y = landed_pos_y;
		jetman->object.mov.y = SPEED_ZERO;
		jetman->airborne = FALSE;

	} else {
		f16 top_pos_y = reachedTop(target_v, planet);
		if (top_pos_y) {
			jetman->object.pos.y = top_pos_y;

		} else {
			jetman->object.pos.y += jetman->object.mov.y;
		}
		jetman->airborne = TRUE;
	}

	// update box
	updateBox(&jetman->object);
}

static f16 reachedTop(Box_s16 subject_box, const Planet planet[static 1]) {

	if (subject_box.pos.y <= MIN_POS_V_PX_S16) {
		return MIN_POS_V_PX_F16;
	}

	for (u8 i = 0; i < planet->num_platforms; i++) {
		Box_s16 object_box = planet->platforms[i]->object.box;
		if (hitUnder(subject_box, object_box)) {
			return FIX16(adjacentYUnder(subject_box, object_box));
		}
	}

	if (hitUnder(subject_box, planet->floor->object.box)) {
		return FIX16(adjacentYUnder(subject_box, planet->floor->object.box));
	}

	return FIX16_0;
}

static f16 blockedByLeft(Box_s16 target_box, const Planet planet[static 1]) {

	if (hitLeft(target_box, planet->floor->object.box)) {
		return FIX16(adjacentXOnTheLeft(target_box, planet->floor->object.box));
	}

	for (u8 i = 0; i < planet->num_platforms; i++) {
		Box_s16 object_box = planet->platforms[i]->object.box;
		if (hitLeft(target_box, object_box)) {
			return FIX16(adjacentXOnTheLeft(target_box, object_box));
		}
	}

	return FIX16_0;
}

static f16 blockedByRight(Box_s16 target_box, const Planet planet[static 1]) {

	if (hitRight(target_box, planet->floor->object.box)) {
		return FIX16(adjacentXOnTheRight(target_box, planet->floor->object.box));
	}

	for (u8 i = 0; i < planet->num_platforms; i++) {
		Box_s16 object_box = planet->platforms[i]->object.box;
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

	SPR_setHFlip(sprite, jetman->head_back);
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

	if (value & BUTTON_C) {
		if (!shoot_pushed) {
			// detect flank
			shoot_order = TRUE;
		}
		shoot_pushed = TRUE;
	} else {
		shoot_pushed = FALSE;
	}
}
