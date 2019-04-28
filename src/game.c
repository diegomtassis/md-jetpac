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

static Level* current_level;

static void updateInfoPanel(Game*);

static void joyEvent(u16 joy, u16 changed, u16 state);

vu8 paused = FALSE;

void startGame(Game* game) {

	current_level = createLevel();
	startLevel(current_level);

	startJetman(current_level);
	startEnemies(current_level);

	JOY_setEventHandler(joyEvent);

	while (game->lives >= 0) {

		if (!paused) {
			handleEnemies(current_level);
			handleJetman(current_level);

			game->score++;
			updateInfoPanel(game);

			SPR_update();
		}

		VDP_waitVSync();
	}

	return;
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
}
