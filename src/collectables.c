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

#include "../inc/players.h"
#include "../inc/events.h"
#include "../inc/fwk/commons.h"
#include "../inc/fwk/physics.h"
#include "../inc/items.h"
#include "../inc/planet.h"
#include "../inc/timers.h"
#include "../res/sprite.h"

#define MAX_COLLECTABLES 5
#define BLINK_TIME 15

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
	bool blinks;
	const SpriteDefinition* spriteDefinition;
} CollectableDefinition;

static const CollectableDefinition COLLECTABLE_DEFS[] = { //
		{ GOLD, 16, 8, FALSE, &gold_sprite }, //
				{ BEAN, 16, 9, FALSE, &bean_sprite }, //
				{ NUKE, 16, 11, TRUE, &nuke_sprite }, //
				{ DIAMOND, 16, 12, FALSE, &diamond_sprite }, //
				{ TRIATOM, 16, 13, TRUE, &triatom_sprite } };

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

static void addCollectables(Planet planet[static 1]);
static void createCollectable(Planet planet[static 1], u8);
static void updateCollectable(Collectable* collectable, Planet planet[static 1]);
static void releaseCollectable(Collectable* collectable);

extern u16 default_sprite_attrs;

void startCollectables(Planet planet[static 1]) {

	planet->collectables.e = MEM_calloc(MAX_COLLECTABLES * sizeof(Collectable*));
	planet->collectables.size = MAX_COLLECTABLES;
	planet->collectables.count = 0;

	startCountDownRandom(COLLECTABLE_CREATION_COUNTDOWN, MIN_TIME_BETWEEN_COLLECTABLES, MAX_TIME_BETWEEN_COLLECTABLES);
}

void releaseCollectables(Planet planet[static 1]) {

	if (!planet->collectables.e) {
		return;
	}

	for (u8 idx = 0; idx < planet->collectables.size; idx++) {

		Collectable* collectable = planet->collectables.e[idx];
		if (collectable) {
			releaseCollectable(collectable);
			planet->collectables.e[idx] = 0;
		}
	}

	planet->collectables.count = 0;
	planet->collectables.size = 0;
	MEM_free(planet->collectables.e);
	planet->collectables.e = 0;
}

void updateCollectables(Planet planet[static 1]) {

	if (!planet->collectables.e) {
		return;
	}

	for (u8 idx = 0; idx < planet->collectables.size; idx++) {

		Collectable* collectable = planet->collectables.e[idx];
		if (collectable) {

			updateCollectable(collectable, planet);

			if (collectable->step == GRABBED || collectable->step == LOST) {

				releaseCollectable(collectable);
				planet->collectables.e[idx] = 0;
				planet->collectables.count--;
			}
		}
	}

	addCollectables(planet);
}

static void addCollectables(Planet planet[static 1]) {

	if (planet->collectables.count < planet->collectables.size
			&& isCountDownFinished(COLLECTABLE_CREATION_COUNTDOWN, FALSE)) {

		u8 num_collectables = planet->collectables.size;
		u8 idx;
		while (num_collectables--) {
			// find the first empty slot
			Collectable* collectable = planet->collectables.e[num_collectables];
			if (!collectable) {
				idx = num_collectables;
				break;
			}
		}

		createCollectable(planet, idx);
	}
}

static void createCollectable(Planet planet[static 1], u8 idx) {

	Collectable* collectable = MEM_calloc(sizeof *collectable);

	collectable->type = randomInRangeU16(COLLECTABLE_FIRST, COLLECTABLE_LAST);

	CollectableDefinition collectableDef = COLLECTABLE_DEFS[collectable->type];

	dropFromSky(&collectable->object, &planet->spaceship->base_object->box, collectableDef.width, collectableDef.height,
	ITEM_DEFAULT_MIN_POS_H_PX_F16,
	ITEM_DEFAULT_MAX_POS_H_PX_F16);
	collectable->step = FALLING;

	collectable->sprite = SPR_addSprite(collectableDef.spriteDefinition, //
			fix16ToInt(collectable->object.pos.x), //
			fix16ToInt(collectable->object.pos.y), //
			default_sprite_attrs);

	if (collectableDef.blinks) {
		collectable->blinker = MEM_calloc(sizeof(*collectable->blinker));
		collectable->blinker->init_value = BLINK_TIME;
		collectable->blinker->counter = BLINK_TIME;
		collectable->blinker->visible = TRUE;
	}

	planet->collectables.e[idx] = collectable;
	planet->collectables.count++;

	startCountDownRandom(COLLECTABLE_CREATION_COUNTDOWN, MIN_TIME_BETWEEN_COLLECTABLES, MAX_TIME_BETWEEN_COLLECTABLES);
}

static void updateCollectable(Collectable* collectable, Planet planet[static 1]) {

	if (!collectable) {
		return;
	}

	if (collectable->step == FALLING) {

		if (grab(&planet->j1->object, &collectable->object)) {
			// collectable grabbed by p1 while falling
			collectable->step = GRABBED;
			onEvent(GRABBED_COLLECTABLE, P1);

		} else if (planet->j2 && (planet->j2->health & ALIVE) && grab(&planet->j2->object, &collectable->object)) {
			// collectable grabbed by p2 while falling
			collectable->step = GRABBED;
			onEvent(GRABBED_COLLECTABLE, P2);

		} else {
			Box_s16 target_v = targetVBox(collectable->object);
			if (landed(target_v, planet)) {
				collectable->step = WAITING;
				collectable->object.mov.y = SPEED_0;

			} else if (planet->def.mind_bottom && target_v.pos.y > BOTTOM_POS_V_PX_S16) {
				// collectable lost
				collectable->step = LOST;
				onEvent(LOST_COLLECTABLE, 0);

			} else {
				collectable->object.pos.y += collectable->object.mov.y;
			}

			updateBox(&collectable->object);
			SPR_setPosition(collectable->sprite, fix16ToInt(collectable->object.pos.x),
					fix16ToInt(collectable->object.pos.y));
		}

	} else if (collectable->step == WAITING) {
		if (grab(&planet->j1->object, &collectable->object)) {
			// collectable grabbed by p1 while waiting
			collectable->step = GRABBED;
			onEvent(GRABBED_COLLECTABLE, P1);

		} else if (planet->j2 && (planet->j2->health & ALIVE) && grab(&planet->j2->object, &collectable->object)) {
			// collectable grabbed by p2 while waiting
			collectable->step = GRABBED;
			onEvent(GRABBED_COLLECTABLE, P2);
		}
	}

	if (collectable->blinker) {
		blink(collectable->blinker, collectable->sprite);
	}
}

static void releaseCollectable(Collectable* collectable) {

	if (collectable) {
		if (collectable->blinker) {
			MEM_free(collectable->blinker);
		}

		SPR_releaseSprite(collectable->sprite);
		MEM_free(collectable);
	}
}
