/*
 * shooting.c
 *
 *  Created on: May 26, 2019
 *      Author: diegomtassis
 */

#include "../inc/shooting.h"

#include <genesis.h>

#include "../inc/constants.h"
#include "../inc/fwk/commons.h"
#include "../inc/fwk/physics.h"
#include "../res/sprite.h"

#define JETMAN_WIDTH 16
#define GUN_Y_OFFSET 11

#define SHOT_TYPES	3

#define RANGE_SHORT 22
#define RANGE_MEDIUM 28
#define RANGE_LONG 35

#define BURST_A 0
#define BURST_B 1
#define BURST_C 2

#define SPEED_LASER	5
#define SPEED_LASER_F16	FIX16(SPEED_LASER)

#define GRAPE_WIDTH 16
#define GRAPE_HEIGHT 1

static const u8 BURST_TYPE_PER_GRAPE[6] = { BURST_A, BURST_A, BURST_B, BURST_B, BURST_C, BURST_C };

static void releaseShotIfNoGrapes(Planet planet[static 1], u8 shot_idx);
static void releaseShot(Shot* shot);
static Grape* createGrape(V2s16 where, bool to_left, u8 type, u8 range, u8 burst);
static void releaseGrapeInShot(Planet planet[static 1], u8 idx_shot, u8 idx_grape);
static void releaseGrape(Grape* grape);

static u8 checkShotHit(Shot* shot, Box_s16 object);
static bool crashedIntoPlatform(Shot shot[static 1], Grape grape[static 1], Planet planet[static 1]);
static bool checkGrapeHit(Grape grape[static 1], Box_s16 object_box);

void initShots(Planet planet[static 1]) {

	planet->shots.count = 0;
	planet->shots.size = MAX_SHOTS_PER_PLAYER * planet->game->config->players;
	planet->shots.e = MEM_calloc(planet->shots.size * sizeof(Shot*));
}

void releaseShots(Planet planet[static 1]) {

	for (int idx = 0; idx < planet->shots.size; idx++) {
		releaseShot(planet->shots.e[idx]);
		planet->shots.e[idx] = 0;
	}

	planet->shots.count = 0;
	planet->shots.size = 0;
	MEM_free(planet->shots.e);
	planet->shots.e = 0;
}

void shoot(Jetman* shooter, Planet planet[static 1]) {

	if (!shooter || shooter->shots == MAX_SHOTS_PER_PLAYER) {
		return;
	}

	Shot* shot = MEM_calloc(sizeof *shot);

	shot->shooter = shooter;
	shooter->shots++;

	list_add(&planet->shots, shot);

	shot->where.x = shooter->object.box.min.x + (shooter->head_back ? 0 : JETMAN_WIDTH);
	shot->where.y = shooter->object.box.min.y + GUN_Y_OFFSET;
	shot->to_left = shooter->head_back;
	shot->type = abs(random()) % SHOT_TYPES;

	u8 range = abs(random()) % 3;
	if (range == 0) {
		shot->range = RANGE_SHORT;
	} else if (range == 1) {
		shot->range = RANGE_MEDIUM;
	} else {
		shot->range = RANGE_LONG;
	}

	shot->grapes.size = 6;
	shot->grapes.count = 0;
	shot->grapes.e = MEM_calloc(sizeof(Grape*) * shot->grapes.size);
	shot->grapes_created = 0;

	list_add(&shot->grapes, createGrape(shot->where, shot->to_left, shot->type, shot->range, BURST_A));
	shot->grapes_created++;
	shot->distance_to_last = 0;
}

void updateShots(Planet planet[static 1]) {

	Shot* shot = 0;
	for (int idx_shot = 0; idx_shot < planet->shots.size; idx_shot++) {

		shot = planet->shots.e[idx_shot];
		if (shot) {

			Grape* grape = 0;
			for (int idx_grape = 0; idx_grape < shot->grapes.size; ++idx_grape) {

				grape = shot->grapes.e[idx_grape];
				if (grape) {

					grape->life_left--;
					if (!grape->life_left) {
						// release
						releaseGrapeInShot(planet, idx_shot, idx_grape);

					} else {
						// move
						Box_s16 target_h = targetHBox(*grape->object);
						if (target_h.min.x > MAX_POS_H_PX_S16) {
							grape->object->pos.x = FIX16(target_h.min.x - LEVEL_WIDTH_PX_S16);

						} else if (target_h.min.x < MIN_POS_H_PX_S16) {
							grape->object->pos.x = FIX16(LEVEL_WIDTH_PX_S16 + target_h.min.x);

						} else {
							grape->object->pos.x += grape->object->mov.x;
						}

						updateBox(grape->object);
						SPR_setPosition(grape->sprite, grape->object->box.min.x, grape->object->box.min.y);
					}
				}
			}

			// check if the first grape crashed into a platform
			grape = shot->grapes.e[0];
			if (grape && crashedIntoPlatform(shot, grape, planet)) {
				list_remove_at(&planet->shots, idx_shot);
				releaseShot(shot);
				continue;
			}

			shot->distance_to_last += SPEED_LASER;
			if (shot->grapes_created < shot->grapes.size && shot->distance_to_last >= GRAPE_WIDTH) {

				list_add(&shot->grapes,
						createGrape(shot->where, shot->to_left, shot->type, shot->range,
								BURST_TYPE_PER_GRAPE[shot->grapes_created]));
				shot->grapes_created++;
				shot->distance_to_last = 0;
			}
		}
	}
}

u8 checkHit(Box_s16 subject, Planet planet[static 1]) {

	Shot* shot = 0;

	for (int idx_shot = 0; idx_shot < planet->shots.size; idx_shot++) {

		shot = planet->shots.e[idx_shot];
		if (shot && checkShotHit(shot, subject)) {

			list_remove_at(&planet->shots, idx_shot);
			releaseShot(shot);

			return TRUE;
		}
	}

	return 0;
}

static u8 checkShotHit(Shot* shot, Box_s16 object) {

	if (shot) {

		// check only the first grape
		Grape* grape = shot->grapes.e[0];
		if (grape && checkGrapeHit(grape, object)) {
			return shot->shooter->id;
		}
	}

	return 0;
}

static void releaseShotIfNoGrapes(Planet planet[static 1], u8 idx_shot) {

	if (!planet->shots.count) {
		return;
	}

	Shot* shot = planet->shots.e[idx_shot];
	if (shot && !shot->grapes.count) {

		list_remove_at(&planet->shots, idx_shot);
		releaseShot(shot);
	}
}

static void releaseShot(Shot* shot) {

	if (!shot) {
		return;
	}

	Grape* grape = 0;
	for (int idx = 0; idx < shot->grapes.size; idx++) {

		grape = shot->grapes.e[idx];
		if (grape) {
			releaseGrape(grape);
		}
	}

	shot->grapes.count = 0;
	shot->grapes.size = 0;
	MEM_free(shot->grapes.e);
	shot->grapes.e = 0;

	shot->shooter->shots--;
	shot->shooter = 0;

	MEM_free(shot);
}

static Grape* createGrape(V2s16 where, bool to_left, u8 type, u8 range, u8 burst) {

	Grape* grape = MEM_calloc(sizeof(*grape));

	Object_f16* object = MEM_calloc(sizeof(Object_f16));
	grape->object = object;
	grape->object->pos.x = FIX16(where.x - (to_left ? JETMAN_WIDTH : 0));
	grape->object->pos.y = FIX16(where.y);
	grape->object->mov.x = to_left ? -SPEED_LASER_F16 : SPEED_LASER_F16;
	grape->object->mov.y = FIX16_0;
	grape->object->size.x = GRAPE_WIDTH;
	grape->object->size.y = GRAPE_HEIGHT;
	grape->object->box.w = GRAPE_WIDTH;
	grape->object->box.h = GRAPE_HEIGHT;
	updateBox(grape->object);

	grape->life_left = range;

	grape->sprite = SPR_addSprite(&shot_sprite, grape->object->box.min.x, grape->object->box.min.y,
			TILE_ATTR(PAL0, TRUE, FALSE, to_left));
	SPR_setAnim(grape->sprite, type);
	SPR_setFrame(grape->sprite, burst);

	return grape;
}

static bool crashedIntoPlatform(Shot shot[static 1], Grape grape[static 1], Planet planet[static 1]) {

	if (checkGrapeHit(grape, planet->floor->object.box)) {
		return TRUE;
	}

	for (u8 i = 0; i < planet->num_platforms; i++) {
		if (checkGrapeHit(grape, planet->platforms[i]->object.box)) {
			return TRUE;
		}
	}

	return FALSE;
}

static bool checkGrapeHit(Grape grape[static 1], Box_s16 object_box) {

	// optimization cause grapes have height 1
	return ((IN_BETWEEN & axisYPxRelativePos(grape->object->box.min.y, object_box))
			&& (OVERLAPPED & axisXBoxRelativePos(grape->object->box, object_box)));
}

static void releaseGrapeInShot(Planet planet[static 1], u8 idx_shot, u8 idx_grape) {

	Shot* shot = planet->shots.e[idx_shot];
	if (!shot) {
		return;
	}

	Grape* grape = shot->grapes.e[idx_grape];
	if (!grape) {
		return;
	}

	list_remove_at(&shot->grapes, idx_grape);
	releaseGrape(grape);
	releaseShotIfNoGrapes(planet, idx_shot);
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
