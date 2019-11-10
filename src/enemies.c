/*
 * enemies.c
 *
 *  Created on: Apr 24, 2019
 *      Author: diegomtassis
 */

#include "../inc/enemies.h"

#include <genesis.h>

#include "../inc/enemies.h"
#include "../inc/constants.h"
#include "../inc/explosions.h"
#include "../inc/fwk/commons.h"
#include "../inc/fwk/physics.h"
#include "../inc/fwk/physics.h"
#include "../inc/timers.h"

#define SPEED_ZERO		FIX16_0
#define SPEED_H_NORMAL	FIX16(1)
#define SPEED_V_NORMAL	FIX16(0.3)

static const int MIN_TIME_BETWEEN_ENEMIES = SUBTICKPERSECOND * 1.3;
static const int MAX_TIME_BETWEEN_ENEMIES = SUBTICKPERSECOND * 2;

static void addEnemy(Level level[static 1], u8 pos);
static void releaseDeadEnemies(Level level[static 1]);
static void releaseEnemy(Enemy*);
static void enemiesJoin(Level level[static 1]);

static void enemyActs(Enemy*, Level*);
static void updatePosition(Enemy*, Level*);
static bool crashedIntoPlatform(Box_s16 subject_box, const Level level[static 1]);

static void detectNuclearBomb();

bool nuclear_bomb;

void startEnemies(Level level[static 1]) {

	// First time
	level->enemies.e = MEM_calloc(sizeof(Enemy*) * level->enemies.size);

	setRandomSeed(getTick());

	// start with a portion of the maximum enemies
	for (u8 enemy_idx = 0; enemy_idx < level->enemies.size / 3; enemy_idx++) {
		addEnemy(level, enemy_idx);
	}

	startCountDownRandom(ENEMY_CREATION_COUNTDOWN, MIN_TIME_BETWEEN_ENEMIES, MAX_TIME_BETWEEN_ENEMIES);
}

void enemiesAct(Level level[static 1]) {

	if (!level->enemies.e) {
		return;
	}

//	detectNuclearBomb();

	for (u8 idx = 0; idx < level->enemies.size; idx++) {

		Enemy* enemy = level->enemies.e[idx];
		if (enemy) {

			if (ALIVE & enemy->health) {
				if (nuclear_bomb) {
					killEnemy(enemy, level, TRUE);
				} else {
					enemyActs(enemy, level);
					SPR_setPosition(enemy->sprite, fix16ToInt(enemy->object.pos.x), fix16ToInt(enemy->object.pos.y));
				}
			}
		}
	}

	releaseDeadEnemies(level);

	// New enemies joining the party?
	enemiesJoin(level);

	nuclear_bomb = FALSE;
}

void killEnemy(Enemy* enemy, Level level[static 1], u8 exploding) {

	if (exploding) {
		explode(enemy->object.box, level);
	}

	enemy->health = DEAD;
}

void releaseEnemies(Level level[static 1]) {

	if (!level->enemies.e) {
		return;
	}

	for (u8 idx = 0; idx < level->enemies.size; idx++) {

		Enemy* enemy = level->enemies.e[idx];
		if (enemy) {
			releaseEnemy(enemy);
		}
	}

	level->enemies.count = 0;
	memset(level->enemies.e, 0, level->enemies.size);
	MEM_free(level->enemies.e);
	level->enemies.e = 0;
}

void releaseDeadEnemies(Level level[static 1]) {

	if (!level->enemies.e) {
		return;
	}

	for (u8 idx = 0; idx < level->enemies.size; idx++) {

		Enemy* enemy = level->enemies.e[idx];
		if (enemy && (enemy->health & DEAD)) {

			releaseEnemy(enemy);
			level->enemies.e[idx] = 0;
			level->enemies.count--;
		}
	}
}

static void releaseEnemy(Enemy* enemy) {

	enemy->definition->dieFunc(enemy);
	MEM_free(enemy);

	startCountDownRandom(ENEMY_CREATION_COUNTDOWN, MIN_TIME_BETWEEN_ENEMIES, MAX_TIME_BETWEEN_ENEMIES);
}

static void addEnemy(Level level[static 1], u8 pos) {

	Enemy* enemy = MEM_calloc(sizeof *enemy);
	enemy->definition = &level->def.enemy_def;
	enemy->health = ALIVE;

	enemy->definition->growFunc(enemy);

	startCountDownRandom(ENEMY_CREATION_COUNTDOWN, MIN_TIME_BETWEEN_ENEMIES, MAX_TIME_BETWEEN_ENEMIES);

	level->enemies.e[pos] = enemy;

	level->enemies.count++;
}

static void enemiesJoin(Level level[static 1]) {

	if (level->enemies.count < level->enemies.size && isCountDownFinished(ENEMY_CREATION_COUNTDOWN, FALSE)) {

		u8 num_enemies = level->enemies.size;
		u8 idx;
		while (num_enemies--) {
			// find the first empty slot
			Enemy* enemy = level->enemies.e[num_enemies];
			if (!enemy) {
				idx = num_enemies;
				break;
			}
		}

		addEnemy(level, idx);
	}
}

static void enemyActs(Enemy* enemy, Level level[static 1]) {

	enemy->definition->actFunc(enemy);
	updatePosition(enemy, level);
}

static void updatePosition(Enemy* enemy, Level level[static 1]) {

	// horizontal position
	Box_s16 target = targetBox(enemy->object);
	if (crashedIntoPlatform(target, level)) {

		killEnemy(enemy, level, TRUE);
		return;
	}

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

static bool crashedIntoPlatform(Box_s16 subject_box, const Level level[static 1]) {

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

static void detectNuclearBomb() {

	if (JOY_readJoypad(JOY_1) & BUTTON_A) {
		nuclear_bomb = TRUE;
	}
}
