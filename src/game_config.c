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

Config config;

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


typedef struct ConfigEntry {
	u8 entry_id;
	const char* text;
	u8 text_pos;
	u8 num_options;
	u8 current_option;
	ConfigOption *options;
} ConfigEntry;

typedef struct ConfigView {
	ConfigEntry *entries;
	u8 num_entries;
	u8 current_entry;
} ConfigView;

static void initConfigScreen();
static void clearConfigScreen();

static void displayConfig(V2u16 pos);

static void displayConfigEntry(const ConfigEntry *entry, u8 highlighted, u16 x, u16 y);
static void incEntryCurrentOption(ConfigEntry *entry);
static void decEntryCurrentOption(ConfigEntry *entry);
static const char* printableOptionValue(const ConfigEntry *entry);
static u8 entrySpacing(u8 entryId);

static void expandGameConfig(void);

static void joyEvent(u16 joy, u16 changed, u16 state);
static void createModeEntry(ConfigEntry* entry);
static void createPlayersEntry(ConfigEntry* entry);
static void createDifficultyEntry(ConfigEntry* entry);
static void createStartEntry(ConfigEntry* entry);

static ConfigView config_view;
static volatile bool start = FALSE;
static volatile bool refresh = TRUE;

static const V2u16 pos_init = { .x = 6, .y = 8 };

static ConfigEntry *mode_entry = NULL;
static ConfigEntry *players_entry = NULL;
static ConfigEntry *difficulty_entry = NULL;
static ConfigEntry *start_entry = NULL;

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

	mode_entry = &config_view.entries[0];
	players_entry = &config_view.entries[1];
	difficulty_entry = &config_view.entries[2];
	start_entry = &config_view.entries[3];

	createModeEntry(mode_entry);
	createPlayersEntry(players_entry);
	createDifficultyEntry(difficulty_entry);
	createStartEntry(start_entry);
}

void CONFIG_setUp(void) {

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
		ConfigEntry *entry = &config_view.entries[idx];
		u8 highlighted = (idx == config_view.current_entry);
		current_y += entrySpacing(entry->entry_id);
		displayConfigEntry(entry, highlighted, pos.x, current_y);
	}

	refresh = FALSE;
}

static void incEntryCurrentOption(ConfigEntry *entry) {

	if (!entry || !entry->num_options) {
		return;
	}

	if (entry->current_option + 1 >= entry->num_options) {
		entry->current_option = 0;
	} else {
		entry->current_option++;
	}
}

static void decEntryCurrentOption(ConfigEntry *entry) {

	if (!entry || !entry->num_options) {
		return;
	}

	if (entry->current_option == 0) {
		entry->current_option = entry->num_options - 1;
	} else {
		entry->current_option--;
	}
}

static void displayConfigEntry(const ConfigEntry *entry, u8 highlighted, u16 x, u16 y) {

	if (!entry || !entry->text) {
		return;
	}

	VDP_clearTextLine(y);
	VDP_setTextPriority(highlighted);
	VDP_drawText(entry->text, x, y);
	const char *value = printableOptionValue(entry);
	if (value) {
		VDP_drawText(value, x + 15, y);
	}
	VDP_setTextPriority(0);
}

static const char* printableOptionValue(const ConfigEntry *entry) {

	if (!entry || !entry->options || entry->current_option >= entry->num_options) {
		return NULL;
	}

	return entry->options[entry->current_option].text;
}

static u8 entrySpacing(u8 entryId) {

	return (entryId == ENTRY_START) ? 4 : 2;
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

		ConfigEntry *entry = &config_view.entries[config_view.current_entry];
		if (entry->entry_id != ENTRY_START && entry->num_options) {
			incEntryCurrentOption(entry);
			refresh = TRUE;
		}
	}

	if (BUTTON_LEFT & changed & ~state) {

		ConfigEntry *entry = &config_view.entries[config_view.current_entry];
		if (entry->entry_id != ENTRY_START && entry->num_options) {
			decEntryCurrentOption(entry);
			refresh = TRUE;
		}
	}

	if (BUTTON_START & changed & ~state) {
		if (config_view.entries[config_view.current_entry].entry_id == ENTRY_START) {
			start = TRUE;
		}
	}
}

static void createModeEntry(ConfigEntry* entry) {

	entry->entry_id = ENTRY_MODE;
	entry->text = TEXT_ENTRY_MODE;
	entry->text_pos = 0;
	entry->num_options = 2;
	entry->options = MEM_calloc(sizeof(ConfigOption) * entry->num_options);
	entry->current_option = 1;

	entry->options[0].text = TEXT_OPTION_ZX;
	entry->options[0].value = ZX;
	entry->options[0].text_pos = 0;

	entry->options[1].text = TEXT_OPTION_MD;
	entry->options[1].value = MD;
	entry->options[1].text_pos = 0;
}

static void createPlayersEntry(ConfigEntry* entry) {

	entry->entry_id = ENTRY_PLAYERS;
	entry->text = TEXT_ENTRY_PLAYERS;
	entry->text_pos = 0;
	entry->num_options = 2;
	entry->options = MEM_calloc(sizeof(ConfigOption) * entry->num_options);
	entry->current_option = 1;

	entry->options[0].text = TEXT_OPTION_ONE_PLAYER;
	entry->options[0].value = ONE_PLAYER;
	entry->options[0].text_pos = 0;

	entry->options[1].text = TEXT_OPTION_TWO_PLAYERS;
	entry->options[1].value = TWO_PLAYERS;
	entry->options[1].text_pos = 0;
}

static void createDifficultyEntry(ConfigEntry* entry) {

	entry->entry_id = ENTRY_DIFFICULTY;
	entry->text = TEXT_ENTRY_DIFFICULTY;
	entry->text_pos = 0;
	entry->num_options = 4;
	entry->options = MEM_calloc(sizeof(ConfigOption) * entry->num_options);
	entry->current_option = 1;

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

	entry->entry_id = ENTRY_START;
	entry->text = TEXT_ENTRY_START;
	entry->text_pos = 0;
	entry->options = NULL;
	entry->num_options = 0;
	entry->current_option = 0;
}
