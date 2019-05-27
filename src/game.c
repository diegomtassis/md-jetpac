/*
 * game.c
 *
 *  Created on: Apr 20, 2019
 *      Author: diegomtassis
 */

#include "../inc/game.h"

#include <genesis.h>

#include "../inc/config.h"
#include "../inc/elements.h"
#include "../inc/enemies.h"
#include "../inc/spaceship.h"
#include "../inc/explosions.h"
#include "../inc/shooting.h"
#include "../inc/hud.h"
#include "../inc/jetman.h"
#include "../inc/level.h"
#include "../inc/planets.h"
#include "../inc/fwk/physics.h"
//#include "../inc/fwk/logger.h"

static void handleCollisionsBetweenElementsAlive(Level level[static 1]);
static void handleElementsLeavingScreenUnder(Level level[static 1]);
static bool isJetmanAlive(Level level[static 1]);
static bool isMissionFinished(Level level[static 1]);

static void waitForLanding(Level level[static 1]);
static void leavePlanet(Level level[static 1]);

static void joyEvent(u16 joy, u16 changed, u16 state);

volatile bool paused = FALSE;

static const V2u16 game_over_text_pos = { .x = 12, .y = 5 };

void runGame(Game* game) {

	SPR_init();

	bool game_over = FALSE;

	/* when all the levels are passed start again */

	u8 level_number = 0;

	while (!game_over) {

		//	log_memory();

		Level* current_level = game->createLevel[level_number]();
		if (++level_number == game->num_levels) {
			level_number = 0;
		}

		startLevel(current_level);

		startSpaceship(current_level);
		waitForLanding(current_level);

		startJetman(current_level);
		startEnemies(current_level);

		initShots(current_level);
		initExplosions(current_level);

		SPR_update();
		VDP_waitVSync();

		JOY_setEventHandler(joyEvent);

		bool jetman_alive = TRUE;
		bool mission_finished = FALSE;

		while (!game_over && !mission_finished) {

			if (!paused) {

				if (jetman_alive) {

					jetmanActs(current_level);
					enemiesAct(current_level);
					handleCollisionsBetweenElementsAlive(current_level);
					if (current_level->def.mind_bottom) {
						handleElementsLeavingScreenUnder(current_level);
					}

					handleSpaceship(current_level);

					jetman_alive = isJetmanAlive(current_level);
					if (!jetman_alive) {
						dropIfGrabbed(current_level->spaceship);
						game->lives--;
					}

					game->score++;
					updateHud(game);

				} else {

					// Smart dying, wait for explosions to finish
					if (!current_level->booms.count) {

						waitMs(100);

						releaseEnemies(current_level);
						if (game->lives > 0) {
							resetJetman(current_level);
							startEnemies(current_level);
							jetman_alive = TRUE;
						}
					}
				}

				updateShots(current_level);
				updateExplosions(current_level);

				mission_finished = jetman_alive && isMissionFinished(current_level);
				game_over = !game->lives && !current_level->booms.count;
				SPR_update();
			}

			VDP_waitVSync();
		}

		if (mission_finished) {
			SPR_setVisibility(current_level->jetman->sprite, HIDDEN);
			leavePlanet(current_level);
			waitMs(500);
		} else {
			VDP_drawText("Game Over", game_over_text_pos.x, game_over_text_pos.y);
			waitMs(1000);
		}

		releaseExplosions(current_level);
		releaseShots(current_level);
		releaseEnemies(current_level);
		releaseJetman(current_level);
		releaseSpaceship(current_level);
		releaseLevel(current_level);
		current_level = 0;

		SPR_update();

		VDP_clearTextLine(game_over_text_pos.y); // Game over text
		VDP_clearPlan(PLAN_B, TRUE);
	}

	SPR_end();

	return;
}

void releaseGame(Game* game) {

	if (!game) {
		return;
	}

	for (int idx = 0; idx < game->num_levels; ++idx) {
		if (game->createLevel[idx]) {
			MEM_free(game->createLevel[idx]);
			game->createLevel[idx] = 0;
		}
	}
	MEM_free(game->createLevel);
	game->createLevel = 0;

	MEM_free(game);
}

static void handleCollisionsBetweenElementsAlive(Level level[static 1]) {

	for (u8 enemy_idx = 0; enemy_idx < level->enemies.size; enemy_idx++) {

		Enemy* enemy = level->enemies.e[enemy_idx];
		if (enemy && (ALIVE & enemy->health)) {

			// enemy & jetman
			if (overlap(level->jetman->object.box, enemy->object.box)) {
				killJetman(level, TRUE);
				killEnemy(enemy, level, TRUE);
				break;
			}

			// enemy & shot
			if (checkHit(enemy->object.box, level)) {
				killEnemy(enemy, level, TRUE);
			}
		}
	}
}

static void handleElementsLeavingScreenUnder(Level level[static 1]) {

	if ((ALIVE & level->jetman->health) && level->jetman->object.box.pos.y > BOTTOM_POS_V_PX_S16) {
		killJetman(level, FALSE);
	}

	for (u8 enemy_idx = 0; enemy_idx < level->enemies.size; enemy_idx++) {

		Enemy* enemy = level->enemies.e[enemy_idx];
		if (enemy && (ALIVE & enemy->health) && enemy->object.box.pos.y > BOTTOM_POS_V_PX_S16) {
			killEnemy(enemy, level, FALSE);
		}
	}
}

static bool isJetmanAlive(Level level[static 1]) {

	return ALIVE & level->jetman->health;
}

static bool isMissionFinished(Level level[static 1]) {

	return (level->spaceship->step == READY) && shareBase(level->jetman->object.box, level->spaceship->base_object->box);
}

static void waitForLanding(Level level[static 1]) {

	while (level->spaceship->step == LANDING) {

		handleSpaceship(level);

		SPR_update();
		VDP_waitVSync();
	}
}

static void leavePlanet(Level level[static 1]) {

	launch(level->spaceship);
	do {
		// keep life going while the orbiter lifts
		handleSpaceship(level);
		enemiesAct(level);
		updateExplosions(level);

		SPR_update();
		VDP_waitVSync();

	} while (level->spaceship->step == LIFTING);
}

static void joyEvent(u16 joy, u16 changed, u16 state) {

	if (BUTTON_START & changed & ~state) {
		paused ^= 1;
	}
}
