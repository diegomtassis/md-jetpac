/*
 * config.c
 *
 *  Created on: May 5, 2019
 *      Author: diegomtassis
 */

#include "../../inc/config/game_config.h"

#include <genesis.h>

#include "../../inc/fwk/commons.h"
#include "../../inc/config/config.h"
#include "../../inc/elements.h"
#include "../../inc/planets.h"

Config config;

static const u16 MARGIN = 3;

static const char* TEXT_CONFIGURATION = "JETPAC Configuration";

static const char* TEXT_ENTRY_MODE = "Mode";
static const char* TEXT_OPTION_ZX = "ZX";
static const char* TEXT_OPTION_MD = "MD";
static const char* TEXT_OPTION_SANDBOX = "Sandbox";

static const char* TEXT_ENTRY_PLAYERS = "Players";
static const char* TEXT_OPTION_ONE_PLAYER = "1";
static const char* TEXT_OPTION_TWO_PLAYERS = "2";

static const char* TEXT_ENTRY_DIFFICULTY = "Difficulty";
static const char* TEXT_OPTION_EASY = "Easy";
static const char* TEXT_OPTION_NORMAL = "Normal";
static const char* TEXT_OPTION_HARD = "Hard";
static const char* TEXT_OPTION_MANIAC = "Maniac";

static const char* TEXT_ENTRY_START = "Start Game";


static const u16 BUTTON_ABC = BUTTON_A | BUTTON_B | BUTTON_C;

typedef enum ConfigEntryId {
	CONFIG_NONE = 0, //
	CONFIG_MODE, //
	CONFIG_PLAYERS, //
	CONFIG_DIFFICULTY //
} ConfigEntryId;

static void initConfigScreen();
static void clearConfigScreen();

static void displayConfig(V2u16 pos);

static void displayConfigEntry(const MenuEntry *entry, u8 highlighted, u16 x, u16 y);
static u8 entrySpacing(u8 entryId);

static void expandGameConfig(void);

static void joyEvent(u16 joy, u16 changed, u16 state);
static void createModeEntry(MenuEntry* entry);
static void createPlayersEntry(MenuEntry* entry);
static void createDifficultyEntry(MenuEntry* entry);
static void createStartEntry(MenuEntry* entry);

static MenuView config_view;
static volatile bool start = FALSE;
static volatile bool refresh = TRUE;

static const V2u16 pos_init = { .x = 6, .y = 6 };

static MenuEntry *mode_entry = NULL;
static MenuEntry *players_entry = NULL;
static MenuEntry *difficulty_entry = NULL;
static MenuEntry *start_entry = NULL;

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

void CONFIG_GAME_init(void) {

	config_view.num_entries = 4;
	config_view.current_entry = 0;
	config_view.entries = MEM_calloc(sizeof(MenuEntry) * config_view.num_entries);

	mode_entry = &config_view.entries[0];
	createModeEntry(mode_entry);

	players_entry = &config_view.entries[1];
	createPlayersEntry(players_entry);

	difficulty_entry = &config_view.entries[2];
	createDifficultyEntry(difficulty_entry);

	start_entry = &config_view.entries[3];
	createStartEntry(start_entry);
}

void CONFIG_GAME_setUp(void) {

	u8 prev_priority = VDP_getTextPriority();

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

	if (!refresh) {
		return;
	}

	VDP_drawText(TEXT_CONFIGURATION, pos.x, pos.y);

	u16 current_y = pos.y + 2;
	for (u8 idx = 0; idx < config_view.num_entries; idx++) {
		MenuEntry *entry = &config_view.entries[idx];
		u8 highlighted = (idx == config_view.current_entry);
		current_y += entrySpacing(entry->type);
		displayConfigEntry(entry, highlighted, pos.x, current_y);
	}

	refresh = FALSE;
}


static void displayConfigEntry(const MenuEntry *entry, u8 highlighted, u16 x, u16 y) {

	if (!entry || !entry->text) {
		return;
	}

	VDP_clearTextLine(y);
	VDP_setTextPriority(highlighted);
	VDP_drawText(entry->text, entry->text_pos, y);
	const char *value = CONFIG_printableOptionValue(entry);
	if (value) {
		VDP_drawText(value, entry->options[entry->current_option].text_pos, y);
	}
	VDP_setTextPriority(0);
}

static u8 entrySpacing(u8 type) {

	return (type == ENTRY_START) ? 4 : 2;
}


static void expandGameConfig(void) {

	config.mode = mode_entry->options[mode_entry->current_option].value;
	config.difficulty = difficulty_entry->options[difficulty_entry->current_option].value;
	config.players = players_entry->options[players_entry->current_option].value;

	if (config.mode == ZX) {
		config.limited_ammo = FALSE;
		config.num_planets = ZX_NUM_PLANETS;
		config.createPlanet = zxCreatePlanet;
	} else {
		config.limited_ammo = TRUE;
		config.num_planets = MD_NUM_PLANETS;
		config.createPlanet = mdCreatePlanet;
	}

	switch (config.difficulty) {
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
		if (config_view.current_entry + 1 < config_view.num_entries) {
			config_view.current_entry++;
		}
		refresh = TRUE;
	}

	if (BUTTON_UP & changed & ~state) {
		if (config_view.current_entry > 0) {
			config_view.current_entry--;
		}
		refresh = TRUE;
	}

	if (BUTTON_RIGHT & changed & ~state) {
		MenuEntry *entry = &config_view.entries[config_view.current_entry];
		if (entry->type == ENTRY_CONFIG && entry->num_options) {
			CONFIG_incrementOption(entry);
			refresh = TRUE;
		}
	}

	if (BUTTON_LEFT & changed & ~state) {
		MenuEntry *entry = &config_view.entries[config_view.current_entry];
		if (entry->type == ENTRY_CONFIG && entry->num_options) {
			CONFIG_decrementOption(entry);
			refresh = TRUE;
		}
	}

	if (BUTTON_START & changed & ~state) {
		if (config_view.entries[config_view.current_entry].type == ENTRY_START) {
			start = TRUE;
		}
	}
}

static void createModeEntry(MenuEntry* entry) {

	entry->type = ENTRY_CONFIG;
	entry->entry_id = CONFIG_MODE;
	entry->text = TEXT_ENTRY_MODE;
	entry->text_pos = MARGIN;
	entry->num_options = 3;
	entry->options = MEM_calloc(sizeof(ConfigOption) * entry->num_options);
	entry->current_option = 0;

	CONFIG_setOption(&entry->options[0], TEXT_OPTION_ZX, ZX, NULL);
	CONFIG_setOption(&entry->options[1], TEXT_OPTION_MD, MD, NULL);
	CONFIG_setOption(&entry->options[2], TEXT_OPTION_SANDBOX, SANDBOX, NULL);
}

static void createPlayersEntry(MenuEntry* entry) {

	entry->type = ENTRY_CONFIG;
	entry->entry_id = CONFIG_PLAYERS;
	entry->text = TEXT_ENTRY_PLAYERS;
	entry->text_pos = MARGIN;
	entry->num_options = 2;
	entry->options = MEM_calloc(sizeof(ConfigOption) * entry->num_options);
	entry->current_option = 0;

	CONFIG_setOption(&entry->options[0], TEXT_OPTION_ONE_PLAYER, ONE_PLAYER, NULL);
	CONFIG_setOption(&entry->options[1], TEXT_OPTION_TWO_PLAYERS, TWO_PLAYERS, NULL);
}

static void createDifficultyEntry(MenuEntry* entry) {

	entry->type = ENTRY_CONFIG;
	entry->entry_id = CONFIG_DIFFICULTY;
	entry->text = TEXT_ENTRY_DIFFICULTY;
	entry->text_pos = MARGIN;
	entry->num_options = 4;
	entry->options = MEM_calloc(sizeof(ConfigOption) * entry->num_options);
	entry->current_option = 0;

	CONFIG_setOption(&entry->options[0], TEXT_OPTION_EASY, EASY, NULL);
	CONFIG_setOption(&entry->options[1], TEXT_OPTION_NORMAL, NORMAL, NULL);
	CONFIG_setOption(&entry->options[2], TEXT_OPTION_HARD, HARD, NULL);
	CONFIG_setOption(&entry->options[3], TEXT_OPTION_MANIAC, MANIAC, NULL);
}

static void createStartEntry(MenuEntry* entry) {

	entry->type = ENTRY_START;
	entry->entry_id = CONFIG_NONE;
	entry->text = TEXT_ENTRY_START;
	entry->text_pos = MARGIN;
	entry->options = NULL;
	entry->num_options = 0;
	entry->current_option = 0;
}
