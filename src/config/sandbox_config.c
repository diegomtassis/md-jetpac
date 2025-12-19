/*
 * sandbox_config.c
 */

#include "../../inc/config/sandbox_config.h"

#include <genesis.h>
#include <kdebug.h>

#include "../../inc/fwk/commons.h"
#include "../../inc/config/config.h"

SandboxConfig sandbox_config;

static const u16 MARGIN = 3;

static const char* TEXT_CONFIGURATION = "Sandbox Configuration";

static const char* TEXT_ENTRY_PLANET = "Planet";
static const char* TEXT_OPTION_LEYNOS = "Leynos";
static const char* TEXT_OPTION_ANSIMUZ = "Ansimuz";

static const char* TEXT_ENTRY_ENEMY = "Enemy";
static const char* TEXT_OPTION_ALIEN = "Alien";
static const char* TEXT_OPTION_BUBBLE = "Bubble";
static const char* TEXT_OPTION_CROSS = "Cross";
static const char* TEXT_OPTION_FALCON = "Falcon";
static const char* TEXT_OPTION_FIGHTER = "Fighter";
static const char* TEXT_OPTION_METEORITE = "Meteorite";
static const char* TEXT_OPTION_OWL = "Owl";
static const char* TEXT_OPTION_SAUCER = "Saucer";

static const char* TEXT_ENTRY_BACK = "Back";

static const u16 BUTTON_ABC = BUTTON_A | BUTTON_B | BUTTON_C;

typedef enum ConfigEntryId {
    CONFIG_NONE = 0, //
	CONFIG_PLANET, //
	CONFIG_ENEMY//
} ConfigEntryId;

static void initConfigScreen();
static void clearConfigScreen();
static void displayConfig(V2u16 pos);

static void displayConfigEntry(const MenuEntry *entry, u8 highlighted, u16 x, u16 y);
static u8 entrySpacing(u8 entryId);

static void joyEvent(u16 joy, u16 changed, u16 state);
static void createPlanetEntry(MenuEntry* entry);
static void createEnemyEntry(MenuEntry* entry);
static void createBackEntry(MenuEntry* entry);

static MenuView config_view;

static volatile bool back = FALSE;
static volatile bool refresh = TRUE;

static const V2u16 pos_init = { .x = 4, .y = 6 };

static MenuEntry *planet_entry = NULL;
static MenuEntry *enemy_entry = NULL;
static MenuEntry *back_entry = NULL;

void CONFIG_SANDBOX_init(void) {

	config_view.num_entries = 3;
	config_view.current_entry = 0;
	config_view.entries = MEM_calloc(sizeof(MenuEntry) * config_view.num_entries);

	planet_entry = &config_view.entries[0];
	createPlanetEntry(planet_entry);

	enemy_entry = &config_view.entries[1];
	createEnemyEntry(enemy_entry);

   	back_entry = &config_view.entries[2];
	createBackEntry(back_entry);
}

void CONFIG_SANDBOX_setUp(void) {

	clearConfigScreen();
	initConfigScreen();

	back = FALSE;
	refresh = TRUE;

	JOY_setEventHandler(joyEvent);

	do {
		displayConfig(pos_init);
		SYS_doVBlankProcess();
	} while (!back);
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

	return (type == ENTRY_CONFIG) ? 2 : 4;
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

	if (BUTTON_BTN & changed & ~state) {
		if (config_view.entries[config_view.current_entry].type == ENTRY_BACK) {
		 back = TRUE;
		 config_view.current_entry = 0;
		}
	}
}

static void createPlanetEntry(MenuEntry* entry) {

    entry->type = ENTRY_CONFIG;
    entry->text = TEXT_ENTRY_PLANET;
	entry->text_pos = MARGIN;
	entry->num_options = 2;
	entry->options = MEM_calloc(sizeof(ConfigOption) * entry->num_options);
	entry->current_option = 0;

	CONFIG_setOption(&entry->options[0], TEXT_OPTION_LEYNOS, LEYNOS, NULL);
	CONFIG_setOption(&entry->options[1], TEXT_OPTION_ANSIMUZ, ANSIMUZ, NULL);
}

static void createEnemyEntry(MenuEntry* entry) {

    entry->type = ENTRY_CONFIG;
    entry->text = TEXT_ENTRY_ENEMY;
    entry->text_pos = MARGIN;
    entry->num_options = 8;
	entry->options = MEM_calloc(sizeof(ConfigOption) * entry->num_options);
    entry->current_option = 0;

	CONFIG_setOption(&entry->options[0], TEXT_OPTION_ALIEN, ALIEN, NULL);
	CONFIG_setOption(&entry->options[1], TEXT_OPTION_BUBBLE, BUBBLE, NULL);
	CONFIG_setOption(&entry->options[2], TEXT_OPTION_CROSS, CROSS, NULL);
	CONFIG_setOption(&entry->options[3], TEXT_OPTION_FALCON, FALCON, NULL);
	CONFIG_setOption(&entry->options[4], TEXT_OPTION_FIGHTER, FIGHTER, NULL);
	CONFIG_setOption(&entry->options[5], TEXT_OPTION_METEORITE, METEORITE, NULL);
	CONFIG_setOption(&entry->options[6], TEXT_OPTION_OWL, OWL, NULL);
	CONFIG_setOption(&entry->options[7], TEXT_OPTION_SAUCER, SAUCER, NULL);
}

static void createBackEntry(MenuEntry* entry) {

	entry->type = ENTRY_BACK;
	entry->entry_id = CONFIG_NONE;
	entry->text = TEXT_ENTRY_BACK;
	entry->text_pos = MARGIN;
	entry->options = NULL;
	entry->num_options = 0;
	entry->current_option = 0;
}

