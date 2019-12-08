/*
 * config.c
 *
 *  Created on: May 5, 2019
 *      Author: diegomtassis
 */

#include "../inc/config.h"

#include <genesis.h>

#include "../inc/planets.h"

static const char* JETPAC_GAME_SELECTION = "JETPAC GAME SELECTION";
static const char* TEXT_MODE = "MODE";
static const char* TEXT_ZX = "ZX";
static const char* TEXT_MD = "MD";
static const char* TEXT_DIFFICULTY = "DIFFICULTY";
static const char* TEXT_EASY = "EASY";
static const char* TEXT_NORMAL = "NORMAL";
static const char* TEXT_HARD = "HARD";
static const char* TEXT_MANIAC = "MANIAC";
static const char* TEXT_PLAYERS = "PLAYERS";
static const char* TEXT_ONE_PLAYER = "1";
static const char* TEXT_TWO_PLAYERS = "2";
static const char* PRESS_START_BUTTON = "START GAME";

static void initConfigScreen();
static void clearConfigScreen();

static void handleConfig(Config config[static 1], V2u16 pos);
static void displayConfig(Config config, V2u16 pos);
static void displayOption(const char *option, const char *value, u8 highlighted, u16 x, u16 y);

static const char* printableMode(Config config);
static const char* printableDifficulty(Config config);
static const char* printablePlayers(Config config);

static void changeMode(Config config[static 1]);
static void changeDifficulty(Config config[static 1]);
static void changePlayers(Config config[static 1]);

static void expandGameConfig(Game* game);

static void joyEvent(u16 joy, u16 changed, u16 state);

volatile enum option {
	OPTION_MODE, //
	OPTION_DIFFICULTY, //
	OPTION_PLAYERS, //
	OPTION_START,
} current_option;

Config* current_config;

volatile bool start = FALSE;
volatile bool refresh = TRUE;

const V2u16 pos_init = { .x = 6, .y = 8 };

void setUpGame(Game* game) {

	u8 prev_priority = VDP_getTextPriority();

	if (!current_config) {
		current_config = MEM_alloc(sizeof(*current_config));
	}

	current_option = OPTION_MODE;

	initConfigScreen();

	start = FALSE;
	refresh = TRUE;

	JOY_setEventHandler(joyEvent);

	do {
		handleConfig(current_config, pos_init);
		VDP_waitVSync();
	} while (!start);

	setRandomSeed(getTick());

	expandGameConfig(game);

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

	VDP_clearTextAreaBG(PLAN_A, 0, 5, 32, 25); // don't clear the HUD
	VDP_setHilightShadow(FALSE);
}

static void handleConfig(Config config[static 1], V2u16 pos) {

	displayConfig(*config, pos);
}

static void displayConfig(Config config, V2u16 pos) {

	if (refresh) {

		VDP_drawText(JETPAC_GAME_SELECTION, pos.x, pos.y);

		pos.y += 4;
		displayOption(TEXT_MODE, printableMode(config), current_option == OPTION_MODE, pos.x, pos.y);

		pos.y += 2;
		displayOption(TEXT_DIFFICULTY, printableDifficulty(config), current_option == OPTION_DIFFICULTY, pos.x, pos.y);

		pos.y += 2;
		displayOption(TEXT_PLAYERS, printablePlayers(config), current_option == OPTION_PLAYERS, pos.x, pos.y);

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

static const char* printablePlayers(Config config) {

	switch (config.players) {
	case ONE_PLAYER:
		return TEXT_ONE_PLAYER;
	default:
		return TEXT_TWO_PLAYERS;
	}
}

static void changeMode(Config config[static 1]) {

	if (config->mode == MD) {
		config->mode = ZX;
	} else {
		config->mode++;
	}
}

static void changeDifficulty(Config config[static 1]) {

	if (config->difficulty == MANIAC) {
		config->difficulty = EASY;
	} else {
		config->difficulty++;
	}
}

static void changePlayers(Config config[static 1]) {

	if (config->players == TWO_PLAYERS) {
		config->players = ONE_PLAYER;
	} else {
		config->players++;
	}
}

static void displayOption(const char *option, const char *value, u8 highlighted, u16 x, u16 y) {

	VDP_setTextPriority(highlighted);
	VDP_clearTextLine(y);
	VDP_drawText(option, x, y);
	if (value) {
		VDP_drawText(value, x + 15, y);
	}
	VDP_setTextPriority(0);
}

static void expandGameConfig(Game* game) {

	u8 planet = 0;
	if (current_config->mode == ZX) {
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
		game->num_planets = 5;
		game->createPlanet = MEM_alloc(game->num_planets * sizeof(Planet*));
		game->createPlanet[planet++] = createPlanetZX01;
		game->createPlanet[planet++] = createPlanetMD01;
		game->createPlanet[planet++] = createPlanetMD02;
		game->createPlanet[planet++] = createPlanetMD03;
		game->createPlanet[planet++] = createPlanetMD04;
	}

	switch (current_config->difficulty) {
	case MANIAC:
		game->lives = 1;
		break;
	case HARD:
		game->lives = 3;
		break;
	case NORMAL:
		game->lives = 5;
		break;
	default: // EASY
		game->lives = 10;
	}

	game->score = 0;

	game->config = current_config;
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

	if ((BUTTON_A | BUTTON_B | BUTTON_C) & changed & ~state) {

		if (current_option == OPTION_MODE) {
			changeMode(current_config);
			refresh = TRUE;

		} else if (current_option == OPTION_DIFFICULTY) {
			changeDifficulty(current_config);
			refresh = TRUE;

		} else if (current_option == OPTION_PLAYERS) {
			changePlayers(current_config);
			refresh = TRUE;
		}
	}

	if (BUTTON_START & changed & ~state) {
		if (current_option == OPTION_START) {
			start = TRUE;
		}
	}
}
