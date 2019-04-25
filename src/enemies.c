/*
 * enemies.c
 *
 *  Created on: Apr 24, 2019
 *      Author: diegomtassis
 */

#include "../inc/enemies.h"

#include <maths.h>
#include <memory.h>
#include <sprite_eng.h>
#include <timer.h>
#include <tools.h>
#include <types.h>
#include <vdp_tile.h>

#include "../inc/physics.h"
#include "../res/sprite.h"

#define SPEED_ZERO		FIX16(0)
#define SPEED_H_NORMAL	FIX16(1.5)

#define ENEMY_HEIGHT	16
#define ENEMY_WIDTH		16

#define MIN_POS_H_PX_F16	LEFT_POS_H_PX_F16 - FIX16(-8)
#define MAX_POS_H_PX_F16	RIGHT_POS_H_PX_F16 - FIX16(8)
#define MAX_POS_V_PX_F16	TOP_POS_V_PX_F16

static void moveEnemy(Enemy*, Sprite*, const Level*);
static void calculateNextMovement(Enemy*);
static void updatePosition(Enemy*, const Level*);
static void killEnemy(Enemy*, Sprite*, Level*);
static void drawEnemies();

Sprite** enemiesSprites;

Enemy** enemies;

void startEnemies(Level* level) {

	u8 num_enemies = level->enemies->max_num_enemies;
	enemies = MEM_alloc(sizeof(Enemy*) * num_enemies);
	enemiesSprites = MEM_alloc(sizeof(Sprite*) * num_enemies);

	Enemy* enemy;
	Sprite* enemySprite;
	u32 tick = getTick();

	u8 minVPos = fix16ToInt(TOP_POS_V_PX_F16);
	u8 maxVPos = 256 - minVPos;

	while (num_enemies--) {

		// enemy object
		enemy = MEM_alloc(sizeof(Enemy));
		enemy->object.pos.y = FIX16(((tick + random()) % maxVPos) + minVPos);

		u16 first = random();
		u16 second = random();
		fix16 speed = fix16Div(FIX16(min(first, second)), FIX16(max(first, second)));

		if (num_enemies % 2) {
			enemy->object.pos.x = MIN_POS_H_PX_F16;
			enemy->object.mov.x = speed;
		} else {
			enemy->object.pos.x = MAX_POS_H_PX_F16;
			enemy->object.mov.x = -speed;
		}
		enemy->idx = num_enemies;
		enemies[num_enemies] = enemy;

		// sprite
		enemySprite = SPR_addSprite(&enemy_02_sprite, fix16Int(enemy->object.mov.x), fix16Int(enemy->object.mov.y),
				TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
		SPR_setAnim(enemySprite, (tick + random()) % 4);
		SPR_setFrame(enemySprite, (tick + random()) % 2);
		SPR_setVisibility(enemySprite, VISIBLE);
		enemiesSprites[num_enemies] = enemySprite;

		level->enemies->current_num_enemies++;
	}
}

void handleEnemies(Level* level) {

	u8 num_enemies = level->enemies->max_num_enemies;
	while (num_enemies--) {
		Enemy* enemy = enemies[num_enemies];
		if (enemy) {
			moveEnemy(enemy, enemiesSprites[num_enemies], level);
		}
	}

	drawEnemies();
}

static void moveEnemy(Enemy* enemy, Sprite* sprite, const Level* level) {

	calculateNextMovement(enemy);
	updatePosition(enemy, level);

	SPR_setPosition(sprite, fix16ToInt(enemy->object.pos.x), fix16ToInt(enemy->object.pos.y));
}

static void calculateNextMovement(Enemy* enemy) {

}

static void updatePosition(Enemy* enemy, const Level* level) {

	// horizontal position
	Box_f16 target_h = targetHBox(&enemy->object, ENEMY_WIDTH, ENEMY_HEIGHT);
	if (target_h.x > MAX_POS_H_PX_F16) {
		enemy->object.pos.x = MIN_POS_H_PX_F16;

	} else if (target_h.x < MIN_POS_H_PX_F16) {
		enemy->object.pos.x = MAX_POS_H_PX_F16;

	} else {
		enemy->object.pos.x = target_h.x;
	}

	// vertical position
	// no changes for the moment

}

static void killEnemy(Enemy* enemy, Sprite* sprite, Level* level) {

}

static void drawEnemies() {

}
