/*
 * explosions.c
 *
 *  Created on: May 4, 2019
 *      Author: diegomtassis
 */

#include "../inc/explosions.h"

#include <genesis.h>

#include "../inc/fwk/commons.h"
#include "../res/sprite.h"
#include "../inc/timers.h"

#define BOOM_H_PX 16

#define FINISHED 3

#define BOOST 0

static void boom(Box_s16 what, Planet planet[static 1], u8 style);
static void releaseFinishedExplosions(Planet planet[static 1]);

void initExplosions(Planet planet[static 1]) {

	planet->booms.count = 0;
	planet->booms.size = 1 + planet->enemies.size;
	planet->booms.e = MEM_calloc(sizeof(Explosion*) * planet->booms.size);

	startTimer(EXPLOSIONS_TIMER);
}

void updateExplosions(Planet planet[static 1]) {

	if (getTimer(EXPLOSIONS_TIMER, FALSE) > BOOM_ANIMATION_SPEED) {

		for (u8 idx = 0; idx < planet->booms.size; idx++) {

			Explosion* boom = planet->booms.e[idx];
			if (boom) {
				if (boom->step < FINISHED) {
					SPR_setFrame(boom->sprite, ++boom->step);
				}
			}
		}

		startTimer(EXPLOSIONS_TIMER);
	}

	releaseFinishedExplosions(planet);
}

void releaseExplosions(Planet planet[static 1]) {

	if (!planet->booms.e) {
		return;
	}

	for (u8 idx = 0; idx < planet->booms.size; idx++) {

		Explosion* boom = planet->booms.e[idx];
		if (boom) {
			SPR_releaseSprite(boom->sprite);
			MEM_free(boom);
			planet->booms.e[idx] = 0;
		}
	}

	MEM_free(planet->booms.e);
	planet->booms.e = 0;

	planet->booms.count = 0;
}

static void releaseFinishedExplosions(Planet planet[static 1]) {

	for (u8 idx = 0; idx < planet->booms.size; idx++) {

		Explosion* boom = planet->booms.e[idx];
		if (boom && boom->step == FINISHED) {
			SPR_releaseSprite(boom->sprite);
			MEM_free(boom);
			planet->booms.e[idx] = 0;
			planet->booms.count--;
		}
	}
}

void explode(Box_s16 what, Planet planet[static 1]) {

	boom(what, planet, abs(random()) % 3 + 1);
}

void boost(Box_s16 what, Planet planet[static 1]) {

	boom(what, planet, BOOST);
}

static void boom(Box_s16 what, Planet planet[static 1], u8 style) {

	// Find an empty slot
	u8 num_booms = planet->booms.size;
	u8 boom_idx;
	while (num_booms--) {
		// find the first empty slot
		Explosion* boom = planet->booms.e[num_booms];
		if (!boom) {
			boom_idx = num_booms;
			break;
		}
	}

	// Create the explosion
	Explosion* boom = MEM_calloc(sizeof *boom);
	planet->booms.e[boom_idx] = boom;
	boom->step = 0;
	planet->booms.count++;
	boom->where.x = what.min.x;
	boom->where.y = what.max.y - BOOM_H_PX;

	// Create sprite
	Sprite* sprite = SPR_addSprite(&boom_sprite, boom->where.x, boom->where.y, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
	boom->sprite = sprite;
	SPR_setAnim(sprite, style);
}
