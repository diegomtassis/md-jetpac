/*
 * main.c
 *
 *  Created on: Apr 06, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../inc/game_config.h"
#include "../inc/elements.h"
#include "../inc/fwk/printer.h"
#include "../inc/fwk/vdp_utils.h"
#include "../inc/game.h"
#include "../inc/hud.h"
#include "../inc/jetpac_file.h"
#include "../inc/splash_screen.h"

//#include "../inc/fwk/logger.h"

#define LOADING_TIME	3000

int main(bool hard) {

	// default resolution
	VDP_setScreenWidth256();
	VDP_setScreenHeight224();

	initPrinter();

	if (hard) {

		// jetpac file
		// VDP_setScreenWidth320();
		// printDisclaimer();
		// JOY_waitPress(JOY_1, BUTTON_BTN);
		// clearDisclaimer();
		VDP_setScreenWidth256();

		waitMs(50);

		// splash screen
		// showSplashScreen();
		// waitMs(LOADING_TIME);
		// clearSplashScreen();
	}

	resetTileMemory();
	initHud();

	CONFIG_init();

	while (1) {

		// log_memory();
		CONFIG_setUp();

		GameResult result = runGame(&config);

		registerScore(result.p1_score);
		registerScore(result.p2_score);

		SYS_doVBlankProcess();
	}

	return 0;
}
