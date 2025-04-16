/*
 * owls.c
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

#define OWL_WIDTH    		16
#define OWL_HEIGHT    	14

#define OWL_DEFAULT_SPEED_H	FIX16(0.9)
#define OWL_DEFAULT_SPEED_V	FIX16(0.6)

#define WAIT_BETWEEN_DIRECTION_CHANGE    	125

static Enemy* createOwl();
static void actOwl(Enemy enemy[static 1], Planet planet[static 1]);
static void releaseOwl(Enemy enemy[static 1]);

const EnemyDefinition owlDefinition = { //
		.type = OWL, //
				.size_t.x = OWL_WIDTH, //
				.size_t.y = OWL_HEIGHT, //
				.createFunc = &createOwl, //
				.actFunc = &actOwl, //
				.releaseFunc = &releaseOwl };

typedef struct {
	u16 mov_counter;
} Owl;

static f16 randomVSpeed();

static Enemy* createOwl() {

	Enemy* enemy = createEnemy(&owlDefinition);

	Owl* owl = MEM_calloc(sizeof *owl);
	owl->mov_counter = WAIT_BETWEEN_DIRECTION_CHANGE;
	enemy->extension = owl;

	// position & movement
	initPosAndMov(enemy, OWL_DEFAULT_SPEED_H, randomVSpeed());

	// box
	initBox(enemy);

	// sprite
	Sprite* enemySprite = SPR_addSprite(&owl_sprite, F16_toInt(enemy->object.pos.x), F16_toInt(enemy->object.pos.y),
			TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
	SPR_setAnim(enemySprite, (abs(random())) % 4);
	SPR_setFrame(enemySprite, (abs(random())) % 2);
	enemy->sprite = enemySprite;

	return enemy;
}

static void actOwl(Enemy enemy[static 1], Planet planet[static 1]) {

	Box_s16 target = targetBox(&enemy->object);

	Owl* owl = enemy->extension;
	owl->mov_counter--;
	if (!owl->mov_counter) {
		enemy->object.mov.y = randomVSpeed();
		owl->mov_counter = WAIT_BETWEEN_DIRECTION_CHANGE;
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

static void releaseOwl(Enemy enemy[static 1]) {

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
		return OWL_DEFAULT_SPEED_V;
	}

	if (i) {
		return -OWL_DEFAULT_SPEED_V;
	}

	return SPEED_ZERO;
}
