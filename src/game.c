/*
 * game.c
 *
 *  Created on: Apr 20, 2019
 *      Author: diegomtassis
 */

#include "../inc/game.h"

#include <genesis.h>

#include "../inc/elements.h"
#include "../inc/enemies.h"
#include "../inc/jetman.h"
#include "../inc/levels.h"
#include "../inc/level_00.h"
#include "../inc/level_01.h"

static void handleCollisionsBetweenElementsAlive(Level*);
static u8 isJetmanAlive(Level*);
static void updateInfoPanel(Game*);

static void joyEvent(u16 joy, u16 changed, u16 state);

vu8 paused = FALSE;
vu8 killJetman = FALSE;

void startGame(Game* game) {

	SPR_init(50, 256, 256);

	Level* current_level = createLevel00();
	startLevel(current_level);

	startJetman(current_level);
	startEnemies(current_level);

	SPR_update();

	JOY_setEventHandler(joyEvent);

	while (game->lives > 0) {

		VDP_clearText(15, 15, 10);

		if (!paused) {

			jetmanActs(current_level);
			enemiesAct(current_level);

			handleCollisionsBetweenElementsAlive(current_level);

			if (killJetman) {
				current_level->jetman->alive = FALSE;
				killJetman = FALSE;
			}

			if (!isJetmanAlive(current_level)) {

				releaseAllEnemies(current_level);
				game->lives--;
				if (game->lives > 0) {
					resetJetman(current_level);
					startEnemies(current_level);
				} else {
					releaseJetman(current_level->jetman);
				}
			} else {
				releaseDeadEnemies(current_level);
			}

			game->score++;
			updateInfoPanel(game);

			SPR_update();
		}

		VDP_waitVSync();
	}

	VDP_drawText("Game Over", 12, 5);
	releaseLevel(current_level);
	current_level = NULL;

	SPR_end();

	waitMs(5000);

	return;
}

static void handleCollisionsBetweenElementsAlive(Level* level) {

	u8 num_enemies = level->enemies.current_num_enemies;
	u8 current_enemy = 0;
	u8 idx = 0;

	while (current_enemy < num_enemies) {

		Enemy* enemy = level->enemies.objects[idx++];
		if (enemy) {
			current_enemy++;
			if (enemy->alive && overlap(level->jetman->object.box, enemy->object.box)) {
				level->jetman->alive = FALSE;
				break;
			}
		}
	}
}

static u8 isJetmanAlive(Level* level) {

	return level->jetman->alive;
}

static void updateInfoPanel(Game* game) {

// lives
	char lives[2];
	uint16ToStr(game->lives, lives, 1);
	VDP_drawText(lives, 8, 2);

// score
	char score[6];
	sprintf(score, "%06d", game->score);
	VDP_drawText(score, 1, 3);
}

static void joyEvent(u16 joy, u16 changed, u16 state) {

	if (BUTTON_START & changed & ~state) {
		paused ^= 1;
	}

	if (BUTTON_C & changed & ~state) {
		killJetman = TRUE;
	}
}
