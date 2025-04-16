/*
 * saucers.c
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

#define SAUCER_WIDTH    	16
#define SAUCER_HEIGHT    	8

#define SAUCER_DEFAULT_SPEED_H	FIX16(0.9)
#define SAUCER_DEFAULT_SPEED_V	FIX16(0.6)

#define WAIT_BETWEEN_DIRECTION_CHANGE    	125

static Enemy* createSaucer();
static void actSaucer(Enemy enemy[static 1], Planet planet[static 1]);
static void releaseSaucer(Enemy enemy[static 1]);

const EnemyDefinition saucerDefinition = { //
		.type = SAUCER, //
				.size_t.x = SAUCER_WIDTH, //
				.size_t.y = SAUCER_HEIGHT, //
				.createFunc = &createSaucer, //
				.actFunc = &actSaucer, //
				.releaseFunc = &releaseSaucer };

typedef struct {
	u16 mov_counter;
} Saucer;

static f16 randomVSpeed();

static Enemy* createSaucer() {

	Enemy* enemy = createEnemy(&saucerDefinition);

	Saucer* saucer = MEM_calloc(sizeof *saucer);
	saucer->mov_counter = WAIT_BETWEEN_DIRECTION_CHANGE;
	enemy->extension = saucer;

	// position & movement
	initPosAndMov(enemy, SAUCER_DEFAULT_SPEED_H, randomVSpeed());

	// box
	initBox(enemy);

	// sprite
	Sprite* enemySprite = SPR_addSprite(&saucer_sprite, F16_toInt(enemy->object.pos.x),
			F16_toInt(enemy->object.pos.y), TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
	SPR_setAnim(enemySprite, (abs(random())) % 4);
	enemy->sprite = enemySprite;

	return enemy;
}

static void actSaucer(Enemy enemy[static 1], Planet planet[static 1]) {

	Box_s16 target = targetBox(&enemy->object);

	Saucer* saucer = enemy->extension;
	saucer->mov_counter--;
	if (!saucer->mov_counter) {
		enemy->object.mov.y = randomVSpeed();
		saucer->mov_counter = WAIT_BETWEEN_DIRECTION_CHANGE;
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

		} else {
			SPR_setHFlip(enemy->sprite, enemy->object.mov.x < 0);
		}
	}

	updatePosition(enemy, target);
}

static void releaseSaucer(Enemy enemy[static 1]) {

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
		return SAUCER_DEFAULT_SPEED_V;
	}

	if (i) {
		return -SAUCER_DEFAULT_SPEED_V;
	}

	return SPEED_ZERO;
}
