/*
 * sandbox_config.c
 */

#include "../../inc/config/sandbox_config.h"

#include <genesis.h>
#include <kdebug.h>

#include "../../inc/fwk/commons.h"
#include "../../inc/config/config.h"
#include "../../inc/planets.h"

SandboxConfig sandbox_config;

static const char* TEXT_CONFIGURATION = "Sandbox Configuration";

static const char* TEXT_ENTRY_SYSTEM = "System";
static const char* TEXT_OPTION_P_CENTAURI = "P Centauri";
static const char* TEXT_OPTION_ERIDANI = "Eridani";

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

static void createPlanetEntry(MenuEntry* entry);
static void createEnemyEntry(MenuEntry* entry);
static void createBackEntry(MenuEntry* entry);

static void expandSandboxConfig(void);

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

	menu_context.current_menu = &config_view;

	CONFIG_clearScreen();
	CONFIG_initScreen();

	menu_context.back = FALSE;
	menu_context.refresh = TRUE;

	JOY_setEventHandler(CONFIG_handleJoyEvent);

	do {
		CONFIG_displayMenu(TEXT_CONFIGURATION, pos_init);
		SYS_doVBlankProcess();
	} while (!menu_context.back);

	expandSandboxConfig();

	menu_context.back = FALSE;
}

static void createPlanetEntry(MenuEntry* entry) {

    entry->type = ENTRY_CONFIG;
    entry->text = TEXT_ENTRY_SYSTEM;
	entry->text_pos = MARGIN_L1;
	entry->num_options = 2;
	entry->options = MEM_calloc(sizeof(ConfigOption) * entry->num_options);
	entry->current_option = 0;

	CONFIG_setOption(&entry->options[0], TEXT_OPTION_P_CENTAURI, SANDBOX_SYSTEM_P_CENTAURI, NULL);
	CONFIG_setOption(&entry->options[1], TEXT_OPTION_ERIDANI, SANDBOX_SYSTEM_ERIDANI, NULL);
}

static void createEnemyEntry(MenuEntry* entry) {

    entry->type = ENTRY_CONFIG;
    entry->text = TEXT_ENTRY_ENEMY;
    entry->text_pos = MARGIN_L1;
    entry->num_options = 8;
	entry->options = MEM_calloc(sizeof(ConfigOption) * entry->num_options);
    entry->current_option = 0;

	CONFIG_setOption(&entry->options[0], TEXT_OPTION_ALIEN, SANDBOX_ENEMY_ALIEN, NULL);
	CONFIG_setOption(&entry->options[1], TEXT_OPTION_BUBBLE, SANDBOX_ENEMY_BUBBLE, NULL);
	CONFIG_setOption(&entry->options[2], TEXT_OPTION_CROSS, SANDBOX_ENEMY_CROSS, NULL);
	CONFIG_setOption(&entry->options[3], TEXT_OPTION_FALCON, SANDBOX_ENEMY_FALCON, NULL);
	CONFIG_setOption(&entry->options[4], TEXT_OPTION_FIGHTER, SANDBOX_ENEMY_FIGHTER, NULL);
	CONFIG_setOption(&entry->options[5], TEXT_OPTION_METEORITE, SANDBOX_ENEMY_METEORITE, NULL);
	CONFIG_setOption(&entry->options[6], TEXT_OPTION_OWL, SANDBOX_ENEMY_OWL, NULL);
	CONFIG_setOption(&entry->options[7], TEXT_OPTION_SAUCER, SANDBOX_ENEMY_SAUCER, NULL);
}

static void createBackEntry(MenuEntry* entry) {

	entry->type = ENTRY_BACK;
	entry->text = TEXT_ENTRY_BACK;
	entry->text_pos = MARGIN_L1;
	entry->options = NULL;
	entry->num_options = 0;
	entry->current_option = 0;
}

static void expandSandboxConfig(void) {
	sandbox_config.system = planet_entry->options[planet_entry->current_option].value;
	sandbox_config.enemy = enemy_entry->options[enemy_entry->current_option].value;
}

