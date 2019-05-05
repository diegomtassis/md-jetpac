/*
 * menu.c
 *
 *  Created on: May 5, 2019
 *      Author: diegomtassis
 */

#include "menu.h"

#include <genesis.h>

#include "../inc/printer.h"

#define WAIT_MS_BEFORE 	500
#define WAIT_MS_BETWEEN	250

static void joyEvent(u16 joy, u16 changed, u16 state);

vu8 start = FALSE;

void setUpGame(Game* game, u8 level, u8 lives) {

	VDP_clearPlan(PLAN_A, TRUE);
	VDP_clearPlan(PLAN_B, TRUE);

	VDP_setTextPalette(PAL2);

	start = FALSE;

	Vect2D_u16 pos = { .x = 0, .y = 5 };

	printerOn();
	waitMs(WAIT_MS_BEFORE);

	print("JETPAC GAME SELECTION", &pos);
	waitMs(WAIT_MS_BETWEEN);

	println("", &pos);
	println("", &pos);
	print("ZX MODE - 1 PLAYER GAME", &pos);
	waitMs(WAIT_MS_BETWEEN);

	println("", &pos);
	println("", &pos);
	print("PRESS START BUTTON", &pos);
	waitMs(WAIT_MS_BETWEEN);

	JOY_setEventHandler(joyEvent);

	while (!start) {
		VDP_waitVSync();
	}

	printerOff();

	game->level = level;
	game->lives = lives;
	game->score = 0;
}

static void joyEvent(u16 joy, u16 changed, u16 state) {

	if (BUTTON_START & changed & ~state) {
		start = 1;
	}
}
