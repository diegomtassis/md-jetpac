/*
 * shooting.c
 *
 *  Created on: May 26, 2019
 *      Author: diegomtassis
 */

#include "../inc/shooting.h"

#define MAX_SHOTS 4

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

void shoot(V2s16 where, bool right, Level level[static 1]) {

}

void updateShots(Level level[static 1]) {

}

static void releaseShot(Shot* shot) {

	if (!shot) {
		return;
	}

	for (int idx = 0; idx < shot->grapes_count; idx++) {

		Object_f16* grape = shot->grapes[idx];
		if (grape) {
			MEM_free(grape);
			shot->grapes[idx] = 0;
		}

		Sprite* grape_sprite = shot->sprites[idx];
		if (grape_sprite) {
			SPR_releaseSprite(grape_sprite);
			shot->sprites[idx] = 0;
		}
	}

	shot->grapes_count = 0;
}
