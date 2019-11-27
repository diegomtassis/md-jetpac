/*
 * falcons.c
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

#define FALCON_WIDTH    	16
#define FALCON_HEIGHT    	14

#define SPEED_ZERO		FIX16_0
#define FALCON_DEFAULT_SPEED_H	FIX16(0.9)
#define FALCON_DEFAULT_SPEED_V	FIX16(0.6)

#define WAIT_BETWEEN_DIRECTION_CHANGE    	125

static Enemy* createFalcon();
static void actFalcon(Enemy enemy[static 1], Level level[static 1]);
static void releaseFalcon(Enemy enemy[static 1]);

const EnemyDefinition falconDefinition = { //
		.type = FALCON, //
				.size_t.x = FALCON_WIDTH, //
				.size_t.y = FALCON_HEIGHT, //
				.createFunc = &createFalcon, //
				.actFunc = &actFalcon, //
				.releaseFunc = &releaseFalcon };

typedef struct {
	u16 mov_counter;
} Falcon;

static f16 randomVSpeed();

static Enemy* createFalcon() {

	Enemy* enemy = createEnemy(&falconDefinition);

	Falcon* falcon = MEM_calloc(sizeof *falcon);
	falcon->mov_counter = WAIT_BETWEEN_DIRECTION_CHANGE;
	enemy->extension = falcon;

	// position & movement
	initPosAndMov(enemy, FALCON_DEFAULT_SPEED_H, randomVSpeed());

	// box
	initBox(enemy);

	// sprite
	Sprite* enemySprite = SPR_addSprite(&falcon_sprite, fix16ToInt(enemy->object.pos.x),
			fix16ToInt(enemy->object.pos.y), TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
	SPR_setAnim(enemySprite, (abs(random())) % 8); // 8 animations
	SPR_setHFlip(enemySprite, enemy->object.mov.x < 0);
	enemy->sprite = enemySprite;

	return enemy;
}

static void actFalcon(Enemy enemy[static 1], Level level[static 1]) {

	Box_s16 target = targetBox(enemy->object);

	Falcon* falcon = enemy->extension;
	falcon->mov_counter--;
	if (!falcon->mov_counter) {
		enemy->object.mov.y = randomVSpeed();
		falcon->mov_counter = WAIT_BETWEEN_DIRECTION_CHANGE;
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

static void releaseFalcon(Enemy enemy[static 1]) {

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
		return FALCON_DEFAULT_SPEED_V;
	}

	if (i) {
		return -FALCON_DEFAULT_SPEED_V;
	}

	return SPEED_ZERO;
}
