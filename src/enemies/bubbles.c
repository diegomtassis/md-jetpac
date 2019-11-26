/*
 * bubbles.c
 *
 *  Created on: May 30, 2019
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
#define SPEED_H_NORMAL	FIX16(0.8)
#define SPEED_V_NORMAL	FIX16(0.8)

#define WAIT_BETWEEN_DIRECTION_CHANGE    	125

#define BUBBLE_WIDTH    	16
#define BUBBLE_HEIGHT    	14

static Enemy* createBubble();
static void actBubble(Enemy enemy[static 1], Level level[static 1]);
static void releaseBubble(Enemy enemy[static 1]);

const EnemyDefinition bubbleDefinition = { //
		.type = BUBBLE, //
				.size_t.x = BUBBLE_WIDTH, //
				.size_t.y = BUBBLE_HEIGHT, //
				.createFunc = &createBubble, //
				.actFunc = &actBubble, //
				.releaseFunc = &releaseBubble };

typedef struct {
	u16 mov_counter;
} Bubble;

static f16 randomVSpeed();

static Enemy* createBubble() {

	Enemy* enemy = createEnemy(&bubbleDefinition);

	Bubble* bubble = MEM_calloc(sizeof *bubble);
	bubble->mov_counter = WAIT_BETWEEN_DIRECTION_CHANGE;
	enemy->extension = bubble;

	// size
	enemy->object.size.x = BUBBLE_WIDTH;
	enemy->object.size.y = BUBBLE_HEIGHT;

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
	enemy->object.box.w = BUBBLE_WIDTH;
	enemy->object.box.h = BUBBLE_HEIGHT;

	enemy->object.box.pos.x = fix16ToInt(enemy->object.pos.x);
	enemy->object.box.pos.y = fix16ToInt(enemy->object.pos.y);

	// sprite
	Sprite* enemySprite = SPR_addSprite(&bubble_sprite, fix16ToInt(enemy->object.pos.x),
			fix16ToInt(enemy->object.pos.y), TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
	SPR_setAnim(enemySprite, (abs(random())) % 8); // 8 animations
	enemy->sprite = enemySprite;

	if (enemy->object.mov.x > 0) {
		SPR_setHFlip(enemySprite, TRUE);
	}

	return enemy;
}

static void actBubble(Enemy enemy[static 1], Level level[static 1]) {

	Box_s16 target = targetBox(enemy->object);

	Bubble* bubble = enemy->extension;
	bubble->mov_counter--;
	if (!bubble->mov_counter) {
		enemy->object.mov.y = randomVSpeed();
		bubble->mov_counter = WAIT_BETWEEN_DIRECTION_CHANGE;
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
		}
	}

	updatePosition(enemy, target);
}

static void releaseBubble(Enemy enemy[static 1]) {

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
