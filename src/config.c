/*
 * config.c
 *
 *  Created on: May 5, 2019
 *      Author: diegomtassis
 */

#include "../inc/config.h"

#include <genesis.h>

#include "../inc/planets.h"

#define NUM_MODES		2

static const char* PRESS_START_BUTTON = "PRESS START BUTTON";
static const char* MD_MODE_1_PLAYER = "MD MODE - 1 PLAYER";
static const char* ZX_MODE_1_PLAYER = "ZX MODE - 1 PLAYER";
static const char* JETPAC_GAME_SELECTION = "JETPAC GAME SELECTION";

static void initConfigScreen();
static void clearConfigScreen();

static void displayConfig();
static void displayOption(const char *str, u8 highlighted, u16 x, u16 y);

static void setGameConfig(Game* game);

static void joyEvent(u16 joy, u16 changed, u16 state);

vu8 current_option = 0;
volatile bool start = FALSE;
volatile bool refresh = TRUE;

const V2u16 pos_init = { .x = 6, .y = 8 };

void setUpGame(Game* game) {

	u8 prev_priority = VDP_getTextPriority();

	initConfigScreen();

	current_option = 0;
	start = FALSE;
	refresh = TRUE;

	JOY_setEventHandler(joyEvent);

	do {
		displayConfig(pos_init);
		VDP_waitVSync();
	} while (!start);

	setRandomSeed(getTick());

	setGameConfig(game);
	clearConfigScreen();
	VDP_setTextPriority(prev_priority);
}

static void initConfigScreen() {

	VDP_setPalette(PAL1, palette_grey);
	VDP_setTextPalette(PAL1);
	VDP_setTextPriority(0);

	VDP_setHilightShadow(TRUE);
}

static void clearConfigScreen() {

	VDP_clearTextAreaBG(PLAN_A, 0, 5, 32, 15); // don't clear the HUD
	VDP_setHilightShadow(FALSE);
}

static void displayConfig(V2u16 pos) {

	if (refresh) {

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

static void setGameConfig(Game* game) {

	u8 planet = 0;
	if (current_option == 0) {
		game->config.mode = MODE_ZX;
		game->num_planets = 16;
		game->createPlanet = MEM_alloc(game->num_planets * sizeof(Planet*));
		game->createPlanet[planet++] = createPlanetZX01;
		game->createPlanet[planet++] = createPlanetZX02;
		game->createPlanet[planet++] = createPlanetZX03;
		game->createPlanet[planet++] = createPlanetZX04;
		game->createPlanet[planet++] = createPlanetZX05;
		game->createPlanet[planet++] = createPlanetZX06;
		game->createPlanet[planet++] = createPlanetZX07;
		game->createPlanet[planet++] = createPlanetZX08;
		game->createPlanet[planet++] = createPlanetZX09;
		game->createPlanet[planet++] = createPlanetZX10;
		game->createPlanet[planet++] = createPlanetZX11;
		game->createPlanet[planet++] = createPlanetZX12;
		game->createPlanet[planet++] = createPlanetZX13;
		game->createPlanet[planet++] = createPlanetZX14;
		game->createPlanet[planet++] = createPlanetZX15;
		game->createPlanet[planet++] = createPlanetZX16;
	} else {
		game->config.mode = MODE_MD;
		game->num_planets = 5;
		game->createPlanet = MEM_alloc(game->num_planets * sizeof(Planet*));
		game->createPlanet[planet++] = createPlanetZX01;
		game->createPlanet[planet++] = createPlanetMD01;
		game->createPlanet[planet++] = createPlanetMD02;
		game->createPlanet[planet++] = createPlanetMD03;
		game->createPlanet[planet++] = createPlanetMD04;
	}

	game->lives = 4;
	game->score = 0;
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
