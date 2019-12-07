/*
 * enemy.c
 *
 *  Created on: Nov 13, 2019
 *      Author: diegomtassis
 */

#include "../../inc/enemy.h"

#include <genesis.h>

#include "../../inc/fwk/commons.h"
#include "../../inc/fwk/physics.h"

Enemy* createEnemy(const EnemyDefinition definition[static 1]) {

	Enemy* enemy = MEM_calloc(sizeof *enemy);
	enemy->definition = definition;
	enemy->health = ALIVE;

	// size
	enemy->object.size.x = definition->size_t.x;
	enemy->object.size.y = definition->size_t.y;

	return enemy;
}

void releaseEnemy(Enemy enemy[static 1]) {

	MEM_free(enemy);
}

void initPosAndMov(Enemy enemy[static 1], fix16 mov_x, fix16 mov_y) {

	// horizontal
	if (random() % 2) {
		enemy->object.pos.x = ENEMY_DEFAULT_MIN_POS_H_PX_F16;
		enemy->object.mov.x = mov_x;
	} else {
		enemy->object.pos.x = ENEMY_DEFAULT_MAX_POS_H_PX_F16;
		enemy->object.mov.x = -mov_x;
	}

	// vertical
	enemy->object.pos.y = randomInRangeFix16(MIN_POS_V_PX_F16, ENEMY_DEFAULT_MAX_POS_START_V_PX_F16);
	enemy->object.mov.y = mov_y;
}

void initBox(Enemy enemy[static 1]) {

	enemy->object.box.w = enemy->definition->size_t.x;
	enemy->object.box.h = enemy->definition->size_t.y;

	enemy->object.box.pos.x = fix16ToInt(enemy->object.pos.x);
	enemy->object.box.pos.y = fix16ToInt(enemy->object.pos.y);
}

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

bool crashedIntoPlatform(Box_s16 subject_box, const Planet planet[static 1]) {

	bool crashed = overlap(subject_box, planet->floor->object.box);
	if (crashed) {
		return TRUE;
	}

	for (u8 i = 0; i < planet->num_platforms; i++) {
		crashed = overlap(subject_box, planet->platforms[i]->object.box);
		if (crashed) {
			return TRUE;
		}
	}

	return FALSE;
}
