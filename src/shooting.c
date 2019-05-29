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

#define MAX_SHOTS 	3

#define RANGE_SHORT 10
#define RANGE_MEDIUM 20
#define RANGE_LONG 30

#define BURST_A 0
#define BURST_B 1
#define BURST_C 2

#define SPEED_LASER	6
#define SPEED_LASER_F16	FIX16(SPEED_LASER)

#define GRAPE_WIDTH 16
#define GRAPE_HEIGHT 1

#define MIN_POS_H_PX_S16	LEFT_POS_H_PX_S16 - 8
#define MAX_POS_H_PX_S16	RIGHT_POS_H_PX_S16 - 8

#define MIN_POS_H_PX_F16	FIX16(MIN_POS_H_PX_S16)
#define MAX_POS_H_PX_F16	FIX16(MAX_POS_H_PX_S16)

static void releaseShotIfNoGrapes(Level level[static 1], u8 shot_idx);
static void releaseShot(Shot* shot);
static Grape* createGrape(V2s16 where, bool to_left, u8 type, u8 range, u8 burst);
static void releaseGrapeInShot(Level level[static 1], u8 idx_shot, u8 idx_grape);
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

	shot->where.x = where.x;
	shot->where.y = where.y;
	shot->to_left = to_left;
	shot->type = abs(random()) % 3;

	u8 range = abs(random()) % 3;
	if (range == 0) {
		shot->range = RANGE_SHORT;
	} else if (range == 1) {
		shot->range = RANGE_MEDIUM;
	} else {
		shot->range = RANGE_LONG;
	}

	shot->grapes_size = 2;
	shot->grapes_count = 0;
	shot->grapes_created = 0;
	shot->grapes = MEM_alloc(shot->grapes_size * sizeof(Grape*));
	memset(shot->grapes, 0, shot->grapes_size);

	shot->grapes[0] = createGrape(where, to_left, shot->type, shot->range, BURST_A);
	shot->grapes_count++;
	shot->grapes_created++;
	shot->distance_to_last = 0;
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
							grape->object->pos.x = MIN_POS_H_PX_F16 + FIX16(target_h.pos.x) - MAX_POS_H_PX_F16; // optimize

						} else if (target_h.pos.x < MIN_POS_H_PX_S16) {
							grape->object->pos.x = MAX_POS_H_PX_F16 - MIN_POS_H_PX_F16 + FIX16(target_h.pos.x); // optimize

						} else {
							grape->object->pos.x += grape->object->mov.x;
						}

						updateBox(grape->object);
						SPR_setPosition(grape->sprite, grape->object->box.pos.x, grape->object->box.pos.y);

					} else {
						// release
						releaseGrapeInShot(level, idx_shot, idx_grape);
					}
				}
			}

			shot->distance_to_last += SPEED_LASER;
			if (shot->grapes_created < shot->grapes_size && shot->distance_to_last >= GRAPE_WIDTH) {

				u8 burst = BURST_A;
				if (shot->grapes_created > 1) {
					burst = shot->grapes_created < 5 ? BURST_B : BURST_C;
				}

				shot->grapes[shot->grapes_created++] = createGrape(shot->where, shot->to_left, shot->type, shot->range,
						burst);
				shot->grapes_count++;
				shot->distance_to_last = 0;
			}
		}
	}
}

bool checkHit(Box_s16 subject, Level level[static 1]) {

	Shot* shot = 0;
	Grape* grape = 0;

	for (int idx_shot = 0; idx_shot < level->shots.size; idx_shot++) {
		shot = level->shots.e[idx_shot];
		if (shot) {
			for (int idx_grape = 0; idx_grape < shot->grapes_size; idx_grape++) {
				grape = shot->grapes[idx_grape];
				if (grape) {
					Box_s16 grape_box = grape->object->box;
					V2s16 grape_tip = { .x = grape_box.pos.x, .y = grape_box.pos.y };
					if (!shot->to_left) {
						grape_tip.x += grape_box.w - 1;
					}
					if (contained(grape_tip, subject)) {
						releaseGrapeInShot(level, idx_shot, idx_grape);
						return TRUE;
					}
				}
			}
		}
	}

	return FALSE;
}

static void releaseShotIfNoGrapes(Level level[static 1], u8 idx_shot) {

	if (!level->shots.count) {
		return;
	}

	Shot* shot = level->shots.e[idx_shot];
	if (shot && !shot->grapes_count) {

		releaseShot(shot);
		level->shots.e[idx_shot] = 0;
		level->shots.count--;
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
	shot->grapes_created = 0;
	shot->grapes_size = 0;
	MEM_free(shot);
}

static Grape* createGrape(V2s16 where, bool to_left, u8 type, u8 range, u8 burst) {

	Grape* grape = 0;
	grape = MEM_alloc(sizeof(Grape));
	memset(grape, 0, sizeof(Grape));

	Object_f16* object = MEM_alloc(sizeof(Object_f16));
	memset(grape->object, 0, sizeof(Object_f16));
	grape->object = object;
	grape->object->pos.x = FIX16(where.x - (to_left ? 16 : 0));
	grape->object->pos.y = FIX16(where.y);
	grape->object->mov.x = to_left ? -SPEED_LASER_F16 : SPEED_LASER_F16;
	grape->object->mov.y = FIX16_0;
	grape->object->size.x = GRAPE_WIDTH;
	grape->object->size.y = GRAPE_HEIGHT;
	grape->object->box.w = GRAPE_WIDTH;
	grape->object->box.h = GRAPE_HEIGHT;
	updateBox(grape->object);

	grape->life_left = range;

	grape->sprite = SPR_addSprite(&shot_sprite, grape->object->box.pos.x, grape->object->box.pos.y,
			TILE_ATTR(PAL0, TRUE, FALSE, to_left));
	SPR_setAnim(grape->sprite, type);
	SPR_setFrame(grape->sprite, burst);

	return grape;
}

static void releaseGrapeInShot(Level level[static 1], u8 idx_shot, u8 idx_grape) {

	Shot* shot = level->shots.e[idx_shot];
	if (!shot) {
		return;
	}

	Grape* grape = shot->grapes[idx_grape];
	if (!grape) {
		return;
	}

	releaseGrape(grape);
	shot->grapes[idx_grape] = 0;
	shot->grapes_count--;
	releaseShotIfNoGrapes(level, idx_shot);
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
