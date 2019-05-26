/*
 * shooting.c
 *
 *  Created on: May 26, 2019
 *      Author: diegomtassis
 */

#include "../inc/shooting.h"

#include <genesis.h>

#include "../inc/fwk/commons.h"
#include "../inc/fwk/physics.h"
#include "../res/sprite.h"

#define MAX_SHOTS 4

#define RANGE_MEDIUM 200

#define SPEED_H_NORMAL	FIX16(3.5)

static void releaseShot(Shot* shot);

void initShots(Level level[static 1]) {

	level->shots.count = 0;
	level->shots.size = MAX_SHOTS;
	level->shots.e = MEM_alloc(level->shots.size * sizeof(Shot*));
	memset(level->shots.e, 0, level->shots.size * sizeof(Shot*));
}

void releaseShots(Level level[static 1]) {

	for (int idx = 0; idx < level->shots.size; idx++) {
		releaseShot(level->shots.e[idx]);
		level->shots.e[idx] = 0;
	}

	level->shots.count = 0;
	level->shots.size = 0;
	MEM_free(level->shots.e);
	level->shots.e = 0;
}

void shoot(V2s16 where, bool go_right, Level level[static 1]) {

	Shot* shot = MEM_alloc(sizeof *shot);
	memset(shot, 0, sizeof *shot);

	shot->goes_right = go_right;
	shot->grapes_size = 1;
	shot->grapes_count = 0;
	shot->grapes = MEM_alloc(shot->grapes_size * sizeof(Grape*));
	memset(shot->grapes, 0, shot->grapes_size);

	Grape* grape = 0;
	for (int idx = 0; idx < shot->grapes_size; idx++) {

		grape = MEM_alloc(sizeof(Grape));
		memset(grape, 0, sizeof(Grape));
		shot->grapes[idx] = grape;

		Object_f16* object = MEM_alloc(sizeof(Object_f16));
		memset(grape->object, 0, sizeof(Object_f16));
		grape->object = object;
		grape->object->pos.x = FIX16(where.x);
		grape->object->pos.y = FIX16(where.y);
		grape->object->mov.x = SPEED_H_NORMAL;
		grape->object->mov.y = FIX16_0;
		grape->object->size.x = 16;
		grape->object->size.y = 1;
		grape->object->box.w = 16;
		grape->object->box.h = 1;

		grape->sprite = SPR_addSprite(&shot_sprite, where.x, where.y, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));

		shot->grapes_count++;
	}
}

void updateShots(Level level[static 1]) {

	Shot* shot = 0;
	for (int idx = 0; idx < level->shots.size; idx++) {

		shot = level->shots.e[idx];
		if (shot) {

		}
	}
}

static void releaseShot(Shot* shot) {

	if (!shot) {
		return;
	}

	Grape* grape = 0;
	for (int idx = 0; idx < shot->grapes_size; idx++) {

		grape = shot->grapes[idx];
		if (grape) {
			SPR_releaseSprite(grape->sprite);
			grape->sprite = 0;
			MEM_free(grape->object);
			grape->object = 0;
			MEM_free(grape);
			shot->grapes[idx] = 0;
		}
	}

	shot->grapes_count = 0;
	shot->grapes_size = 0;
	MEM_free(shot);
}
