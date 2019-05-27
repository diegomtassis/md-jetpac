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

#define MAX_SHOTS 	5

#define RANGE_MEDIUM 55

#define SPEED_H_NORMAL	FIX16(3.1)

#define GRAPE_WIDTH 16
#define GRAPE_HEIGHT 1

#define MIN_POS_H_PX_S16	LEFT_POS_H_PX_S16 - 8
#define MAX_POS_H_PX_S16	RIGHT_POS_H_PX_S16 - 8

#define MIN_POS_H_PX_F16	FIX16(MIN_POS_H_PX_S16)
#define MAX_POS_H_PX_F16	FIX16(MAX_POS_H_PX_S16)

static void releaseShot(Shot* shot);
static void releaseGrape(Grape* grape);

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

void shoot(V2s16 where, bool to_left, Level level[static 1]) {

	if (level->shots.count >= level->shots.size) {
		return;
	}

	Shot* shot = MEM_alloc(sizeof *shot);
	memset(shot, 0, sizeof *shot);

	for (int idx = 0; idx < level->shots.size; idx++) {
		// Find an empty spot for the shot
		if (!level->shots.e[idx]) {
			level->shots.e[idx] = shot;
			level->shots.count++;
			break;
		}
	}

	shot->to_left = to_left;
	shot->grapes_size = 1;

	shot->grapes_count = 0;
	shot->grapes = MEM_alloc(shot->grapes_size * sizeof(Grape*));
	memset(shot->grapes, 0, shot->grapes_size);

	Grape* grape = 0;
	grape = MEM_alloc(sizeof(Grape));
	memset(grape, 0, sizeof(Grape));
	shot->grapes[0] = grape;

	Object_f16* object = MEM_alloc(sizeof(Object_f16));
	memset(grape->object, 0, sizeof(Object_f16));
	grape->object = object;
	grape->object->pos.x = FIX16(where.x - (to_left ? 16 : 0));
	grape->object->pos.y = FIX16(where.y);
	grape->object->mov.x = to_left ? -SPEED_H_NORMAL : SPEED_H_NORMAL;
	grape->object->mov.y = FIX16_0;
	grape->object->size.x = 16;
	grape->object->size.y = 1;
	grape->object->box.w = 16;
	grape->object->box.h = 1;
	updateBox(grape->object);

	grape->life_left = RANGE_MEDIUM;

	grape->sprite = SPR_addSprite(&shot_sprite, grape->object->box.pos.x, grape->object->box.pos.y,
			TILE_ATTR(PAL0, TRUE, FALSE, to_left));
	SPR_setAnim(grape->sprite, abs(random()) % 3);

	shot->grapes_count++;
}

void updateShots(Level level[static 1]) {

	Shot* shot = 0;
	for (int idx_shot = 0; idx_shot < level->shots.size; idx_shot++) {

		shot = level->shots.e[idx_shot];
		if (shot) {

			Grape* grape = 0;
			for (int idx_grape = 0; idx_grape < shot->grapes_size; ++idx_grape) {

				grape = shot->grapes[idx_grape];
				if (grape) {

					grape->life_left--;
					if (grape->life_left) {
						// move
						Box_s16 target_h = targetHBox(*grape->object, GRAPE_WIDTH, GRAPE_HEIGHT);
						if (target_h.pos.x > MAX_POS_H_PX_S16) {
							grape->object->pos.x = MIN_POS_H_PX_F16;

						} else if (target_h.pos.x < MIN_POS_H_PX_S16) {
							grape->object->pos.x = MAX_POS_H_PX_F16;

						} else {
							grape->object->pos.x += grape->object->mov.x;
						}

						updateBox(grape->object);
						SPR_setPosition(grape->sprite, grape->object->box.pos.x, grape->object->box.pos.y);

					} else {
						// release
						releaseGrape(grape);
						shot->grapes[idx_grape] = 0;
						shot->grapes_count--;
					}
				}
			}

			if (!shot->grapes_count) {
				releaseShot(shot);
				level->shots.e[idx_shot] = 0;
				level->shots.count--;
			}
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
			releaseGrape(shot->grapes[idx]);
			shot->grapes[idx] = 0;
		}
	}

	shot->grapes_count = 0;
	shot->grapes_size = 0;
	MEM_free(shot);
}

static void releaseGrape(Grape* grape) {

	if (grape) {
		SPR_releaseSprite(grape->sprite);
		grape->sprite = 0;
		MEM_free(grape->object);
		grape->object = 0;
		MEM_free(grape);
	}
}
