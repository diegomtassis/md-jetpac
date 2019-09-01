/*
 * collectables.c
 *
 *  Created on: Aug 18, 2019
 *      Author: diegomtassis
 */

#include "../inc/collectables.h"

#include "../inc/fwk/commons.h"
#include "../inc/timers.h"

#define MAX_COLLECTABLES 5

static const int REST_BETWEEN_COLLECTABLES = SUBTICKPERSECOND * 1.5;

static void addCollectables(Level level[static 1]);
static void createCollectable(Level level[static 1], u8);
static void releaseCollectable(Collectable* collectable);

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
			// Do whatever has to be done
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

	level->collectables.e[idx] = 0;
	level->collectables.count++;

	startTimer(COLLECTABLE_CREATION_TIMER);
}

static void releaseCollectable(Collectable* collectable) {

	if (collectable) {
		SPR_releaseSprite(collectable->sprite);
		MEM_free(collectable);
	}
}
