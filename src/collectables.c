/*
 * collectables.c
 *
 *  Created on: Aug 18, 2019
 *      Author: diegomtassis
 */

#include "../inc/collectables.h"

#include "../inc/fwk/commons.h"

#define MAX_COLLECTABLES 5

static void createCollectable(Level level[static 1]);
static void releaseCollectable(Collectable* power_up);

void startCollectables(Level level[static 1]) {

	level->collectables.e = MEM_calloc(MAX_COLLECTABLES * sizeof(Collectable*));
	level->collectables.size = MAX_COLLECTABLES;
	level->collectables.count = 0;
}

void releaseCollectables(Level* level) {

	if (!level->collectables.e) {
		return;
	}

	for (u8 idx = 0; idx < level->collectables.size; idx++) {

		Collectable* power_up = level->collectables.e[idx];
		if (power_up) {
			releaseCollectable(power_up);
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

		Collectable* power_up = level->collectables.e[idx];
		if (power_up) {
			// Do whatever has to be done
		}
	}

	// Create new power ups?
}

static void createCollectable(Level level[static 1]) {

}

static void releaseCollectable(Collectable* power_up) {

	if (power_up) {
		SPR_releaseSprite(power_up->sprite);
		MEM_free(power_up);
	}
}
