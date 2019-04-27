/*
 * enemies.c
 *
 *  Created on: Apr 24, 2019
 *      Author: diegomtassis
 */

#include "../inc/enemies.h"

#include <genesis.h>

#include "../inc/commons.h"
#include "../inc/physics.h"
#include "../res/sprite.h"

#define SPEED_ZERO		FIX16(0)
#define SPEED_H_NORMAL	FIX16(1.5)
#define SPEED_V_NORMAL	FIX16(0.2)

#define ENEMY_HEIGHT	16
#define ENEMY_WIDTH		16

#define MIN_POS_H_PX_F16	LEFT_POS_H_PX_F16 - FIX16(-8)
#define MAX_POS_H_PX_F16	RIGHT_POS_H_PX_F16 - FIX16(8)
#define MIN_POS_V_PX_F16	TOP_POS_V_PX_F16
#define MAX_POS_V_PX_F16	BOTTOM_POS_V_PX_F16 - FIX16(8)

static Enemy* createEnemy();
static void moveEnemy(Enemy*, Sprite*, const Level*);
static void calculateNextMovement(Enemy*);
static void updatePosition(Enemy*, const Level*);

static fix16 crashedV(Box_f16 subject_box, const Level* level);

Sprite** enemiesSprites;

Enemy** enemies;

u32 lastCreatedEnemy;

void startEnemies(Level* level) {

	enemies = MEM_alloc(sizeof(Enemy*) * level->enemies->max_num_enemies);
	enemiesSprites = MEM_alloc(sizeof(Sprite*) * level->enemies->max_num_enemies);
	u8 num_enemies = level->enemies->max_num_enemies / 2; // start with half the maximum enemies

	Sprite* enemySprite;
	u32 tick = getTick();
	setRandomSeed(tick);

	while (num_enemies--) {

		// enemy object
		enemies[num_enemies] = createEnemy();

		// sprite
		enemySprite = SPR_addSprite(&enemy_02_sprite, 0, 0, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
		SPR_setAnim(enemySprite, (tick + abs(random())) % 4);
		SPR_setFrame(enemySprite, (tick + abs(random())) % 2);
		SPR_setVisibility(enemySprite, VISIBLE);
		enemiesSprites[num_enemies] = enemySprite;

		level->enemies->current_num_enemies++;
	}
}

void handleEnemies(Level* level) {

	u8 num_enemies = level->enemies->max_num_enemies;
	while (num_enemies--) {
		Enemy* enemy = enemies[num_enemies];
		if (enemy) {
			moveEnemy(enemy, enemiesSprites[num_enemies], level);

			// get rid of dead bodies
			if (!enemy->alive) {
				MEM_free(enemy);
				enemies[num_enemies] = NULL;
				level->enemies->current_num_enemies--;
			}
		}
	}
}

static void moveEnemy(Enemy* enemy, Sprite* sprite, const Level* level) {

	calculateNextMovement(enemy);
	updatePosition(enemy, level);

	if (enemy->alive) {
		SPR_setPosition(sprite, fix16ToInt(enemy->object.pos.x), fix16ToInt(enemy->object.pos.y));
	} else {
		SPR_releaseSprite(sprite);
	}
}

static void calculateNextMovement(Enemy* enemy) {
	// nothing for the moment
}

static void updatePosition(Enemy* enemy, const Level* level) {

	// horizontal position
	Box_f16 target_h = targetHBox(&enemy->object, ENEMY_WIDTH, ENEMY_HEIGHT);
	if (target_h.x > MAX_POS_H_PX_F16) {
		enemy->object.pos.x = MIN_POS_H_PX_F16;

	} else if (target_h.x < MIN_POS_H_PX_F16) {
		enemy->object.pos.x = MAX_POS_H_PX_F16;

	} else {
		enemy->object.pos.x = target_h.x;
	}

	// vertical position
	Box_f16 target_v = targetVBox(&enemy->object, ENEMY_WIDTH, ENEMY_HEIGHT);
	fix16 crashed = crashedV(target_v, level);
	if (crashed) {
		enemy->alive = FALSE;
	} else {
		enemy->object.pos.y = target_v.y;
	}
}

static Enemy* createEnemy() {

	Enemy* enemy = MEM_alloc(sizeof(Enemy));
	enemy->object.pos.x = MIN_POS_H_PX_F16;
	enemy->object.pos.y = randomInRangeFix16(MIN_POS_V_PX_F16, MAX_POS_V_PX_F16);

	// direction
	if (random() % 2) {
		enemy->object.mov.x = SPEED_H_NORMAL;
	} else {
		enemy->object.mov.x = -SPEED_H_NORMAL;
	}

	// V speed
	if (random() % 2) {
		enemy->object.mov.y = SPEED_ZERO;
	} else {
		enemy->object.mov.y = SPEED_V_NORMAL;
	}

	lastCreatedEnemy = getTick();

	enemy->alive = TRUE;

	return enemy;
}

static fix16 crashedV(Box_f16 subject_box, const Level* level) {

	fix16 crashed = hitAbove(subject_box, *level->floor->object.box);
	if (crashed) {
		return crashed;
	}

	for (u8 i = 0; i < level->num_platforms; i++) {
		crashed = hitAbove(subject_box, *level->platforms[i]->object.box);
		if (crashed) {
			return crashed;
		}
	}

	return 0;
}
