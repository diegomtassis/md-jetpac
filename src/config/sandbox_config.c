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

typedef enum ConfigEntryId {
    CONFIG_NONE = 0, //
	CONFIG_PLANET, //
	CONFIG_ENEMY//
} ConfigEntryId;

static void createPlanetEntry(MenuEntry* entry);
static void createEnemyEntry(MenuEntry* entry);
static void createBackEntry(MenuEntry* entry);

static MenuView config_view;

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

	menuContext.current_menu = &config_view;

	CONFIG_clearScreen();
	CONFIG_initScreen();

	menuContext.back = FALSE;
	menuContext.refresh = TRUE;

	JOY_setEventHandler(CONFIG_handleJoyEvent);

	do {
		CONFIG_displayMenu(TEXT_CONFIGURATION, pos_init);
		SYS_doVBlankProcess();
	} while (!menuContext.back);

	menuContext.back = FALSE;
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

