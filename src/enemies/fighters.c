/*
 * fighters.c
 *
 *  Created on: Nov 09, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../../inc/constants.h"
#include "../../inc/elements.h"
#include "../../inc/enemies.h"
#include "../../inc/enemy.h"
#include "../../inc/fwk/commons.h"
#include "../../res/sprite.h"

#define SPEED_ZERO		FIX16_0
#define SPEED_H_NORMAL	FIX16(0.9)
#define SPEED_V_NORMAL	FIX16(0.6)

#define WAIT_BETWEEN_DIRECTION_CHANGE    	125

#define FIGHTER_WIDTH    	16
#define FIGHTER_HEIGHT    	7

static Enemy* createFighter(EnemyDefinition definition[static 1]);
static void actFighter(Enemy enemy[static 1], Level level[static 1]);
static void releaseFighter(Enemy enemy[static 1]);

const EnemyDefinition fighterDefinition = { //
		.type = FIGHTER, //
				.size_t.x = FIGHTER_WIDTH, //
				.size_t.y = FIGHTER_HEIGHT, //
				.createFunc = &createFighter, //
				.actFunc = &actFighter, //
				.releaseFunc = &releaseFighter };

typedef struct {
	u16 mov_counter;
} Fighter;

static f16 randomVSpeed();

static Enemy* createFighter(EnemyDefinition definition[static 1]) {

	Enemy* enemy = createEnemy(definition);

	Fighter* fighter = MEM_calloc(sizeof *fighter);
	fighter->mov_counter = WAIT_BETWEEN_DIRECTION_CHANGE;
	enemy->extension = fighter;

	// size
	enemy->object.size.x = FIGHTER_WIDTH;
	enemy->object.size.y = FIGHTER_HEIGHT;

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
	enemy->object.mov.y = randomVSpeed();

	// box
	enemy->object.box.w = FIGHTER_WIDTH;
	enemy->object.box.h = FIGHTER_HEIGHT;

	enemy->object.box.pos.x = fix16ToInt(enemy->object.pos.x);
	enemy->object.box.pos.y = fix16ToInt(enemy->object.pos.y);

	// sprite
	Sprite* enemySprite = SPR_addSprite(&fighter_sprite, fix16ToInt(enemy->object.pos.x),
			fix16ToInt(enemy->object.pos.y), TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
	SPR_setAnim(enemySprite, (abs(random())) % 8); // 8 animations
	SPR_setHFlip(enemySprite, enemy->object.mov.x < 0);
	enemy->sprite = enemySprite;

	return enemy;
}

static void actFighter(Enemy enemy[static 1], Level level[static 1]) {

	Box_s16 target = targetBox(enemy->object);

	Fighter* fighter = enemy->extension;
	fighter->mov_counter--;
	if (!fighter->mov_counter) {
		enemy->object.mov.y = randomVSpeed();
		fighter->mov_counter = WAIT_BETWEEN_DIRECTION_CHANGE;
	}

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

		} else {
			SPR_setHFlip(enemy->sprite, enemy->object.mov.x < 0);
		}
	}

	updatePosition(enemy, target);
}

static void releaseFighter(Enemy enemy[static 1]) {

	if (enemy->extension) {
		MEM_free(enemy->extension);
		enemy->extension = 0;
	}

	SPR_releaseSprite(enemy->sprite);
	releaseEnemy(enemy);
}

static f16 randomVSpeed() {

	int i = random() % 3;
	if (i == 2) {
		return SPEED_V_NORMAL;
	}

	if (i) {
		return -SPEED_V_NORMAL;
	}

	return SPEED_ZERO;
}
