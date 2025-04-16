/*
 * crosses.c
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
#include "../../res/zx.h"

#define CROSS_WIDTH    		15
#define CROSS_HEIGHT    	15

#define CROSS_DEFAULT_SPEED_H	FIX16(0.9)
#define CROSS_DEFAULT_SPEED_V	FIX16(0.6)

#define WAIT_BETWEEN_DIRECTION_CHANGE    	125

static Enemy* createCross();
static void actCross(Enemy enemy[static 1], Planet planet[static 1]);
static void releaseCross(Enemy enemy[static 1]);

const EnemyDefinition crossDefinition = { //
		.type = CROSS, //
				.size_t.x = CROSS_WIDTH, //
				.size_t.y = CROSS_HEIGHT, //
				.createFunc = &createCross, //
				.actFunc = &actCross, //
				.releaseFunc = &releaseCross };

typedef struct {
	u16 mov_counter;
} Cross;

static f16 randomVSpeed();

static Enemy* createCross() {

	Enemy* enemy = createEnemy(&crossDefinition);

	Cross* cross = MEM_calloc(sizeof *cross);
	cross->mov_counter = WAIT_BETWEEN_DIRECTION_CHANGE;
	enemy->extension = cross;

	// position & movement
	initPosAndMov(enemy, CROSS_DEFAULT_SPEED_H, randomVSpeed());

	// box
	initBox(enemy);

	// sprite
	Sprite* enemySprite = SPR_addSprite(&cross_sprite, F16_toInt(enemy->object.pos.x),
			F16_toInt(enemy->object.pos.y), TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
	SPR_setAnim(enemySprite, (abs(random())) % 4);
	enemy->sprite = enemySprite;

	return enemy;
}

static void actCross(Enemy enemy[static 1], Planet planet[static 1]) {

	Box_s16 target = targetBox(&enemy->object);

	Cross* cross = enemy->extension;
	cross->mov_counter--;
	if (!cross->mov_counter) {
		enemy->object.mov.y = randomVSpeed();
		cross->mov_counter = WAIT_BETWEEN_DIRECTION_CHANGE;
	}

	if (target.min.y <= MIN_POS_V_PX_S16 || target.min.y >= MAX_POS_V_PX_S16) {
		enemy->object.mov.y = -enemy->object.mov.y;
		target = targetBox(&enemy->object);

	} else if (crashedIntoPlatform(target, planet)) {

		// THIS MUST BE OPTIMIZED

		// change horizontal direction
		enemy->object.mov.x = -enemy->object.mov.x;
		target = targetBox(&enemy->object);

		if (crashedIntoPlatform(target, planet)) {

			enemy->object.mov.x = -enemy->object.mov.x;
			enemy->object.mov.y = -enemy->object.mov.y;
			target = targetBox(&enemy->object);
		}
	}

	updatePosition(enemy, target);
}

static void releaseCross(Enemy enemy[static 1]) {

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
		return CROSS_DEFAULT_SPEED_V;
	}

	if (i) {
		return -CROSS_DEFAULT_SPEED_V;
	}

	return SPEED_ZERO;
}
