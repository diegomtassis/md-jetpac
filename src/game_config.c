/*
 * config.c
 *
 *  Created on: May 5, 2019
 *      Author: diegomtassis
 */

#include "../inc/game_config.h"

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

typedef struct ConfigView {
	u8 mode;
	u8 difficulty;
	u8 players;
	u8 lives;
	bool limited_ammo;
} ConfigView;

static void setUpDefaults();

static void initConfigScreen();
static void clearConfigScreen();

static void displayConfig(V2u16 pos);
static void displayOption(const char *option, const char *value, u8 highlighted, u16 x, u16 y);

static const char* printableMode(void);
static const char* printablePlayers(void);
static const char* printableDifficulty(void);

static void changeMode(void);
static void changePlayers(void);
static void changeDifficulty(void);

static void expandGameConfig(void);

static void joyEvent(u16 joy, u16 changed, u16 state);

volatile enum option {
	OPTION_MODE, //
	OPTION_PLAYERS, //
	OPTION_DIFFICULTY, //
	OPTION_START,
} current_option;

Config config;

static ConfigView config_view;
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
			createPlanetMD01,//
			createPlanetMD02,//
			createPlanetMD03,//
			createPlanetMD04,//
};

void CONFIG_init(void) {

	setUpDefaults();
}

void CONFIG_setUp(void) {

	u8 prev_priority = VDP_getTextPriority();

	current_option = OPTION_MODE;

	initConfigScreen();

	start = FALSE;
	refresh = TRUE;

	JOY_setEventHandler(joyEvent);

	do {
		displayConfig(pos_init);
		SYS_doVBlankProcess();
	} while (!start);

	setRandomSeed(getTick());

	expandGameConfig();

	clearConfigScreen();
	VDP_setTextPriority(prev_priority);
}

static void setUpDefaults() {
	config_view.mode = ZX;
	config_view.difficulty = NORMAL;
	config_view.players = ONE_PLAYER;
	config_view.lives = 0;
	config_view.limited_ammo = FALSE;
}

static void initConfigScreen() {

	PAL_setPalette(PAL1, palette_grey, CPU);
	VDP_setTextPalette(PAL1);
	VDP_setTextPriority(0);

	VDP_setHilightShadow(TRUE);
}

static void clearConfigScreen() {

	VDP_clearTextAreaBG(BG_A, 0, 5, 32, 25); // don't clear the HUD
	VDP_setHilightShadow(FALSE);
}

static void displayConfig(V2u16 pos) {

	if (refresh) {

		VDP_drawText(JETPAC_GAME_SELECTION, pos.x, pos.y);

		pos.y += 4;
		displayOption(TEXT_MODE, printableMode(), current_option == OPTION_MODE, pos.x, pos.y);

		pos.y += 2;
		displayOption(TEXT_PLAYERS, printablePlayers(), current_option == OPTION_PLAYERS, pos.x, pos.y);

		pos.y += 2;
		displayOption(TEXT_DIFFICULTY, printableDifficulty(), current_option == OPTION_DIFFICULTY, pos.x, pos.y);

		pos.y += 4;
		displayOption(PRESS_START_BUTTON, 0, current_option == OPTION_START, pos.x, pos.y);

		refresh = FALSE;
	}
}

static const char* printableMode(void) {

	switch (config_view.mode) {
	case ZX:
		return TEXT_ZX;
	default:
		return TEXT_MD;
	}
}

static const char* printablePlayers(void) {

	switch (config_view.players) {
	case ONE_PLAYER:
		return TEXT_ONE_PLAYER;

	default:
		return TEXT_TWO_PLAYERS;
	}
}

static const char* printableDifficulty(void) {

	switch (config_view.difficulty) {
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

static void changeMode(void) {

	if (config_view.mode == MD) {
		config_view.mode = ZX;
	} else {
		config_view.mode++;
	}
}

static void changePlayers(void) {

	if (config_view.players == TWO_PLAYERS) {
		config_view.players = ONE_PLAYER;
	} else {
		config_view.players++;
	}
}

static void changeDifficulty(void) {

	if (config_view.difficulty == MANIAC) {
		config_view.difficulty = EASY;
	} else {
		config_view.difficulty++;
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

static void expandGameConfig(void) {

	config.mode = config_view.mode;
	config.difficulty = config_view.difficulty;
	config.players = config_view.players;

	if (config_view.mode == ZX) {
		config.limited_ammo = FALSE;
		config.num_planets = ZX_NUM_PLANETS;
		config.createPlanet = zxCreatePlanet;
	} else {
		config.limited_ammo = TRUE;
		config.num_planets = MD_NUM_PLANETS;
		config.createPlanet = mdCreatePlanet;
	}

	switch (config_view.difficulty) {
	case MANIAC:
		config.lives = 1;
		break;
	case HARD:
		config.lives = 3;
		break;
	case NORMAL:
		config.lives = 5;
		break;
	default: // EASY
		config.lives = 10;
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
			changeMode();
			refresh = TRUE;

		} else if (current_option == OPTION_PLAYERS) {
			changePlayers();
			refresh = TRUE;

		} else if (current_option == OPTION_DIFFICULTY) {
			changeDifficulty();
			refresh = TRUE;
		}

	}

	if (BUTTON_START & changed & ~state) {
		if (current_option == OPTION_START) {
			start = TRUE;
		}
	}
}
