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

#define ALIEN_WIDTH    		16
#define ALIEN_HEIGHT    	14

#define ALIEN_DEFAULT_SPEED_H	FIX16(0.8)
#define ALIEN_DEFAULT_SPEED_V	FIX16(0.8)

static Enemy* createAlien();
static void actAlien(Enemy enemy[static 1], Planet planet[static 1]);
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

	// position & movement
	initPosAndMov(enemy, //
			ALIEN_DEFAULT_SPEED_H, //
			random() % 2 ? ALIEN_DEFAULT_SPEED_V : -ALIEN_DEFAULT_SPEED_V);

	// box
	initBox(enemy);

	// sprite
	Sprite* enemySprite = SPR_addSprite(&alien_sprite, fix16ToInt(enemy->object.pos.x), fix16ToInt(enemy->object.pos.y),
			TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
	SPR_setAnim(enemySprite, (abs(random())) % 4);
	SPR_setFrame(enemySprite, (abs(random())) % 2);
	enemy->sprite = enemySprite;

	return enemy;
}

static void actAlien(Enemy enemy[static 1], Planet planet[static 1]) {

	Box_s16 target = targetBox(enemy->object);

	if (target.min.y <= MIN_POS_V_PX_S16 || target.min.y >= MAX_POS_V_PX_S16) {
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
		}
	}

	updatePosition(enemy, target);
}

static void releaseAlien(Enemy enemy[static 1]) {

	SPR_releaseSprite(enemy->sprite);
	releaseEnemy(enemy);
}
