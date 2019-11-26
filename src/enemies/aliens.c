/*
 * aliens.c
 *
 *  Created on: Apr 24, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../../inc/constants.h"
#include "../../inc/elements.h"
#include "../../inc/enemies.h"
#include "../../inc/enemy.h"
#include "../../inc/fwk/commons.h"
#include "../../inc/fwk/physics.h"
#include "../../res/sprite.h"

#define SPEED_ZERO		FIX16_0
#define SPEED_H_NORMAL	FIX16(0.8)
#define SPEED_V_NORMAL	FIX16(0.8)

#define ALIEN_WIDTH    		16
#define ALIEN_HEIGHT    	14

static Enemy* createAlien();
static void actAlien(Enemy enemy[static 1], Level level[static 1]);
static void releaseAlien(Enemy enemy[static 1]);

const EnemyDefinition alienDefinition = { //
		.type = ALIEN, //
				.size_t.x = ALIEN_WIDTH, //
				.size_t.y = ALIEN_HEIGHT, //
				.createFunc = &createAlien, //
				.actFunc = &actAlien, //
				.releaseFunc = &releaseAlien };

typedef struct {
	Enemy* enemy;
} Alien;

static Enemy* createAlien() {

	Enemy* enemy = createEnemy(&alienDefinition);

	// size
	enemy->object.size.x = ALIEN_WIDTH;
	enemy->object.size.y = ALIEN_HEIGHT;

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
		enemy->object.mov.y = SPEED_V_NORMAL;
	} else {
		enemy->object.mov.y = -SPEED_V_NORMAL;
	}

	// box
	enemy->object.box.w = ALIEN_WIDTH;
	enemy->object.box.h = ALIEN_HEIGHT;

	enemy->object.box.pos.x = fix16ToInt(enemy->object.pos.x);
	enemy->object.box.pos.y = fix16ToInt(enemy->object.pos.y);

	// sprite
	Sprite* enemySprite = SPR_addSprite(&alien_sprite, fix16ToInt(enemy->object.pos.x), fix16ToInt(enemy->object.pos.y),
			TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
	SPR_setAnim(enemySprite, (abs(random())) % 4);
	SPR_setFrame(enemySprite, (abs(random())) % 2);
	enemy->sprite = enemySprite;

	if (enemy->object.mov.x > 0) {
		SPR_setHFlip(enemySprite, TRUE);
	}

	return enemy;
}

static void actAlien(Enemy enemy[static 1], Level level[static 1]) {

	Box_s16 target = targetBox(enemy->object);

	if (target.pos.y <= MIN_POS_V_PX_S16 || target.pos.y >= MAX_POS_V_PX_S16) {
		enemy->object.mov.y = -enemy->object.mov.y;
		target = targetBox(enemy->object);

	} else if (crashedIntoPlatform(target, level)) {

		// THIS MUST BE OPTIMIZED

		// change horizontal direction
		enemy->object.mov.x = -enemy->object.mov.x;
		target = targetBox(enemy->object);

		if (crashedIntoPlatform(target, level)) {

			enemy->object.mov.x = -enemy->object.mov.x;
			enemy->object.mov.y = -enemy->object.mov.y;
			target = targetBox(enemy->object);
		}
	}

	updatePosition(enemy, target);
}

static void releaseAlien(Enemy enemy[static 1]) {

	SPR_releaseSprite(enemy->sprite);
	releaseEnemy(enemy);
}
