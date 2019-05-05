/*
 * explosions.c
 *
 *  Created on: May 4, 2019
 *      Author: diegomtassis
 */

#include "../inc/explosions.h"

#include <genesis.h>

#include "../res/sprite.h"
#include "../inc/timers.h"

#define BOOM_H_PX 16

#define FINISHED 3

#define BOOST 0

static void boom(Box_s16 what, Level* level, u8 style);
static void releaseFinishedExplosions(Level*);

void initExplosions(Level* level) {

	level->booms.current_num_booms = 0;
	level->booms.max_num_booms = 1 + level->enemies.max_num_enemies;
	level->booms.objects = MEM_alloc(sizeof(Explosion*) * level->booms.max_num_booms);
	for (u8 idx = 0; idx < level->booms.max_num_booms; idx++) {
		level->booms.objects[idx] = NULL;
	}
}

void updateExplosions(Level* level) {

	if (getTimer(EXPLOSIONS_TIMER, FALSE) > BOOM_ANIMATION_SPEED) {

		u8 num_booms = level->booms.current_num_booms;
		u8 current_boom = 0;
		u8 idx = 0;

		while (current_boom < num_booms) {

			Explosion* boom = level->booms.objects[idx++];
			if (boom) {
				if (boom->step < FINISHED) {
					SPR_setFrame(boom->sprite, ++boom->step);
				}
				current_boom++;
			}
		}

		startTimer(EXPLOSIONS_TIMER);
	}

	releaseFinishedExplosions(level);
}

static void releaseFinishedExplosions(Level* level) {

	for (u8 idx = 0; idx < level->booms.max_num_booms; idx++) {

		Explosion* boom = level->booms.objects[idx];
		if (boom && boom->step == FINISHED) {
			SPR_releaseSprite(boom->sprite);
			MEM_free(boom);
			level->booms.objects[idx] = NULL;
			level->booms.current_num_booms--;
		}
	}
}

void explode(Box_s16 what, Level* level) {

	boom(what, level, abs(random()) % 3 + 1);
}

void boost(Box_s16 what, Level* level) {

	boom(what, level, BOOST);
}

static void boom(Box_s16 what, Level* level, u8 style) {

	// Find an empty slot
	u8 num_booms = level->booms.max_num_booms;
	u8 boom_idx;
	while (num_booms--) {
		// find the first empty slot
		Explosion* boom = level->booms.objects[num_booms];
		if (!boom) {
			boom_idx = num_booms;
			break;
		}
	}

	// Create the explosion
	Explosion* boom = MEM_alloc(sizeof(Explosion));
	level->booms.objects[boom_idx] = boom;
	boom->step = 0;
	level->booms.current_num_booms++;
	boom->where.x = what.pos.x;
	boom->where.y = what.pos.y + what.h - BOOM_H_PX;

	// Create sprite
	Sprite* sprite = SPR_addSprite(&boom_sprite, boom->where.x, boom->where.y, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
	boom->sprite = sprite;
	SPR_setAnim(sprite, style);
}
