/*
 * enemies.c
 *
 *  Created on: Apr 24, 2019
 *      Author: diegomtassis
 */

#include "../inc/enemies.h"

#include "../inc/physics.h"

#include "../res/sprite.h"

#define SPEED_ZERO		FIX16(0)
#define SPEED_H_NORMAL	FIX16(1.5)

static void moveEnemy(Enemy*, Sprite*, const Level*);
static void calculateNextMovement(Enemy*);
static void updatePosition(Enemy*, const Level*);
static void drawEnemies();

Sprite** enemiesSprites;

Enemy** enemies;

void startEnemies(Level* level) {

	u8 num_enemies = level->enemies->max_num_enemies;
	enemies = MEM_alloc(sizeof(Enemy*) * num_enemies);
	enemiesSprites = MEM_alloc(sizeof(Sprite*) * num_enemies);

	Enemy* enemy;
	Sprite* enemySprite;
	while (num_enemies--) {

		// enemy object
		enemy = MEM_alloc(sizeof(Enemy));
		enemy->object.pos.x = FIX16(0);
		enemy->object.pos.y = FIX16( 8 * num_enemies) + 10;
		if (num_enemies % 2) {
			enemy->object.mov.x = FIX16(100);
		} else {
			enemy->object.mov.x = -FIX16(100);
		}
		enemy->alive = TRUE;
		enemies[num_enemies] = enemy;

		// sprite
		enemySprite = SPR_addSprite(&enemy_02_sprite, fix16Int(enemy->object.mov.x), fix16Int(enemy->object.mov.y),
				TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
		SPR_setVisibility(enemySprite, VISIBLE);
		enemiesSprites[num_enemies] = enemySprite;

		level->enemies->current_num_enemies++;
	}
}

void handleEnemies(Level* level) {

	u8 num_enemies = level->enemies->max_num_enemies;
	while (num_enemies--) {
		moveEnemy(enemies[num_enemies], enemiesSprites[num_enemies], level);
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

	enemy->object.pos.x += enemy->object.pos.x;
//	if (target_h.x > MAX_POS_H_PX_F16) {
//		player->object.pos.x = MIN_POS_H_PX_F16;
//
//	} else if (target_h.x < MIN_POS_H_PX_F16) {
//		player->object.pos.x = MAX_POS_H_PX_F16;
//
//	}
}

static void drawEnemies() {

}
