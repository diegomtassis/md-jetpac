/*
 * shooting.c
 *
 *  Created on: May 26, 2019
 *      Author: diegomtassis
 */

#include "../inc/shooting.h"
#include "../inc/game.h"
#include "../inc/config/game_config.h"

#include <maths.h>
#include <memory.h>
#include <sprite_eng.h>
#include <tools.h>
#include <types.h>
#include <vdp_tile.h>

#include "../inc/constants.h"
#include "../inc/fwk/array_fixed_list.h"
#include "../inc/fwk/commons.h"
#include "../inc/fwk/physics.h"
#include "../res/zx.h"

#define JETMAN_WIDTH 16
#define GUN_Y_OFFSET 11

#define SHOT_TYPES	3

#define RANGE_SHORT 22
#define RANGE_MEDIUM 28
#define RANGE_LONG 35

#define BURST_A 0
#define BURST_B 1
#define BURST_C 2
#define BURST_D 3
#define BURST_E 4

#define SPEED_LASER	5
#define SPEED_LASER_F16	FIX16(SPEED_LASER)

#define GRAPE_WIDTH 24
#define GRAPE_HEIGHT 1

#define GRAPES_PER_SHOT 5
#define GAP_BETWEEN_SHOTS GRAPE_WIDTH + 1

static void releaseShotIfNoGrapes(Planet planet[static 1], u8 shot_idx);
static void releaseShot(Shot* shot);
static Grape* createGrape(V2s16 where, bool to_left, u8 type, u8 range, u8 burst);
static void releaseGrapeInShot(Planet planet[static 1], u8 idx_shot, u8 idx_grape);
static void releaseGrape(Grape* grape);

static u8 checkShotHit(Shot* shot, Box_s16 object);
static bool crashedIntoPlatform(Shot shot[static 1], Grape grape[static 1], Planet planet[static 1]);
static bool checkGrapeHit(Grape grape[static 1], Box_s16 object_box);

void initShots(Planet planet[static 1]) {

	const u8 players = game_config.players ? game_config.players : ONE_PLAYER;
	arrayFixedListInit(&planet->shots, MAX_SHOTS_PER_PLAYER * players);
}

void releaseShots(Planet planet[static 1]) {

	for (u8 idx = planet->shots.size; idx;) {

		releaseShot(planet->shots.e[--idx]);
		planet->shots.e[idx] = 0;
	}

	arrayFixedListRelease(&planet->shots);
}

void shoot(Jetman* shooter, Planet planet[static 1]) {

	if (!shooter || shooter->shots == MAX_SHOTS_PER_PLAYER) {
		return;
	}

	s16 where_y = shooter->object.box.min.y + GUN_Y_OFFSET;

	Shot* last_shot = shooter->last_shot;
	if (last_shot && last_shot->where.y == where_y && last_shot->gap_to_start < GAP_BETWEEN_SHOTS) {
		// no 2 shots in the same y
		return;
	}

	Shot* shot = MEM_calloc(sizeof *shot);

	shot->shooter = shooter;
	shooter->last_shot = shot;
	shooter->shots++;

	arrayFixedListAdd(&planet->shots, shot);

	shot->where.x = shooter->object.box.min.x + (shooter->head_back ? 0 : JETMAN_WIDTH);
	shot->where.y = where_y;
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

	arrayFixedListInit(&shot->grapes, GRAPES_PER_SHOT);
	shot->grapes_left = GRAPES_PER_SHOT;

	arrayFixedListAdd(&shot->grapes,
			createGrape(shot->where, shot->to_left, shot->type, shot->range, --shot->grapes_left));
	shot->gap_to_start = 0;
}

void updateShots(Planet planet[static 1]) {

	Shot* shot = 0;
	for (u8 idx_shot = planet->shots.size; idx_shot;) {

		shot = planet->shots.e[--idx_shot];
		if (shot) {

			Grape* grape = 0;
			for (u8 idx_grape = 0; idx_grape < GRAPES_PER_SHOT; ++idx_grape) {

				grape = shot->grapes.e[idx_grape];
				if (grape) {

					grape->life_left--;
					if (!grape->life_left) {
						// release
						releaseGrapeInShot(planet, idx_shot, idx_grape);

					} else {
						// move
						Box_s16 target_h = targetHBox(grape->object);
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
				arrayFixedListRemoveAt(&planet->shots, idx_shot);
				releaseShot(shot);
				continue;
			}

			shot->gap_to_start += SPEED_LASER;

			if (shot->grapes_left) {

				s16 gap_if_grape_added = shot->gap_to_start - GRAPE_WIDTH;
				if (gap_if_grape_added >= 0) {

					V2s16 grape_where = { .x = shot->where.x, .y = shot->where.y };
					if (shot->to_left) {
						grape_where.x -= gap_if_grape_added;
					} else {
						grape_where.x += gap_if_grape_added;
					}

					arrayFixedListAdd(&shot->grapes,
							createGrape(grape_where, shot->to_left, shot->type, shot->range, --shot->grapes_left));
					shot->gap_to_start = gap_if_grape_added;
				}
			}
		}
	}
}

u8 checkHit(Box_s16 subject, Planet planet[static 1]) {

	Shot* shot = 0;

	for (u8 idx_shot = planet->shots.size; idx_shot;) {

		shot = planet->shots.e[--idx_shot];
		if (shot && checkShotHit(shot, subject)) {

			arrayFixedListRemoveAt(&planet->shots, idx_shot);
			releaseShot(shot);

			return TRUE;
		}
	}

	return 0;
}

static u8 checkShotHit(Shot* shot, Box_s16 object) {

	if (shot) {
		for (u8 idx_grape = GRAPES_PER_SHOT; idx_grape;) {
			Grape* grape = shot->grapes.e[--idx_grape];
			if (grape && checkGrapeHit(grape, object)) {
				return shot->shooter->id;
			}
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

		arrayFixedListRemoveAt(&planet->shots, idx_shot);
		releaseShot(shot);
	}
}

static void releaseShot(Shot* shot) {

	if (!shot) {
		return;
	}

	Grape* grape = 0;
	for (u8 idx = shot->grapes.size; idx;) {

		grape = shot->grapes.e[--idx];
		if (grape) {
			releaseGrape(grape);
		}
	}

	arrayFixedListRelease(&shot->grapes);

	Jetman* shooter = shot->shooter;
	if (shooter->last_shot && shooter->last_shot == shot) {
		shooter->last_shot = 0;
	}

	shooter->shots--;
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

	for (u8 idx = planet->num_platforms; idx;) {
		if (checkGrapeHit(grape, planet->platforms[--idx]->object.box)) {
			return TRUE;
		}
	}

	return FALSE;
}

static bool checkGrapeHit(Grape grape[static 1], Box_s16 object_box) {

// optimization cause grapes have height 1
	return ((IN_BETWEEN & axisYPxRelativePos(grape->object->box.min.y, &object_box))
			&& (OVERLAPPED & axisXBoxRelativePos(&grape->object->box, &object_box)));
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

	arrayFixedListRemoveAt(&shot->grapes, idx_grape);
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
