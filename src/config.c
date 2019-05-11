/*
 * config.c
 *
 *  Created on: May 5, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../inc/config.h"

#define NUM_MODES		2

static const char* PRESS_START_BUTTON = "PRESS START BUTTON";
static const char* MD_MODE_1_PLAYER = "MD MODE - 1 PLAYER";
static const char* ZX_MODE_1_PLAYER = "ZX MODE - 1 PLAYER";
static const char* JETPAC_GAME_SELECTION = "JETPAC GAME SELECTION";

static void displayConfig();
static void displayOption(const char *str, u8 highlighted, u16 x, u16 y);

static void joyEvent(u16 joy, u16 changed, u16 state);

vu8 current_option = 0;
vu8 start = FALSE;
vu8 refresh = TRUE;

void setUpGame(Game* game) {

	u8 prev_priority = VDP_getTextPriority();

	VDP_setPalette(PAL1, palette_grey);
	VDP_setTextPalette(PAL1);
	VDP_setTextPriority(0);

	VDP_setHilightShadow(TRUE);

	start = FALSE;
	refresh = TRUE;

	JOY_setEventHandler(joyEvent);

	do {
		displayConfig();
		VDP_waitVSync();
	} while (!start);

	VDP_clearTextAreaBG(PLAN_A, 0, 5, 32, 15); // don't clear the HUD
	VDP_setHilightShadow(FALSE);

	VDP_setTextPriority(prev_priority);

	game->mode = current_option == 0 ? MODE_ZX : MODE_MD;
	game->level = 1;
	game->lives = 3;
	game->score = 0;
}

static void displayConfig() {

	if (refresh) {

		Vect2D_u16 pos = { .x = 6, .y = 8 };
		VDP_drawText(JETPAC_GAME_SELECTION, pos.x, pos.y);

		pos.y += 4;
		displayOption(ZX_MODE_1_PLAYER, current_option == 0, pos.x, pos.y);

		pos.y += 2;
		displayOption(MD_MODE_1_PLAYER, current_option == 1, pos.x, pos.y);

		pos.y += 4;
		VDP_drawText(PRESS_START_BUTTON, pos.x, pos.y);

		refresh = FALSE;
	}
}

static void displayOption(const char *str, u8 highlighted, u16 x, u16 y) {

	VDP_setTextPriority(highlighted);
	VDP_drawText(str, x, y);
	VDP_setTextPriority(0);
}

static void joyEvent(u16 joy, u16 changed, u16 state) {

	if (BUTTON_DOWN & changed & ~state) {

		current_option++;
		if (current_option == NUM_MODES) {
			current_option = 0;
		}
		refresh = TRUE;
	}

	if (BUTTON_UP & changed & ~state) {
		if (current_option == 0) {
			current_option = NUM_MODES;
		}
		current_option--;
		refresh = TRUE;
	}

	if (BUTTON_START & changed & ~state) {
		start = TRUE;
	}
}
