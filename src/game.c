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
#include "../inc/explosions.h"
#include "../inc/hud.h"
#include "../inc/jetman.h"
#include "../inc/levels.h"
#include "../inc/level_zx_01.h"
#include "../inc/level_md_01.h"
#include "../inc/physics.h"

static void handleCollisionsBetweenElementsAlive(Level*);
static u8 isJetmanAlive(Level*);

static void joyEvent(u16 joy, u16 changed, u16 state);

vu8 paused = FALSE;
vu8 commitSuicide = FALSE;

void startGame(Game* game) {

	SPR_init(50, 256, 256);

	Level* current_level = game->createLevel();

	startLevel(current_level);

	startJetman(current_level);
	startEnemies(current_level);
	initExplosions(current_level);

	SPR_update();

	JOY_setEventHandler(joyEvent);

	u8 jetmanAlive = TRUE;
	u8 gameOver = FALSE;

	while (!gameOver) {

		if (!paused) {

			if (jetmanAlive) {

				if (commitSuicide) {
					killJetman(current_level);
					commitSuicide = FALSE;
				}

				jetmanActs(current_level);
				enemiesAct(current_level);
				handleCollisionsBetweenElementsAlive(current_level);

				jetmanAlive = isJetmanAlive(current_level);
				if (jetmanAlive) {
					releaseDeadEnemies(current_level);
				} else {
					game->lives--;
				}

				game->score++;
				updateHud(game);

			} else {

				// Smart dying, wait for explosions to finish
				if (!current_level->booms.current_num_booms) {

					waitMs(100);

					releaseAllEnemies(current_level);
					if (game->lives > 0) {
						resetJetman(current_level);
						startEnemies(current_level);
						jetmanAlive = TRUE;
					} else {
						releaseJetman(current_level->jetman);
					}
				}
			}

			updateExplosions(current_level);

			gameOver = !game->lives && !current_level->booms.current_num_booms;
			SPR_update();
		}

		VDP_waitVSync();
	}

	VDP_drawText("Game Over", 12, 5);
	releaseLevel(current_level);
	current_level = NULL;

	waitMs(5000);

	SPR_end();
	VDP_clearTextLine(5);
	VDP_clearPlan(PLAN_B, TRUE);

	return;
}

static void handleCollisionsBetweenElementsAlive(Level* level) {

	for (u8 enemy_idx = 0; enemy_idx < level->enemies.max_num_enemies; enemy_idx++) {

		Enemy* enemy = level->enemies.objects[enemy_idx];
		if (enemy && (ALIVE & enemy->health) && overlap(level->jetman->object.box, enemy->object.box)) {
			killJetman(level);
			killEnemy(enemy, level);
			break;
		}
	}
}

static u8 isJetmanAlive(Level* level) {

	return ALIVE & level->jetman->health;
}

static void joyEvent(u16 joy, u16 changed, u16 state) {

	if (BUTTON_START & changed & ~state) {
		paused ^= 1;
	}

	if (BUTTON_C & changed & ~state) {
		commitSuicide = TRUE;
	}
}
