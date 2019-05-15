/*
 * enemies.c
 *
 *  Created on: Apr 24, 2019
 *      Author: diegomtassis
 */

#include "../inc/enemies.h"

#include <genesis.h>

#include "../inc/commons.h"
#include "../inc/timers.h"
#include "../inc/enemy_01.h"
#include "../inc/physics.h"
#include "../inc/explosions.h"
#include "../res/sprite.h"

#define SPEED_ZERO		FIX16_0
#define SPEED_H_NORMAL	FIX16(1)
#define SPEED_V_NORMAL	FIX16(0.3)

#define MIN_POS_H_PX_S16	LEFT_POS_H_PX_S16  + 8
#define MAX_POS_H_PX_S16	RIGHT_POS_H_PX_S16 - 8
#define MIN_POS_V_PX_S16	TOP_POS_V_PX_S16
#define MAX_POS_V_PX_S16	BOTTOM_POS_V_PX_S16 - 16

#define MIN_POS_H_PX_F16	FIX16(MIN_POS_H_PX_S16)
#define MAX_POS_H_PX_F16	FIX16(MAX_POS_H_PX_S16)
#define MIN_POS_V_PX_F16	FIX16(MIN_POS_V_PX_S16)
#define MAX_POS_V_PX_F16	FIX16(MAX_POS_V_PX_S16)
#define MAX_POS_START_V_PX_F16	FIX16(MAX_POS_V_PX_S16 - 32)

static void addEnemy(Level*, u8 pos);
static Enemy* createEnemy();
static void releaseEnemy(Enemy*);
static void enemiesJoin(Level* level);

static void moveEnemy(Enemy*, Level*);
static void calculateNextMovement(Enemy*);
static void updatePosition(Enemy*, Level*);
static u8 crashedIntoPlatform(Box_s16 subject_box, const Level* level);
static Sprite* createSprite(Enemy* enemy);

static void updateSprite(Enemy* enemy);

static void detectNuclearBomb();

bool nuclear_bomb;

void startEnemies(Level* level) {

	// First time
	level->enemies.objects = MEM_alloc(sizeof(Enemy*) * level->enemies.max_num_enemies);
	setRandomSeed(getTick());

	for (u8 idx = 0; idx < level->enemies.max_num_enemies; idx++) {
		level->enemies.objects[idx] = 0;
	}

	// start with a portion of the maximum enemies
	for (u8 enemy_idx = 0; enemy_idx < level->enemies.max_num_enemies / 3; enemy_idx++) {
		addEnemy(level, enemy_idx);
	}
}

void enemiesAct(Level* level) {

	detectNuclearBomb();

	u8 num_enemies = level->enemies.current_num_enemies;
	u8 current_enemy = 0;
	u8 enemy_idx = 0;

	while (current_enemy < num_enemies) {

		Enemy* enemy = level->enemies.objects[enemy_idx++];
		if (enemy) {

			if (ALIVE & enemy->health) {
				if (nuclear_bomb) {
					killEnemy(enemy, level, TRUE);
				} else {
					moveEnemy(enemy, level);
				}
				updateSprite(enemy);
			}

			current_enemy++;
		}
	}

	// New enemies joining the party?
	enemiesJoin(level);

	nuclear_bomb = FALSE;
}

void killEnemy(Enemy* enemy, Level* level, u8 exploding) {

	if (exploding) {
		explode(enemy->object.box, level);
	}

	enemy->health = DEAD;
}

void releaseAllEnemies(Level* level) {

	for (u8 idx = 0; idx < level->enemies.max_num_enemies; idx++) {

		Enemy* enemy = level->enemies.objects[idx];
		if (enemy) {
			enemy->health = DEAD;
		}
	}

	releaseDeadEnemies(level);

	MEM_free(level->enemies.objects);
	level->enemies.objects = 0;
}

void releaseDeadEnemies(Level* level) {

	for (u8 idx = 0; idx < level->enemies.max_num_enemies; idx++) {

		Enemy* enemy = level->enemies.objects[idx];
		if (enemy && (enemy->health & DEAD)) {

			releaseEnemy(enemy);
			level->enemies.objects[idx] = 0;
			level->enemies.current_num_enemies--;
		}
	}

}

static void releaseEnemy(Enemy* enemy) {

	SPR_releaseSprite(enemy->sprite);
	MEM_free(enemy);

	startTimer(ENEMY_CREATION_TIMER);
}

static void addEnemy(Level* level, u8 pos) {

	Enemy* enemy = createEnemy();
	level->enemies.objects[pos] = enemy;
	enemy->sprite = createSprite(enemy);
	level->enemies.current_num_enemies++;
}

static Enemy* createEnemy() {

	Enemy* enemy = MEM_alloc(sizeof(Enemy));
	enemy->health = ALIVE;

	// position & direction
	if (random() % 2) {
		enemy->object.pos.x = MIN_POS_H_PX_F16;
		enemy->object.mov.x = SPEED_H_NORMAL;
	} else {
		enemy->object.pos.x = MAX_POS_H_PX_F16;
		enemy->object.mov.x = -SPEED_H_NORMAL;
	}

	enemy->object.pos.y = randomInRangeFix16(MIN_POS_V_PX_F16, MAX_POS_START_V_PX_F16);

	// V speed
	if (random() % 2) {
		enemy->object.mov.y = SPEED_ZERO;
	} else {
		enemy->object.mov.y = SPEED_V_NORMAL;
	}

	// box
	enemy->object.box.w = ENEMY_01_WIDTH;
	enemy->object.box.h = ENEMY_01_HEIGHT;
	enemy->object.box.pos.x = fix16ToInt(enemy->object.pos.x);
	enemy->object.box.pos.y = fix16ToInt(enemy->object.pos.y);

	startTimer(ENEMY_CREATION_TIMER);
	return enemy;
}

static void enemiesJoin(Level* level) {

	if (level->enemies.current_num_enemies
			< level->enemies.max_num_enemies&& getTimer(ENEMY_CREATION_TIMER, FALSE) > SUBTICKPERSECOND) {

		u8 num_enemies = level->enemies.max_num_enemies;
		u8 idx;
		while (num_enemies--) {
			// find the first empty slot
			Enemy* enemy = level->enemies.objects[num_enemies];
			if (!enemy) {
				idx = num_enemies;
				break;
			}
		}

		addEnemy(level, idx);
	}
}

static void moveEnemy(Enemy* enemy, Level* level) {

	calculateNextMovement(enemy);
	updatePosition(enemy, level);
}

static void calculateNextMovement(Enemy* enemy) {
	// nothing for the moment
}

static void updatePosition(Enemy* enemy, Level* level) {

	// horizontal position
	Box_s16 target = targetBox(enemy->object, ENEMY_01_WIDTH, ENEMY_01_HEIGHT);
	if (crashedIntoPlatform(target, level)) {

		killEnemy(enemy, level, TRUE);
		return;
	}

	if (target.pos.x > MAX_POS_H_PX_S16) {
		enemy->object.pos.x = MIN_POS_H_PX_F16;

	} else if (target.pos.x < MIN_POS_H_PX_S16) {
		enemy->object.pos.x = MAX_POS_H_PX_F16;
	} else {
		enemy->object.pos.x += enemy->object.mov.x;
	}

	enemy->object.pos.y += enemy->object.mov.y;

	// update box
	updateBox(&enemy->object);
}

static u8 crashedIntoPlatform(Box_s16 subject_box, const Level* level) {

	u8 crashed = overlap(subject_box, level->floor->object.box);
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

static Sprite* createSprite(Enemy* enemy) {

	Sprite* enemySprite = SPR_addSprite(&enemy_01_sprite, fix16ToInt(enemy->object.pos.x),
			fix16ToInt(enemy->object.pos.y), TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
	SPR_setAnim(enemySprite, (abs(random())) % 4);
	SPR_setFrame(enemySprite, (abs(random())) % 2);

	if (enemy->object.mov.x > 0) {
		SPR_setHFlip(enemySprite, TRUE);
	}

	return enemySprite;
}

static void updateSprite(Enemy* enemy) {

	if (enemy->health & ALIVE) {
		SPR_setPosition(enemy->sprite, fix16ToInt(enemy->object.pos.x), fix16ToInt(enemy->object.pos.y));
	}
}

static void detectNuclearBomb() {

	if (JOY_readJoypad(JOY_1) & BUTTON_A) {
		nuclear_bomb = TRUE;
	}
}
