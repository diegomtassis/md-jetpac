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

#define FINISHED 3

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
}

void releaseAllExplosions(Level* level) {

	for (u8 idx = 0; idx < level->booms.max_num_booms; idx++) {

		Explosion* boom = level->booms.objects[idx];
		if (boom) {
			SPR_releaseSprite(boom->sprite);
			MEM_free(boom);
			level->booms.objects[idx] = NULL;
			level->booms.current_num_booms--;
		}
	}
}

void releaseFinishedExplosions(Level* level) {

	for (u8 idx = 0; idx < level->booms.max_num_booms; idx++) {

		Explosion* boom = level->booms.objects[idx];
		if (boom) {
			if (boom->step == FINISHED) {
				SPR_releaseSprite(boom->sprite);
				MEM_free(boom);
				level->booms.objects[idx] = NULL;
				level->booms.current_num_booms--;
			}
		}
	}
}

void explode(Level* level, Vect2D_f16 where) {

	// Find an empty slot
	u8 num_booms = level->booms.max_num_booms;
	u8 idx;
	while (num_booms--) {
		// find the first empty slot
		Explosion* boom = level->booms.objects[num_booms];
		if (!boom) {
			idx = num_booms;
			break;
		}
	}

	// Create the explosion
	Explosion* boom = MEM_alloc(sizeof(Explosion));
	level->booms.objects[idx] = boom;
	boom->step = 0;
	level->booms.current_num_booms++;
	boom->where.x = fix16ToInt(where.x);
	boom->where.y = fix16ToInt(where.y);

	// Create sprite
	Sprite* sprite = SPR_addSprite(&boom_sprite, boom->where.x, boom->where.y, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
	boom->sprite = sprite;
	SPR_setAnim(sprite, (abs(random())) % 4);
}
