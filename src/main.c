/*
 * main.c
 *
 *  Created on: Apr 06, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../inc/config.h"
#include "../inc/game.h"
#include "../inc/hud.h"
#include "../inc/jetpac_file.h"
#include "../inc/printer.h"
#include "../inc/splash_screen.h"
#include "../inc/vdp_utils.h"

#define LOADING_TIME	3000

int main() {

	// default resolution
	VDP_setScreenWidth256();
	VDP_setScreenHeight224();

	// jetpac file
	printDisclaimer();
	JOY_waitPress(JOY_1, BUTTON_BTN);
	clearDisclaimer();

	waitMs(50);

	// splash screen
	showSplashScreen();
	waitMs(LOADING_TIME);
	clearSplashScreen();

	resetTileMemory();
	initHud();

	Game* game = MEM_alloc(sizeof *game);
	while (1) {

		// run game
		setUpGame(game);
		startGame(game);

		registerScore(game->score);

		VDP_waitVSync();
	}

	return 0;
}
