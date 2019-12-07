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

#define FIGHTER_WIDTH    	16
#define FIGHTER_HEIGHT    	7

#define FIGHTER_DEFAULT_SPEED_H	FIX16(0.9)
#define FIGHTER_DEFAULT_SPEED_V	FIX16(0.6)

#define WAIT_BETWEEN_DIRECTION_CHANGE    	125

static Enemy* createFighter();
static void actFighter(Enemy enemy[static 1], Planet planet[static 1]);
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

static Enemy* createFighter() {

	Enemy* enemy = createEnemy(&fighterDefinition);

	Fighter* fighter = MEM_calloc(sizeof *fighter);
	fighter->mov_counter = WAIT_BETWEEN_DIRECTION_CHANGE;
	enemy->extension = fighter;

	// position & movement
	initPosAndMov(enemy, FIGHTER_DEFAULT_SPEED_H, randomVSpeed());

	// box
	initBox(enemy);

	// sprite
	Sprite* enemySprite = SPR_addSprite(&fighter_sprite, fix16ToInt(enemy->object.pos.x),
			fix16ToInt(enemy->object.pos.y), TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
	SPR_setAnim(enemySprite, (abs(random())) % 5);
	SPR_setHFlip(enemySprite, enemy->object.mov.x < 0);
	enemy->sprite = enemySprite;

	return enemy;
}

static void actFighter(Enemy enemy[static 1], Planet planet[static 1]) {

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

	} else if (crashedIntoPlatform(target, planet)) {

		// THIS MUST BE OPTIMIZED

		// change horizontal direction
		enemy->object.mov.x = -enemy->object.mov.x;
		target = targetBox(enemy->object);

		if (crashedIntoPlatform(target, planet)) {

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
		return FIGHTER_DEFAULT_SPEED_V;
	}

	if (i) {
		return -FIGHTER_DEFAULT_SPEED_V;
	}

	return SPEED_ZERO;
}
