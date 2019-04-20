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

int main() {

	// default resolution
	VDP_setScreenWidth256();
	VDP_setScreenHeight224();

	// initialization area
	SPR_init(16, 256, 256);

	// jetpac file
	printDisclaimer();

	JOY_waitPress(JOY_1, BUTTON_BTN);
	printerOff();
	waitMs(75);

	// jetpac loading...
	showSplashScreen();
	waitMs(LOADING_TIME);

	// run game
	startGame();

	return 0;
}
