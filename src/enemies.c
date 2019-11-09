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
#include "../res/sprite.h"

#define SPEED_ZERO		FIX16_0
#define SPEED_H_NORMAL	FIX16(1)
#define SPEED_V_NORMAL	FIX16(0.3)

#define ENEMY_DEFAULT_MIN_POS_H_PX_S16			LEFT_POS_H_PX_S16 + 8
#define ENEMY_DEFAULT_MAX_POS_H_PX_S16			RIGHT_POS_H_PX_S16 - 8
#define ENEMY_DEFAULT_MAX_POS_V_PX_S16			BOTTOM_POS_V_PX_S16 - 16

#define ENEMY_DEFAULT_MIN_POS_H_PX_F16			FIX16(ENEMY_DEFAULT_MIN_POS_H_PX_S16)
#define ENEMY_DEFAULT_MAX_POS_H_PX_F16			FIX16(ENEMY_DEFAULT_MAX_POS_H_PX_S16)
#define ENEMY_DEFAULT_MAX_POS_V_PX_F16			FIX16(MAX_POS_V_PX_S16)
#define ENEMY_DEFAULT_MAX_POS_START_V_PX_F16	FIX16(ENEMY_DEFAULT_MAX_POS_V_PX_S16 - 32)

static const int MIN_TIME_BETWEEN_ENEMIES = SUBTICKPERSECOND * 1.3;
static const int MAX_TIME_BETWEEN_ENEMIES = SUBTICKPERSECOND * 2;

static void addEnemy(Level level[static 1], u8 pos);
static Enemy* createEnemy(EnemyDefinition);
static void releaseDeadEnemies(Level level[static 1]);
static void releaseEnemy(Enemy*);
static void enemiesJoin(Level level[static 1]);

static void enemyActs(Enemy*, Level*);
static void calculateNextMovement(Enemy*);
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

	SPR_releaseSprite(enemy->sprite);
	MEM_free(enemy);

	startCountDownRandom(ENEMY_CREATION_COUNTDOWN, MIN_TIME_BETWEEN_ENEMIES, MAX_TIME_BETWEEN_ENEMIES);
}

static void addEnemy(Level level[static 1], u8 pos) {

	// object
	Enemy* enemy = createEnemy(level->def.enemy_def);
	level->enemies.e[pos] = enemy;

	// sprite
	const SpriteDefinition* sprite_definition;
	if (level->def.enemy_def.sprite_def) {
		sprite_definition = level->def.enemy_def.sprite_def;
	} else {
		sprite_definition = &meteorite_sprite;
	}

	Sprite* enemySprite = SPR_addSprite(sprite_definition, fix16ToInt(enemy->object.pos.x),
			fix16ToInt(enemy->object.pos.y), TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
	SPR_setAnim(enemySprite, (abs(random())) % 4);
	SPR_setFrame(enemySprite, (abs(random())) % 2);
	enemy->sprite = enemySprite;

	if (enemy->object.mov.x > 0) {
		SPR_setHFlip(enemySprite, TRUE);
	}

	level->enemies.count++;
}

static Enemy* createEnemy(EnemyDefinition enemy_def) {

	Enemy* enemy = MEM_calloc(sizeof *enemy);
	enemy->enemy_def = &enemy_def;
	enemy->type = enemy_def.type;
	enemy->health = ALIVE;

	enemy->enemy_def->createEnemyFunc();

	// size
	enemy->object.size.x = enemy_def.size_t.x;
	enemy->object.size.y = enemy_def.size_t.y;

	// position & direction
	if (random() % 2) {
		enemy->object.pos.x = ENEMY_DEFAULT_MIN_POS_H_PX_F16;
		enemy->object.mov.x = SPEED_H_NORMAL;
	} else {
		enemy->object.pos.x = ENEMY_DEFAULT_MAX_POS_H_PX_F16;
		enemy->object.mov.x = -SPEED_H_NORMAL;
	}

	enemy->object.pos.y = randomInRangeFix16(MIN_POS_V_PX_F16, ENEMY_DEFAULT_MAX_POS_START_V_PX_F16);

	// V speed
	if (random() % 2) {
		enemy->object.mov.y = SPEED_ZERO;
	} else {
		enemy->object.mov.y = SPEED_V_NORMAL;
	}

	// box
	if (enemy_def.size_t.x) {
		enemy->object.box.w = enemy_def.size_t.x;
	} else {
		enemy->object.box.w = METEORITE_WIDTH;
	}

	if (enemy_def.size_t.y) {
		enemy->object.box.h = enemy_def.size_t.y;
	} else {
		enemy->object.box.h = METEORITE_HEIGHT;
	}

	enemy->object.box.pos.x = fix16ToInt(enemy->object.pos.x);
	enemy->object.box.pos.y = fix16ToInt(enemy->object.pos.y);

	startCountDownRandom(ENEMY_CREATION_COUNTDOWN, MIN_TIME_BETWEEN_ENEMIES, MAX_TIME_BETWEEN_ENEMIES);
	return enemy;
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

	calculateNextMovement(enemy);
	updatePosition(enemy, level);
}

static void calculateNextMovement(Enemy* enemy) {
	// nothing for the moment
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
