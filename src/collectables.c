/*
 * collectables.c
 *
 *  Created on: Aug 18, 2019
 *      Author: diegomtassis
 */

#include "../inc/collectables.h"

#include <genesis.h>

#include "../inc/fwk/commons.h"
#include "../inc/items.h"
#include "../inc/timers.h"
#include "../inc/events.h"
#include "../inc/level.h"
#include "../res/sprite.h"

#define MAX_COLLECTABLES 5

#define FALLING		1
#define WAITING		2
#define GRABBED		3
#define LOST		4

static const int REST_BETWEEN_COLLECTABLES = SUBTICKPERSECOND * 8;

static void addCollectables(Level level[static 1]);
static void createCollectable(Level level[static 1], u8);
static void updateCollectable(Collectable* collectable, Level level[static 1]);
static void releaseCollectable(Collectable* collectable);

extern u16 default_sprite_attrs;

void startCollectables(Level level[static 1]) {

	level->collectables.e = MEM_calloc(MAX_COLLECTABLES * sizeof(Collectable*));
	level->collectables.size = MAX_COLLECTABLES;
	level->collectables.count = 0;

	startTimer(COLLECTABLE_CREATION_TIMER);
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
			&& getTimer(COLLECTABLE_CREATION_TIMER, FALSE) > REST_BETWEEN_COLLECTABLES) {

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
	collectable->type = GOLD;

	dropFromSky(&collectable->object, &level->spaceship->base_object->box);
	collectable->step = FALLING;

	collectable->sprite = SPR_addSprite(&gold_sprite, fix16ToInt(collectable->object.pos.x),
			fix16ToInt(collectable->object.pos.y), default_sprite_attrs);

	level->collectables.e[idx] = collectable;
	level->collectables.count++;

	startTimer(COLLECTABLE_CREATION_TIMER);
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
			Box_s16 target_v = targetVBox(collectable->object, COLLECTABLE_WIDTH, COLLECTABLE_HEIGHT);
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
