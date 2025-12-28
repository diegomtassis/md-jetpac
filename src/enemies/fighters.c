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
#include "../../res/zx.h"

#define FIGHTER_WIDTH    	16
#define FIGHTER_HEIGHT    	7

#define FIGHTER_DEFAULT_SPEED_H	FIX16(0.9)
#define FIGHTER_DEFAULT_SPEED_V	FIX16(0.6)
#define FIGHTER_WAITING_SPEED_V	FIX16(0.5)

#define WAIT_BETWEEN_DIRECTION_CHANGE_WAITING    	40
#define WAIT_BETWEEN_DIRECTION_CHANGE_CHASING    	25
#define MIN_WAITING    	20
#define MAX_WAITING    	450

#define WAITING		0x01
#define CHASING		0x02

#define ANIM_WHITE		0

static Enemy* createFighter(EnemyHostility hostility);
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
	u16 step;
	u16 waiting_counter;
	bool headed_left;
} Fighter;

static f16 chaseVSpeed(Object_f16* enemy_object, Planet planet[static 1]);

static Enemy* createFighter(EnemyHostility hostility) {
	(void)hostility;

	Enemy* enemy = createEnemy(&fighterDefinition);

	Fighter* fighter = MEM_calloc(sizeof *fighter);
	fighter->mov_counter = WAIT_BETWEEN_DIRECTION_CHANGE_CHASING;
	enemy->extension = fighter;

	// position & movement
	if (random() % 4) {
		fighter->step = WAITING;
		fighter->waiting_counter = randomInRangeU16(MIN_WAITING, MAX_WAITING);

		enemy->object.mov.x = 0;
		fighter->headed_left = random() % 2;
		enemy->object.mov.y = random() % 2 ? FIGHTER_WAITING_SPEED_V : -FIGHTER_WAITING_SPEED_V;
		enemy->object.pos.y = randomInRangeFix16(MIN_POS_V_PX_F16, ENEMY_DEFAULT_MAX_POS_START_V_PX_F16);
		enemy->object.pos.x = ENEMY_DEFAULT_MIN_POS_H_PX_F16; // Begin always in the left side

	} else {
		fighter->step = CHASING;
		initPosAndMov(enemy, FIGHTER_DEFAULT_SPEED_H, -FIGHTER_DEFAULT_SPEED_V);
		fighter->headed_left = enemy->object.mov.x < 0;
	}

	// box
	initBox(enemy);

	// sprite
	Sprite* enemySprite = SPR_addSprite(&fighter_sprite, F16_toInt(enemy->object.pos.x),
			F16_toInt(enemy->object.pos.y), TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
	SPR_setAnim(enemySprite, fighter->step & WAITING ? (abs(random())) % 5 : ANIM_WHITE); // the chasing fighters are all white
	SPR_setHFlip(enemySprite, fighter->headed_left);
	enemy->sprite = enemySprite;

	return enemy;
}

static void actFighter(Enemy enemy[static 1], Planet planet[static 1]) {

	Fighter* fighter = enemy->extension;

	fighter->mov_counter--;

	if (fighter->step & WAITING) {

		fighter->waiting_counter--;
		if (fighter->waiting_counter) {
			if (!fighter->mov_counter) {
				enemy->object.mov.y = -enemy->object.mov.y;
				fighter->mov_counter = WAIT_BETWEEN_DIRECTION_CHANGE_WAITING;
			}
		} else {
			fighter->step = CHASING;
			enemy->object.mov.x = fighter->headed_left ? -FIGHTER_DEFAULT_SPEED_H : FIGHTER_DEFAULT_SPEED_H;
			enemy->object.mov.y = chaseVSpeed(&enemy->object, planet);
			SPR_setAnim(enemy->sprite, ANIM_WHITE); // the chasing fighters are all white
			fighter->mov_counter = 1;
		}

	} else { // fighter->step & CHASING
		if (!fighter->mov_counter) {
			enemy->object.mov.y = chaseVSpeed(&enemy->object, planet);
			fighter->mov_counter = WAIT_BETWEEN_DIRECTION_CHANGE_CHASING;
		}
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
		 * A better approach would be to chase the closer jetman, but that requires a more expensive operation.
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
