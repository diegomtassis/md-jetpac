/*
 * enemy.c
 *
 *  Created on: Nov 13, 2019
 *      Author: diegomtassis
 */

#include "../inc/enemy.h"

#include "../../inc/fwk/physics.h"


void updatePosition(Enemy* enemy, Box_s16 target) {

	// horizontal position
	if (target.pos.x > ENEMY_DEFAULT_MAX_POS_H_PX_S16) {
		enemy->object.pos.x = ENEMY_DEFAULT_MIN_POS_H_PX_F16;

	} else if (target.pos.x < ENEMY_DEFAULT_MIN_POS_H_PX_S16) {
		enemy->object.pos.x = ENEMY_DEFAULT_MAX_POS_H_PX_F16;
	} else {
		enemy->object.pos.x += enemy->object.mov.x;
	}

	enemy->object.pos.y += enemy->object.mov.y;

	// update box
	updateBox(&enemy->object);
}

bool crashedIntoPlatform(Box_s16 subject_box, const Level level[static 1]) {

	bool crashed = overlap(subject_box, level->floor->object.box);
	if (crashed) {
		return TRUE;
	}

	for (u8 i = 0; i < level->num_platforms; i++) {
		crashed = overlap(subject_box, level->platforms[i]->object.box);
		if (crashed) {
			return TRUE;
		}
	}

	return FALSE;
}
