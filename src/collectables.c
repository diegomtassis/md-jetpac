/*
 * collectables.c
 *
 *  Created on: Aug 18, 2019
 *      Author: diegomtassis
 */

#include "../inc/collectables.h"

#include <maths.h>
#include <memory.h>
#include <sprite_eng.h>
#include <timer.h>
#include <types.h>

#include "../inc/events.h"
#include "../inc/fwk/commons.h"
#include "../inc/fwk/physics.h"
#include "../inc/items.h"
#include "../inc/level.h"
#include "../inc/timers.h"
#include "../res/sprite.h"

#define MAX_COLLECTABLES 5

typedef enum {
	GOLD, //
	BEAN, //
	NUKE, //
	DIAMOND, //
	TRIATOM, //
} CollectableType;

#define COLLECTABLE_FIRST GOLD
#define COLLECTABLE_LAST TRIATOM

typedef struct {
	CollectableType type;
	u16 width;
	u16 height;
	const SpriteDefinition* spriteDefinition;
} CollectableDefinition;

static const CollectableDefinition COLLECTABLE_DEFS[] = { //
		{ GOLD, 16, 8, &gold_sprite }, //
				{ BEAN, 16, 9, &bean_sprite }, //
				{ NUKE, 16, 11, &nuke_sprite }, //
				{ DIAMOND, 16, 12, &diamond_sprite }, //
				{ TRIATOM, 16, 13, &triatom_sprite } };

typedef enum {
	FALLING = 1, //
	WAITING = 2, //
	GRABBED = 4, //
	LOST = 8, //
} CollectableStep;

#define COLLECTABLE_MIN_POS_H_PX_S16	LEFT_POS_H_PX_S16
#define COLLECTABLE_MAX_POS_H_PX_S16	RIGHT_POS_H_PX_S16 - COLLECTABLE_WIDTH

#define COLLECTABLE_MIN_POS_H_PX_F16	FIX16(COLLECTABLE_MIN_POS_H_PX_S16)
#define COLLECTABLE_MAX_POS_H_PX_F16	FIX16(COLLECTABLE_MAX_POS_H_PX_S16)

static const int MIN_TIME_BETWEEN_COLLECTABLES = SUBTICKPERSECOND * 7;
static const int MAX_TIME_BETWEEN_COLLECTABLES = SUBTICKPERSECOND * 12;

static void addCollectables(Level level[static 1]);
static void createCollectable(Level level[static 1], u8);
static void updateCollectable(Collectable* collectable, Level level[static 1]);
static void releaseCollectable(Collectable* collectable);

extern u16 default_sprite_attrs;

void startCollectables(Level level[static 1]) {

	level->collectables.e = MEM_calloc(MAX_COLLECTABLES * sizeof(Collectable*));
	level->collectables.size = MAX_COLLECTABLES;
	level->collectables.count = 0;

	startCountDownRandom(COLLECTABLE_CREATION_COUNTDOWN, MIN_TIME_BETWEEN_COLLECTABLES, MAX_TIME_BETWEEN_COLLECTABLES);
}

void releaseCollectables(Level level[static 1]) {

	if (!level->collectables.e) {
		return;
	}

	for (u8 idx = 0; idx < level->collectables.size; idx++) {

		Collectable* collectable = level->collectables.e[idx];
		if (collectable) {
			releaseCollectable(collectable);
			level->collectables.e[idx] = 0;
		}
	}

	level->collectables.count = 0;
	level->collectables.size = 0;
	MEM_free(level->collectables.e);
	level->collectables.e = 0;
}

void updateCollectables(Level level[static 1]) {

	if (!level->collectables.e) {
		return;
	}

	for (u8 idx = 0; idx < level->collectables.size; idx++) {

		Collectable* collectable = level->collectables.e[idx];
		if (collectable) {

			updateCollectable(collectable, level);

			if (collectable->step == GRABBED || collectable->step == LOST) {

				releaseCollectable(collectable);
				level->collectables.e[idx] = 0;
				level->collectables.count--;
			}
		}
	}

	addCollectables(level);
}

static void addCollectables(Level level[static 1]) {

	if (level->collectables.count < level->collectables.size
			&& isCountDownFinished(COLLECTABLE_CREATION_COUNTDOWN, FALSE)) {

		u8 num_collectables = level->collectables.size;
		u8 idx;
		while (num_collectables--) {
			// find the first empty slot
			Collectable* collectable = level->collectables.e[num_collectables];
			if (!collectable) {
				idx = num_collectables;
				break;
			}
		}

		createCollectable(level, idx);
	}
}

static void createCollectable(Level level[static 1], u8 idx) {

	Collectable* collectable = MEM_calloc(sizeof *collectable);

	collectable->type = randomInRangeU16(COLLECTABLE_FIRST, COLLECTABLE_LAST);

	CollectableDefinition collectableDef = COLLECTABLE_DEFS[collectable->type];

	dropFromSky(&collectable->object, &level->spaceship->base_object->box, collectableDef.width, collectableDef.height,
	ITEM_DEFAULT_MIN_POS_H_PX_F16,
	ITEM_DEFAULT_MAX_POS_H_PX_F16);
	collectable->step = FALLING;

	collectable->sprite = SPR_addSprite(collectableDef.spriteDefinition, //
			fix16ToInt(collectable->object.pos.x), //
			fix16ToInt(collectable->object.pos.y), //
			default_sprite_attrs);

	level->collectables.e[idx] = collectable;
	level->collectables.count++;

	startCountDownRandom(COLLECTABLE_CREATION_COUNTDOWN, MIN_TIME_BETWEEN_COLLECTABLES, MAX_TIME_BETWEEN_COLLECTABLES);
}

static void updateCollectable(Collectable* collectable, Level level[static 1]) {

	if (!collectable) {
		return;
	}

	if (collectable->step == FALLING) {

		if (grab(&level->jetman->object, &collectable->object)) {
			// collectable grabbed while falling
			collectable->step = GRABBED;
			onEvent(GRABBED_COLLECTABLE);

		} else {
			Box_s16 target_v = targetVBox(collectable->object);
			if (landed(target_v, level)) {
				collectable->step = WAITING;
				collectable->object.mov.y = SPEED_0;

			} else if (level->def.mind_bottom && target_v.pos.y > BOTTOM_POS_V_PX_S16) {
				// collectable lost
				collectable->step = LOST;
				onEvent(LOST_COLLECTABLE);

			} else {
				collectable->object.pos.y += collectable->object.mov.y;
			}

			updateBox(&collectable->object);
			SPR_setPosition(collectable->sprite, fix16ToInt(collectable->object.pos.x),
					fix16ToInt(collectable->object.pos.y));
		}

	} else if (collectable->step == WAITING) {
		if (grab(&level->jetman->object, &collectable->object)) {
			// collectable grabbed while waiting
			collectable->step = GRABBED;
			onEvent(GRABBED_COLLECTABLE);
		}
	}
}

static void releaseCollectable(Collectable* collectable) {

	if (collectable) {
		SPR_releaseSprite(collectable->sprite);
		MEM_free(collectable);
	}
}
