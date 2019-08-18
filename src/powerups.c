/*
 * powerups.c
 *
 *  Created on: Aug 18, 2019
 *      Author: diegomtassis
 */

#include "../inc/powerups.h"

#include "../inc/fwk/commons.h"

#define MAX_POWERUPS 5

static void createPowerUp(Level level[static 1]);
static void releasePowerUp(PowerUp* power_up);

void startPowerUps(Level level[static 1]) {

	level->power_ups.e = MEM_calloc(MAX_POWERUPS * sizeof(PowerUp*));
	level->power_ups.size = MAX_POWERUPS;
	level->power_ups.count = 0;
}

void releasePowerUps(Level* level) {

	if (!level->power_ups.e) {
		return;
	}

	for (u8 idx = 0; idx < level->power_ups.size; idx++) {

		PowerUp* power_up = level->power_ups.e[idx];
		if (power_up) {
			releasePowerUp(power_up);
			level->power_ups.e[idx] = 0;
		}
	}

	level->power_ups.count = 0;
	level->power_ups.size = 0;
	MEM_free(level->power_ups.e);
	level->power_ups.e = 0;
}

void updatePowerUps(Level level[static 1]) {

	if (!level->power_ups.e) {
		return;
	}

	for (u8 idx = 0; idx < level->power_ups.size; idx++) {

		PowerUp* power_up = level->power_ups.e[idx];
		if (power_up) {
			// Do whatever has to be done
		}
	}

	// Create new power ups?
}

static void createPowerUp(Level level[static 1]) {

}

static void releasePowerUp(PowerUp* power_up) {

	if (power_up) {
		SPR_releaseSprite(power_up->sprite);
		MEM_free(power_up);
	}
}
