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
#include "../../inc/fwk/physics.h"
#include "../../res/sprite.h"

#define FIGHTER_WIDTH    	16
#define FIGHTER_HEIGHT    	7

#define FIGHTER_DEFAULT_SPEED_H	FIX16(0.9)
#define FIGHTER_DEFAULT_SPEED_V	FIX16(0.6)

#define WAIT_BETWEEN_DIRECTION_CHANGE    	25

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

static f16 chaseVSpeed(Object_f16* enemy_object, Planet planet[static 1]);

static Enemy* createFighter() {

	Enemy* enemy = createEnemy(&fighterDefinition);

	Fighter* fighter = MEM_calloc(sizeof *fighter);
	fighter->mov_counter = WAIT_BETWEEN_DIRECTION_CHANGE;
	enemy->extension = fighter;

	// position & movement
	initPosAndMov(enemy, FIGHTER_DEFAULT_SPEED_H, -FIGHTER_DEFAULT_SPEED_V);

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

	Fighter* fighter = enemy->extension;

	fighter->mov_counter--;
	if (!fighter->mov_counter) {
		enemy->object.mov.y = chaseVSpeed(&enemy->object, planet);
		fighter->mov_counter = WAIT_BETWEEN_DIRECTION_CHANGE;
	}

	Box_s16 target = targetBox(&enemy->object);

	if (target.min.y <= MIN_POS_V_PX_S16 || target.min.y >= MAX_POS_V_PX_S16 || crashedIntoPlatform(target, planet)) {

		killEnemy(enemy, planet, TRUE);
		return;
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

static f16 chaseVSpeed(Object_f16* enemy_object, Planet planet[static 1]) {
	/*
	 * Follow the jetman alive. If there are more than one alive, chase one randomly.
	 */
	Jetman* target;
	if (planet->j2 == 0) {
		target = planet->j1;

	} else if (planet->j1 == 0) {
		target = planet->j2;

	} else {
		/*
		 * A smarter approach would be to chase the closer jetman, but that is more expensive to figure out.
		 */
		if (random() % 2) {
			target = planet->j2;
		} else {
			target = planet->j1;
		}
	}

	if (enemy_object->pos.y < target->object.pos.y) {
		return FIGHTER_DEFAULT_SPEED_V;
	}

	return -FIGHTER_DEFAULT_SPEED_V;
}
