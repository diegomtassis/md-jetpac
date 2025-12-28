/*
 * meteorites.c
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
#include "../../res/zx.h"

#define METEORITE_WIDTH		16
#define METEORITE_HEIGHT  	12

#define METEORITE_DEFAULT_SPEED_H	FIX16(1)
#define METEORITE_DEFAULT_SPEED_V	FIX16(0.3)

static Enemy* createMeteorite(EnemyHostility hostility);
static void actMeteorite(Enemy enemy[static 1], Planet planet[static 1]);
static void releaseMeteorite(Enemy enemy[static 1]);

const EnemyDefinition meteoriteDefinition = { //
		.type = METEORITE, //
				.size_t.x = METEORITE_WIDTH, //
				.size_t.y = METEORITE_HEIGHT, //
				.createFunc = &createMeteorite, //
				.actFunc = &actMeteorite, //
				.releaseFunc = &releaseMeteorite };

typedef struct {
	Enemy* enemy;
} Meteorite;

static Enemy* createMeteorite(EnemyHostility hostility) {
	(void)hostility;

	Enemy* enemy = createEnemy(&meteoriteDefinition);

	// position & movement
	initPosAndMov(enemy, //
			METEORITE_DEFAULT_SPEED_H, //
			random() % 2 ? SPEED_ZERO : METEORITE_DEFAULT_SPEED_V);

	// box
	initBox(enemy);

	// sprite
	Sprite* enemySprite = SPR_addSprite(&meteorite_sprite, F16_toInt(enemy->object.pos.x),
			F16_toInt(enemy->object.pos.y), TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
	SPR_setAnim(enemySprite, (abs(random())) % 8);
	enemy->sprite = enemySprite;

	if (enemy->object.mov.x < 0) {
		SPR_setHFlip(enemySprite, TRUE);
	}

	return enemy;
}

static void actMeteorite(Enemy enemy[static 1], Planet planet[static 1]) {

	Box_s16 target = targetBox(&enemy->object);
	if (crashedIntoPlatform(target, planet)) {

		killEnemy(enemy, planet, TRUE);
		return;
	}

	updatePosition(enemy, target);
}

static void releaseMeteorite(Enemy enemy[static 1]) {

	SPR_releaseSprite(enemy->sprite);
	releaseEnemy(enemy);
}
