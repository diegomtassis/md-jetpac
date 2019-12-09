/*
 * main.c
 *
 *  Created on: Apr 06, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../inc/config.h"
#include "../inc/fwk/vdp_utils.h"
#include "../inc/fwk/commons.h"
#include "../inc/game.h"
#include "../inc/hud.h"
#include "../inc/jetpac_file.h"
#include "../inc/splash_screen.h"
//#include "../inc/fwk/logger.h"

#define LOADING_TIME	3000

int main() {

	// default resolution
	VDP_setScreenWidth256();
	VDP_setScreenHeight224();

//	// jetpac file
//	printDisclaimer();
//	JOY_waitPress(JOY_1, BUTTON_BTN);
//	clearDisclaimer();
//
//	waitMs(50);
//
//	// splash screen
//	showSplashScreen();
//	waitMs(LOADING_TIME);
//	clearSplashScreen();

	resetTileMemory();
	initHud();

	while (1) {

		//	log_memory();

		Config config = setUpGame();
		GameResult result = runGame(&config);

		registerScore(result.p1_score);
		registerScore(result.p2_score);

		VDP_waitVSync();
	}

	return 0;
}
