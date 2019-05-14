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
#include "../inc/hud.h"
#include "../inc/jetman.h"
#include "../inc/level.h"
#include "../inc/level_zx_01.h"
#include "../inc/level_md_01.h"
#include "../inc/physics.h"

static void handleCollisionsBetweenElementsAlive(Level*);
static void handleElementsLeavingScreenUnder(Level*);
static u8 isJetmanAlive(Level*);

static void joyEvent(u16 joy, u16 changed, u16 state);

vu8 paused = FALSE;
vu8 commitSuicide = FALSE;

static const V2u16 game_over_text_pos = { .x = 12, .y = 5 };

void startGame(Game* game) {

	SPR_init();

	Level* current_level = game->createLevel();

	startLevel(current_level);

	startJetman(current_level);
	startEnemies(current_level);
	startSpaceship(current_level);
	initExplosions(current_level);

	SPR_update();

	JOY_setEventHandler(joyEvent);

	u8 jetmanAlive = TRUE;
	u8 gameOver = FALSE;

	while (!gameOver) {

		if (!paused) {

			if (jetmanAlive) {

				if (commitSuicide) {
					killJetman(current_level, TRUE);
					commitSuicide = FALSE;
				}

				jetmanActs(current_level);
				enemiesAct(current_level);
				handleCollisionsBetweenElementsAlive(current_level);
				if (current_level->def.check_bottom) {
					handleElementsLeavingScreenUnder(current_level);
				}

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
						current_level->jetman = 0;
						releaseSpaceship(current_level);
					}
				}
			}

			updateExplosions(current_level);

			gameOver = !game->lives && !current_level->booms.current_num_booms;
			SPR_update();
		}

		VDP_waitVSync();
	}

	VDP_drawText("Game Over", game_over_text_pos.x, game_over_text_pos.y);
	releaseLevel(current_level);
	current_level = 0;

	waitMs(5000);

	SPR_end();
	VDP_clearTextLine(5); // Game over text
	VDP_clearPlan(PLAN_B, TRUE);

	return;
}

static void handleCollisionsBetweenElementsAlive(Level* level) {

	for (u8 enemy_idx = 0; enemy_idx < level->enemies.max_num_enemies; enemy_idx++) {

		Enemy* enemy = level->enemies.objects[enemy_idx];
		if (enemy && (ALIVE & enemy->health) && overlap(level->jetman->object.box, enemy->object.box)) {
			killJetman(level, TRUE);
			killEnemy(enemy, level, TRUE);
			break;
		}
	}
}

static void handleElementsLeavingScreenUnder(Level* level) {

	if ((ALIVE & level->jetman->health) && level->jetman->object.box.pos.y > BOTTOM_POS_V_PX_S16) {
		killJetman(level, FALSE);
	}

	for (u8 enemy_idx = 0; enemy_idx < level->enemies.max_num_enemies; enemy_idx++) {

		Enemy* enemy = level->enemies.objects[enemy_idx];
		if (enemy && (ALIVE & enemy->health) && enemy->object.box.pos.y > BOTTOM_POS_V_PX_S16) {
			killEnemy(enemy, level, FALSE);
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
