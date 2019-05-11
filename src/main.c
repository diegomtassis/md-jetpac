/*
 * main.c
 *
 *  Created on: Apr 06, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../inc/config.h"
#include "../inc/jetpac_file.h"
#include "../inc/game.h"
#include "../inc/printer.h"
#include "../inc/splash_screen.h"

#define LOADING_TIME	3000

u16 maxScore = 0;

int main() {

	// default resolution
	VDP_setScreenWidth256();
	VDP_setScreenHeight224();

	// jetpac file
	printDisclaimer();

	JOY_waitPress(JOY_1, BUTTON_BTN);
	printerOff();
	waitMs(75);

	// jetpac loading...
	showSplashScreen();
	waitMs(LOADING_TIME);

	Game* game = MEM_alloc(sizeof(Game));
	while (1) {

		// run game
		setUpGame(game);
		startGame(game);

		if (game->score > maxScore) {
			maxScore = game->score;
		}

		VDP_waitVSync();
	}

	return 0;
}
