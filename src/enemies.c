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

#include "../inc/enemy_01.h"

#define SPEED_ZERO		FIX16(0)
#define SPEED_H_NORMAL	FIX16(1)
#define SPEED_V_NORMAL	FIX16(0.3)

#define MIN_POS_H_PX_F16	LEFT_POS_H_PX_F16 - FIX16(-8)
#define MAX_POS_H_PX_F16	RIGHT_POS_H_PX_F16 - FIX16(8)
#define MIN_POS_V_PX_F16	TOP_POS_V_PX_F16
#define MAX_POS_V_PX_F16	BOTTOM_POS_V_PX_F16 - FIX16(16)

#define LAST_ENEMY_CREATION_TIMER 0

static Enemy* createEnemy();
static void moveEnemy(Enemy*, Sprite*, const Level*);
static void calculateNextMovement(Enemy*);
static void updatePosition(Enemy*, const Level*);
static fix16 crashedHIntoPlatform(Box_f16 subject_box, const Level* level);
static fix16 crashedVIntoPlatform(Box_f16 subject_box, const Level* level);
static Sprite* createSprite(Enemy* enemy);
static void updateSprite(Enemy* enemy, Sprite* sprite);

static void detectExplosion();

Enemy** enemies;
Sprite** enemiesSprites;

u8 explode_all;

void startEnemies(Level* level) {

	enemies = MEM_alloc(sizeof(Enemy*) * level->enemies->max_num_enemies);
	enemiesSprites = MEM_alloc(sizeof(Sprite*) * level->enemies->max_num_enemies);
	u8 num_enemies = level->enemies->max_num_enemies / 2; // start with half the maximum enemies

	setRandomSeed(getTick());

	Enemy* enemy;
	while (num_enemies--) {
		// enemy object
		enemy = createEnemy();
		enemies[num_enemies] = enemy;

		// sprite
		enemiesSprites[num_enemies] = createSprite(enemy);
		level->enemies->current_num_enemies++;
	}
}

void handleEnemies(Level* level) {

	detectExplosion();

	u8 num_enemies = level->enemies->max_num_enemies;

	while (num_enemies--) {

		Enemy* enemy = enemies[num_enemies];
		if (enemy) {

			Sprite* sprite = enemiesSprites[num_enemies];
			if (explode_all) {
				enemy->alive = FALSE;
			} else {
				moveEnemy(enemy, sprite, level);
			}

			updateSprite(enemy, sprite);

			// get rid of dead bodies
			if (!enemy->alive) {
				MEM_free(enemy);
				enemies[num_enemies] = NULL;
				level->enemies->current_num_enemies--;
			}
		}
	}

	if (level->enemies->current_num_enemies < level->enemies->max_num_enemies
			&& getTimer(LAST_ENEMY_CREATION_TIMER, FALSE) > SUBTICKPERSECOND * 2) {

		u8 num_enemies = level->enemies->max_num_enemies;
		u8 idx;
		while (num_enemies--) {
			// find the first empty slot
			Enemy* enemy = enemies[num_enemies];
			if (!enemy) {
				idx = num_enemies;
				break;
			}
		}

		Enemy* enemy = createEnemy();
		enemies[idx] = enemy;
		enemiesSprites[idx] = createSprite(enemy);
		level->enemies->current_num_enemies++;
	}

	explode_all = FALSE;
}

static Enemy* createEnemy() {

	Enemy* enemy = MEM_alloc(sizeof(Enemy));
	enemy->alive = TRUE;

	// position & direction
	if (random() % 2) {
		enemy->object.pos.x = MIN_POS_H_PX_F16;
		enemy->object.mov.x = SPEED_H_NORMAL;
	} else {
		enemy->object.pos.x = MAX_POS_H_PX_F16;
		enemy->object.mov.x = -SPEED_H_NORMAL;
	}
	enemy->object.pos.y = randomInRangeFix16(MIN_POS_V_PX_F16, MAX_POS_V_PX_F16);

	// V speed
	if (random() % 2) {
		enemy->object.mov.y = SPEED_ZERO;
	} else {
		enemy->object.mov.y = SPEED_V_NORMAL;
	}

	startTimer(LAST_ENEMY_CREATION_TIMER);
	return enemy;
}

static void moveEnemy(Enemy* enemy, Sprite* sprite, const Level* level) {

	calculateNextMovement(enemy);
	updatePosition(enemy, level);
}

static void calculateNextMovement(Enemy* enemy) {
	// nothing for the moment
}

static void updatePosition(Enemy* enemy, const Level* level) {

	// horizontal position
	Box_f16 target_h = targetHBox(&enemy->object, ENEMY_01_WIDTH, ENEMY_01_HEIGHT);
	if (target_h.x > MAX_POS_H_PX_F16) {
		enemy->object.pos.x = MIN_POS_H_PX_F16;

	} else if (target_h.x < MIN_POS_H_PX_F16) {
		enemy->object.pos.x = MAX_POS_H_PX_F16;

	} else if (crashedHIntoPlatform(target_h, level)) {
		enemy->alive = FALSE;
	} else {
		enemy->object.pos.x = target_h.x;
	}

	// vertical position
	Box_f16 target_v = targetVBox(&enemy->object, ENEMY_01_WIDTH, ENEMY_01_HEIGHT);
	if (crashedVIntoPlatform(target_v, level)) {
		enemy->alive = FALSE;
	} else {
		enemy->object.pos.y = target_v.y;
	}
}

static fix16 crashedHIntoPlatform(Box_f16 subject_box, const Level* level) {

	fix16 crashed;
	for (u8 i = 0; i < level->num_platforms; i++) {
		Box_f16 platform_box = *level->platforms[i]->object.box;
		crashed = hitLeft(subject_box, platform_box) //
		|| hitRight(subject_box, platform_box);
		if (crashed) {
			return crashed;
		}
	}

	return 0;
}

static fix16 crashedVIntoPlatform(Box_f16 subject_box, const Level* level) {

	fix16 crashed = hitAbove(subject_box, *level->floor->object.box);
	if (crashed) {
		return crashed;
	}

	for (u8 i = 0; i < level->num_platforms; i++) {
		Box_f16 platform_box = *level->platforms[i]->object.box;
		crashed = hitAbove(subject_box, platform_box) //
		|| hitUnder(subject_box, platform_box);
		if (crashed) {
			return crashed;
		}
	}

	return 0;
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

static void updateSprite(Enemy* enemy, Sprite* sprite) {

	if (enemy->alive) {
		SPR_setPosition(sprite, fix16ToInt(enemy->object.pos.x), fix16ToInt(enemy->object.pos.y));
	} else {
		SPR_releaseSprite(sprite);
	}
}

static void detectExplosion() {

	if (JOY_readJoypad(JOY_1) & BUTTON_A) {
		explode_all = TRUE;
	}
}
