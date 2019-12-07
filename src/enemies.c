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

static const int MIN_TIME_BETWEEN_ENEMIES = SUBTICKPERSECOND * 1.3;
static const int MAX_TIME_BETWEEN_ENEMIES = SUBTICKPERSECOND * 2;

static void addEnemy(Planet planet[static 1], u8 pos);
static void releaseDeadEnemies(Planet planet[static 1]);
static void releaseDeadEnemy(Enemy enemy[static 1]);
static void enemiesJoin(Planet planet[static 1]);

static void enemyActs(Enemy enemy[static 1], Planet planet[static 1]);

static void detectNuclearBomb();

bool nuclear_bomb;

void startEnemies(Planet planet[static 1]) {

	// First time
	planet->enemies.e = MEM_calloc(sizeof(Enemy*) * planet->enemies.size);

	setRandomSeed(getTick());

	// start with a portion of the maximum enemies
	for (u8 enemy_idx = 0; enemy_idx < planet->enemies.size / 3; enemy_idx++) {
		addEnemy(planet, enemy_idx);
	}

	startCountDownRandom(ENEMY_CREATION_COUNTDOWN, MIN_TIME_BETWEEN_ENEMIES, MAX_TIME_BETWEEN_ENEMIES);
}

void enemiesAct(Planet planet[static 1]) {

	if (!planet->enemies.e) {
		return;
	}

//	detectNuclearBomb();

	for (u8 idx = 0; idx < planet->enemies.size; idx++) {

		Enemy* enemy = planet->enemies.e[idx];
		if (enemy) {

			if (ALIVE & enemy->health) {
				if (nuclear_bomb) {
					killEnemy(enemy, planet, TRUE);
				} else {
					enemyActs(enemy, planet);
					SPR_setPosition(enemy->sprite, fix16ToInt(enemy->object.pos.x), fix16ToInt(enemy->object.pos.y));
				}
			}
		}
	}

	releaseDeadEnemies(planet);

	// New enemies joining the party?
	enemiesJoin(planet);

	nuclear_bomb = FALSE;
}

void killEnemy(Enemy* enemy, Planet planet[static 1], u8 exploding) {

	if (exploding) {
		explode(enemy->object.box, planet);
	}

	enemy->health = DEAD;
}

void releaseEnemies(Planet planet[static 1]) {

	if (!planet->enemies.e) {
		return;
	}

	for (u8 idx = 0; idx < planet->enemies.size; idx++) {

		Enemy* enemy = planet->enemies.e[idx];
		if (enemy) {
			releaseDeadEnemy(enemy);
		}
	}

	planet->enemies.count = 0;
	memset(planet->enemies.e, 0, planet->enemies.size);
	MEM_free(planet->enemies.e);
	planet->enemies.e = 0;
}

void releaseDeadEnemies(Planet planet[static 1]) {

	if (!planet->enemies.e) {
		return;
	}

	for (u8 idx = 0; idx < planet->enemies.size; idx++) {

		Enemy* enemy = planet->enemies.e[idx];
		if (enemy && (enemy->health & DEAD)) {

			releaseDeadEnemy(enemy);
			planet->enemies.e[idx] = 0;
			planet->enemies.count--;
		}
	}
}

static void releaseDeadEnemy(Enemy enemy[static 1]) {

	enemy->definition->releaseFunc(enemy);

	startCountDownRandom(ENEMY_CREATION_COUNTDOWN, MIN_TIME_BETWEEN_ENEMIES, MAX_TIME_BETWEEN_ENEMIES);
}

static void addEnemy(Planet planet[static 1], u8 pos) {

	Enemy* enemy = planet->def.enemy_def.createFunc(&planet->def.enemy_def);
	planet->enemies.e[pos] = enemy;
	planet->enemies.count++;

	startCountDownRandom(ENEMY_CREATION_COUNTDOWN, MIN_TIME_BETWEEN_ENEMIES, MAX_TIME_BETWEEN_ENEMIES);
}

static void enemiesJoin(Planet planet[static 1]) {

	if (planet->enemies.count < planet->enemies.size && isCountDownFinished(ENEMY_CREATION_COUNTDOWN, FALSE)) {

		u8 num_enemies = planet->enemies.size;
		u8 idx;
		while (num_enemies--) {
			// find the first empty slot
			Enemy* enemy = planet->enemies.e[num_enemies];
			if (!enemy) {
				idx = num_enemies;
				break;
			}
		}

		addEnemy(planet, idx);
	}
}

static void enemyActs(Enemy enemy[static 1], Planet planet[static 1]) {

	enemy->definition->actFunc(enemy, planet);
}

static void detectNuclearBomb() {

	if (JOY_readJoypad(JOY_1) & BUTTON_A) {
		nuclear_bomb = TRUE;
	}
}
