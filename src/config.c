/*
 * config.c
 *
 *  Created on: May 5, 2019
 *      Author: diegomtassis
 */

#include "../inc/config.h"

#include <genesis.h>

#include "../inc/fwk/commons.h"
#include "../inc/elements.h"
#include "../inc/planets.h"

static const char* JETPAC_GAME_SELECTION = "JETPAC GAME SELECTION";
static const char* TEXT_MODE = "MODE";
static const char* TEXT_ZX = "ZX";
static const char* TEXT_MD = "MD";
static const char* TEXT_PLAYERS = "PLAYERS";
static const char* TEXT_ONE_PLAYER = "1";
static const char* TEXT_TWO_PLAYERS = "2";
static const char* TEXT_DIFFICULTY = "DIFFICULTY";
static const char* TEXT_EASY = "EASY";
static const char* TEXT_NORMAL = "NORMAL";
static const char* TEXT_HARD = "HARD";
static const char* TEXT_MANIAC = "MANIAC";
static const char* PRESS_START_BUTTON = "START GAME";

static const u16 BUTTON_ABC = BUTTON_A | BUTTON_B | BUTTON_C;

static void setUpDefaults();

static void initConfigScreen();
static void clearConfigScreen();

static void displayConfig(Config config, V2u16 pos);
static void displayOption(const char *option, const char *value, u8 highlighted, u16 x, u16 y);

static const char* printableMode(Config config);
static const char* printablePlayers(Config config);
static const char* printableDifficulty(Config config);

static void changeMode(Config config[static 1]);
static void changePlayers(Config config[static 1]);
static void changeDifficulty(Config config[static 1]);

static void expandGameConfig(Config config[static 1]);

static void joyEvent(u16 joy, u16 changed, u16 state);

volatile enum option {
	OPTION_MODE, //
	OPTION_PLAYERS, //
	OPTION_DIFFICULTY, //
	OPTION_START,
} current_option;

Config* current_config;

volatile bool start = FALSE;
volatile bool refresh = TRUE;

const V2u16 pos_init = { .x = 6, .y = 8 };

static const u8 ZX_NUM_PLANETS = 16;
Planet* (* const zxCreatePlanet[16])(void) = { //
			createPlanetZX01,//
			createPlanetZX02,//
			createPlanetZX03,//
			createPlanetZX04,//
			createPlanetZX05,//
			createPlanetZX06,//
			createPlanetZX07,//
			createPlanetZX08,//
			createPlanetZX09,//
			createPlanetZX10,//
			createPlanetZX11,//
			createPlanetZX12,//
			createPlanetZX13,//
			createPlanetZX14,//
			createPlanetZX15,//
			createPlanetZX16,//
};

static const u8 MD_NUM_PLANETS = 5;
Planet* (* const mdCreatePlanet[5])(void) = { //
			createPlanetZX01,//
			createPlanetMD01,//
			createPlanetMD02,//
			createPlanetMD03,//
			createPlanetMD04,//
};

Config setUpGame() {

	setUpDefaults();

	u8 prev_priority = VDP_getTextPriority();

	if (!current_config) {
		current_config = MEM_calloc(sizeof(*current_config));
		current_config->mode = ZX;
		current_config->difficulty = NORMAL;
		current_config->players = ONE_PLAYER;
	}

	current_option = OPTION_MODE;

	initConfigScreen();

	start = FALSE;
	refresh = TRUE;

	JOY_setEventHandler(joyEvent);

	do {
		displayConfig(*current_config, pos_init);
		VDP_waitVSync();
	} while (!start);

	setRandomSeed(getTick());

	expandGameConfig(current_config);

	clearConfigScreen();
	VDP_setTextPriority(prev_priority);

	return *current_config;
}

static void setUpDefaults() {

}

static void initConfigScreen() {

	VDP_setPalette(PAL1, palette_grey);
	VDP_setTextPalette(PAL1);
	VDP_setTextPriority(0);

	VDP_setHilightShadow(TRUE);
}

static void clearConfigScreen() {

	VDP_clearTextAreaBG(PLAN_A, 0, 5, 32, 25); // don't clear the HUD
	VDP_setHilightShadow(FALSE);
}

static void displayConfig(Config config, V2u16 pos) {

	if (refresh) {

		VDP_drawText(JETPAC_GAME_SELECTION, pos.x, pos.y);

		pos.y += 4;
		displayOption(TEXT_MODE, printableMode(config), current_option == OPTION_MODE, pos.x, pos.y);

		pos.y += 2;
		displayOption(TEXT_PLAYERS, printablePlayers(config), current_option == OPTION_PLAYERS, pos.x, pos.y);

		pos.y += 2;
		displayOption(TEXT_DIFFICULTY, printableDifficulty(config), current_option == OPTION_DIFFICULTY, pos.x, pos.y);

		pos.y += 4;
		displayOption(PRESS_START_BUTTON, 0, current_option == OPTION_START, pos.x, pos.y);

		refresh = FALSE;
	}
}

static const char* printableMode(Config config) {

	switch (config.mode) {
	case ZX:
		return TEXT_ZX;
	default:
		return TEXT_MD;
	}
}

static const char* printablePlayers(Config config) {

	switch (config.players) {
	case ONE_PLAYER:
		return TEXT_ONE_PLAYER;
	default:
		return TEXT_TWO_PLAYERS;
	}
}

static const char* printableDifficulty(Config config) {

	switch (config.difficulty) {
	case EASY:
		return TEXT_EASY;
	case NORMAL:
		return TEXT_NORMAL;
	case HARD:
		return TEXT_HARD;
	default:
		return TEXT_MANIAC;
	}
}

static void changeMode(Config config[static 1]) {

	if (config->mode == MD) {
		config->mode = ZX;
	} else {
		config->mode++;
	}
}

static void changePlayers(Config config[static 1]) {

	if (config->players == TWO_PLAYERS) {
		config->players = ONE_PLAYER;
	} else {
		config->players++;
	}
}

static void changeDifficulty(Config config[static 1]) {

	if (config->difficulty == MANIAC) {
		config->difficulty = EASY;
	} else {
		config->difficulty++;
	}
}

static void displayOption(const char *option, const char *value, u8 highlighted, u16 x, u16 y) {

	VDP_clearTextLine(y);
	VDP_setTextPriority(highlighted);
	VDP_drawText(option, x, y);
	if (value) {
		VDP_drawText(value, x + 15, y);
	}
	VDP_setTextPriority(0);
}

static void expandGameConfig(Config config[static 1]) {

	if (current_config->mode == ZX) {
		config->limited_ammo = FALSE;
		config->num_planets = ZX_NUM_PLANETS;
		config->createPlanet = zxCreatePlanet;
	} else {
		config->limited_ammo = TRUE;
		config->num_planets = MD_NUM_PLANETS;
		config->createPlanet = mdCreatePlanet;
	}

	switch (current_config->difficulty) {
	case MANIAC:
		current_config->lives = 1;
		break;
	case HARD:
		current_config->lives = 3;
		break;
	case NORMAL:
		current_config->lives = 5;
		break;
	default: // EASY
		current_config->lives = 10;
	}
}

static void joyEvent(u16 joy, u16 changed, u16 state) {

	if (BUTTON_DOWN & changed & ~state) {

		if (current_option == OPTION_START) {
			current_option = OPTION_MODE;
		} else {
			current_option++;
		}
		refresh = TRUE;
	}

	if (BUTTON_UP & changed & ~state) {
		if (current_option == OPTION_MODE) {
			current_option = OPTION_START;
		} else {
			current_option--;
		}
		refresh = TRUE;
	}

	if (BUTTON_ABC & changed & ~state) {

		if (current_option == OPTION_MODE) {
			changeMode(current_config);
			refresh = TRUE;

		} else if (current_option == OPTION_PLAYERS) {
			changePlayers(current_config);
			refresh = TRUE;

		} else if (current_option == OPTION_DIFFICULTY) {
			changeDifficulty(current_config);
			refresh = TRUE;
		}

	}

	if (BUTTON_START & changed & ~state) {
		if (current_option == OPTION_START) {
			start = TRUE;
		}
	}
}
