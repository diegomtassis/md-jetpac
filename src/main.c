/*
 * main.c
 *
 *  Created on: Apr 06, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../inc/jetpac_file.h"
#include "../inc/levels.h"
#include "../inc/printer.h"
#include "../inc/splash_screen.h"

int main() {

	// Default resolution
	VDP_setScreenWidth256();
	VDP_setScreenHeight224();

	// Initializing area
	SPR_init(16, 256, 256);

	// Jetpac file
	printDisclaimer();

	JOY_waitPress(JOY_1, BUTTON_START);
	printerOff();

	showSplashScreen();

	JOY_waitPress(JOY_1, BUTTON_START);

	while (TRUE) {

		runGame();
		VDP_waitVSync();
	}

	return 0;
}
