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
#include "../../inc/fwk/commons.h"
#include "../../res/sprite.h"

#define SPEED_ZERO		FIX16_0
#define SPEED_H_NORMAL	FIX16(1)
#define SPEED_V_NORMAL	FIX16(0.3)

void growAlien(Enemy* enemy) {

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
		enemy->object.mov.y = SPEED_ZERO;
	} else {
		enemy->object.mov.y = SPEED_V_NORMAL;
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
}

void actAlien(Enemy enemy[static 1]) {

	return;
}

void releaseAlien(Enemy enemy[static 1]) {

	return;
}
