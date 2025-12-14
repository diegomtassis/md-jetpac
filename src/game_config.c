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

static const char* TEXT_CONFIGURATION = "JETPAC GAME SELECTION";

static const char* TEXT_ENTRY_MODE = "MODE";
static const char* TEXT_OPTION_ZX = "ZX";
static const char* TEXT_OPTION_MD = "MD";

static const char* TEXT_ENTRY_PLAYERS = "PLAYERS";
static const char* TEXT_OPTION_ONE_PLAYER = "1";
static const char* TEXT_OPTION_TWO_PLAYERS = "2";

static const char* TEXT_ENTRY_DIFFICULTY = "DIFFICULTY";
static const char* TEXT_OPTION_EASY = "EASY";
static const char* TEXT_OPTION_NORMAL = "NORMAL";
static const char* TEXT_OPTION_HARD = "HARD";
static const char* TEXT_OPTION_MANIAC = "MANIAC";

static const char* TEXT_ENTRY_START = "START GAME";

static const u16 BUTTON_ABC = BUTTON_A | BUTTON_B | BUTTON_C;

typedef struct ConfigOption {
	const char* text;
	u8 value;
	u8 text_pos;
} ConfigOption;

typedef enum ConfigEntryId {
	ENTRY_MODE, //
	ENTRY_PLAYERS, //
	ENTRY_DIFFICULTY, //
	ENTRY_START,
} ConfigEntryId;

volatile enum ConfigEntryId current_entry;

typedef struct ConfigEntry {
	u8 entry;
	const char* text;
	u8 text_pos;
	u8 num_options;
	u8 current_option;
	ConfigOption *options;
} ConfigEntry;

typedef struct ConfigView {
	u8 mode;
	u8 difficulty;
	u8 players;

	u8 num_entries;
	u8 current_entry;
	ConfigEntry *entries;
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
static void createModeEntry(ConfigEntry* entry);
static void createPlayersEntry(ConfigEntry* entry);
static void createDifficultyEntry(ConfigEntry* entry);
static void createStartEntry(ConfigEntry* entry);

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

	config_view.num_entries = 4;
	config_view.current_entry = 0;
	config_view.entries = MEM_calloc(sizeof(ConfigEntry) * config_view.num_entries);

	createModeEntry(&config_view.entries[0]);
	createPlayersEntry(&config_view.entries[1]);
	createDifficultyEntry(&config_view.entries[2]);
	createStartEntry(&config_view.entries[3]);
}

void CONFIG_setUp(void) {

	u8 prev_priority = VDP_getTextPriority();

	current_entry = ENTRY_MODE;

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

		VDP_drawText(TEXT_CONFIGURATION, pos.x, pos.y);

		pos.y += 4;
		displayOption(TEXT_ENTRY_MODE, printableMode(), current_entry == ENTRY_MODE, pos.x, pos.y);

		pos.y += 2;
		displayOption(TEXT_ENTRY_PLAYERS, printablePlayers(), current_entry == ENTRY_PLAYERS, pos.x, pos.y);

		pos.y += 2;
		displayOption(TEXT_ENTRY_DIFFICULTY, printableDifficulty(), current_entry == ENTRY_DIFFICULTY, pos.x, pos.y);

		pos.y += 4;
		displayOption(TEXT_ENTRY_START, 0, current_entry == ENTRY_START, pos.x, pos.y);

		refresh = FALSE;
	}
}

static const char* printableMode(void) {

	switch (config_view.mode) {
	case ZX:
		return TEXT_OPTION_ZX;
	default:
		return TEXT_OPTION_MD;
	}
}

static const char* printablePlayers(void) {

	switch (config_view.players) {
	case ONE_PLAYER:
		return TEXT_OPTION_ONE_PLAYER;

	default:
		return TEXT_OPTION_TWO_PLAYERS;
	}
}

static const char* printableDifficulty(void) {

	switch (config_view.difficulty) {
	case EASY:
		return TEXT_OPTION_EASY;
	case NORMAL:
		return TEXT_OPTION_NORMAL;
	case HARD:
		return TEXT_OPTION_HARD;
	default:
		return TEXT_OPTION_MANIAC;
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

		if (current_entry == ENTRY_START) {
			current_entry = ENTRY_MODE;
		} else {
			current_entry++;
		}
		refresh = TRUE;
	}

	if (BUTTON_UP & changed & ~state) {
		if (current_entry == ENTRY_MODE) {
			current_entry = ENTRY_START;
		} else {
			current_entry--;
		}
		refresh = TRUE;
	}

	if (BUTTON_ABC & changed & ~state) {

		if (current_entry == ENTRY_MODE) {
			changeMode();
			refresh = TRUE;

		} else if (current_entry == ENTRY_PLAYERS) {
			changePlayers();
			refresh = TRUE;

		} else if (current_entry == ENTRY_DIFFICULTY) {
			changeDifficulty();
			refresh = TRUE;
		}

	}

	if (BUTTON_START & changed & ~state) {
		if (current_entry == ENTRY_START) {
			start = TRUE;
		}
	}
}

static void createModeEntry(ConfigEntry* entry) {

	entry->entry = ENTRY_MODE;
	entry->text = TEXT_ENTRY_MODE;
	entry->text_pos = 0;
	entry->num_options = 2;
	entry->current_option = config_view.mode;
	entry->options = MEM_calloc(sizeof(ConfigOption) * entry->num_options);

	entry->options[0].text = TEXT_OPTION_ZX;
	entry->options[0].value = ZX;
	entry->options[0].text_pos = 0;

	entry->options[1].text = TEXT_OPTION_MD;
	entry->options[1].value = MD;
	entry->options[1].text_pos = 0;
}

static void createPlayersEntry(ConfigEntry* entry) {

	entry->entry = ENTRY_PLAYERS;
	entry->text = TEXT_ENTRY_PLAYERS;
	entry->text_pos = 0;
	entry->num_options = 2;
	entry->current_option = config_view.players - 1;
	entry->options = MEM_calloc(sizeof(ConfigOption) * entry->num_options);

	entry->options[0].text = TEXT_OPTION_ONE_PLAYER;
	entry->options[0].value = ONE_PLAYER;
	entry->options[0].text_pos = 0;

	entry->options[1].text = TEXT_OPTION_TWO_PLAYERS;
	entry->options[1].value = TWO_PLAYERS;
	entry->options[1].text_pos = 0;
}

static void createDifficultyEntry(ConfigEntry* entry) {

	entry->entry = ENTRY_DIFFICULTY;
	entry->text = TEXT_ENTRY_DIFFICULTY;
	entry->text_pos = 0;
	entry->num_options = 4;
	entry->current_option = config_view.difficulty;
	entry->options = MEM_calloc(sizeof(ConfigOption) * entry->num_options);

	entry->options[0].text = TEXT_OPTION_EASY;
	entry->options[0].value = EASY;
	entry->options[0].text_pos = 0;

	entry->options[1].text = TEXT_OPTION_NORMAL;
	entry->options[1].value = NORMAL;
	entry->options[1].text_pos = 0;

	entry->options[2].text = TEXT_OPTION_HARD;
	entry->options[2].value = HARD;
	entry->options[2].text_pos = 0;

	entry->options[3].text = TEXT_OPTION_MANIAC;
	entry->options[3].value = MANIAC;
	entry->options[3].text_pos = 0;
}

static void createStartEntry(ConfigEntry* entry) {

	entry->entry = ENTRY_START;
	entry->text = TEXT_ENTRY_START;
	entry->text_pos = 0;
	entry->num_options = 0;
	entry->current_option = 0;
	entry->options = NULL;
}
