/*
 * main.c
 *
 *  Created on: Apr 06, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../inc/jetpac_file.h"
#include "../inc/game.h"
#include "../inc/printer.h"
#include "../inc/splash_screen.h"

#define LOADING_TIME	3000

static void setUpGame(Game*, u8, u8);

int main() {

	// default resolution
	VDP_setScreenWidth256();
	VDP_setScreenHeight224();

	// jetpac file
	printDisclaimer();

	JOY_waitPress(JOY_1, BUTTON_BTN);
	printerOff();
	waitMs(75);

	Game* game = MEM_alloc(sizeof(Game));
	while (1) {

		// jetpac loading...
		showSplashScreen();
		waitMs(LOADING_TIME);

		// run game
		setUpGame(game, 1, 3);
		startGame(game);

		VDP_waitVSync();
	}

	return 0;
}

static void setUpGame(Game* game, u8 level, u8 lives) {

	game->level = level;
	game->lives = lives;
	game->score = 0;
}
