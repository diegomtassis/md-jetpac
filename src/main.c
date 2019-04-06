/*
 * main.c
 *
 *  Created on: Apr 06, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../inc/input_handling.h"
#include "../inc/jetpac_file.h"
#include "../inc/splash_screen.h"

int main() {

	showDisclaimer();

	IH_waitForPress(JOY_1, BUTTON_START);

	VDP_clearPlan(PLAN_A, TRUE);
	VDP_clearPlan(PLAN_B, TRUE);

	showSplashScreen();

	while (TRUE) {
		VDP_waitVSync();
	}

	return 0;
}
