/*
 * main.c
 *
 *  Created on: Apr 06, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../inc/input_handling.h"
#include "../inc/jetpac_file.h"
#include "../inc/levels.h"
#include "../inc/splash_screen.h"

int main() {

	showDisclaimer();

	JOY_waitPress(JOY_1, BUTTON_START);

	showSplashScreen();

	JOY_waitPress(JOY_1, BUTTON_START);

	while (TRUE) {

		runGame();
		VDP_waitVSync();
	}

	return 0;
}
