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

static const char* TEXT_CONFIGURATION = "SANDBOX OPTIONS";

static const char* TEXT_ENTRY_SYSTEM = "SYSTEM";
static const char* TEXT_OPTION_P_CENTAURI = "P CENTAURI";
static const char* TEXT_OPTION_RAN = "RAN";
static const char* TEXT_OPTION_LALANDE = "LALANDE";

static const char* TEXT_ENTRY_ENEMY = "ENEMY";
static const char* TEXT_OPTION_ALIEN = "ALIEN";
static const char* TEXT_OPTION_BUBBLE = "BUBBLE";
static const char* TEXT_OPTION_CROSS = "CROSS";
static const char* TEXT_OPTION_FALCON = "FALCON";
static const char* TEXT_OPTION_FIGHTER = "FIGHTER";
static const char* TEXT_OPTION_METEORITE = "METEORITE";
static const char* TEXT_OPTION_OWL = "OWL";
static const char* TEXT_OPTION_SAUCER = "SAUCER";

static const char* TEXT_ENTRY_LIVES = "LIVES";
static const char* TEXT_OPTION_FIVE = "5";
static const char* TEXT_OPTION_NINETY_NINE = "99";

static const char* TEXT_ENTRY_IMMUNITY = "IMMUNITY";
static const char* TEXT_OPTION_DISABLED = "OFF";
static const char* TEXT_OPTION_ENABLED = "ON";

static const char* TEXT_ENTRY_NUKE = "NUKE";

static const char* TEXT_ENTRY_MAX_ENEMIES = "MAX ENEMIES";
static const char* TEXT_OPTION_ZERO = "0";
static const char* TEXT_OPTION_ONE = "1";
static const char* TEXT_OPTION_TEN = "10";
static const char* TEXT_OPTION_TWENTY_FIVE = "25";

static const char* TEXT_ENTRY_ENEMY_SPEED = "ENEMY SPEED";
static const char* TEXT_OPTION_SLOW = "SLOW";
static const char* TEXT_OPTION_NORMAL = "NORMAL";
static const char* TEXT_OPTION_FAST = "FAST";

static const char* TEXT_ENTRY_LIMIT_AMMO = "LIMIT AMMO";
static const char* TEXT_ENTRY_AMMO = "AMMO";

static const char* TEXT_ENTRY_BACK = "BACK";

static void createPlanetEntry(MenuEntry* entry);
static void createEnemyEntry(MenuEntry* entry);
static void createLivesEntry(MenuEntry* entry);
static void createImmunityEntry(MenuEntry* entry);
static void createNukeEntry(MenuEntry* entry);
static void createMaxEnemiesEntry(MenuEntry* entry);
static void createEnemySpeedEntry(MenuEntry* entry);
static void createLimitAmmoEntry(MenuEntry* entry);
static void createAmmoEntry(MenuEntry* entry);
static void createBackEntry(MenuEntry* entry);

static void expandSandboxConfig(void);

static MenuView config_view;

static const V2u16 pos_init = { .x = 8, .y = 6 };

static MenuEntry *lives_entry = NULL;
static MenuEntry *immunity_entry = NULL;
static MenuEntry *limit_ammo_entry = NULL;
static MenuEntry *ammo_entry = NULL;
static MenuEntry *nuke_entry = NULL;
static MenuEntry *planet_entry = NULL;
static MenuEntry *enemy_entry = NULL;
static MenuEntry *enemy_speed_entry = NULL;
static MenuEntry *max_enemies_entry = NULL;
static MenuEntry *back_entry = NULL;

void CONFIG_SANDBOX_init(void) {

	config_view.compact = TRUE;

	config_view.num_entries = 10;
	config_view.current_entry = 0;
	config_view.entries = MEM_calloc(sizeof(MenuEntry) * config_view.num_entries);

	lives_entry = &config_view.entries[0];
	createLivesEntry(lives_entry);

	immunity_entry = &config_view.entries[1];
	createImmunityEntry(immunity_entry);

	limit_ammo_entry = &config_view.entries[2];
	createLimitAmmoEntry(limit_ammo_entry);

	ammo_entry = &config_view.entries[3];
	createAmmoEntry(ammo_entry);

	nuke_entry = &config_view.entries[4];
	createNukeEntry(nuke_entry);

	planet_entry = &config_view.entries[5];
	createPlanetEntry(planet_entry);

	enemy_entry = &config_view.entries[6];
	createEnemyEntry(enemy_entry);

	enemy_speed_entry = &config_view.entries[7];
	createEnemySpeedEntry(enemy_speed_entry);

	max_enemies_entry = &config_view.entries[8];
	createMaxEnemiesEntry(max_enemies_entry);

	back_entry = &config_view.entries[9];
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
	entry->num_options = 3;
	entry->options = MEM_calloc(sizeof(ConfigOption) * entry->num_options);
	entry->current_option = 0;

	CONFIG_setOption(&entry->options[0], TEXT_OPTION_P_CENTAURI, SANDBOX_SYSTEM_P_CENTAURI, NULL);
	CONFIG_setOption(&entry->options[1], TEXT_OPTION_RAN, SANDBOX_SYSTEM_RAN, NULL);
	CONFIG_setOption(&entry->options[2], TEXT_OPTION_LALANDE, SANDBOX_SYSTEM_LALANDE, NULL);
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

static void createLivesEntry(MenuEntry* entry) {

	entry->type = ENTRY_CONFIG;
	entry->text = TEXT_ENTRY_LIVES;
	entry->text_pos = MARGIN_L1;
	entry->num_options = 3;
	entry->options = MEM_calloc(sizeof(ConfigOption) * entry->num_options);
	entry->current_option = 1;

	CONFIG_setOption(&entry->options[0], TEXT_OPTION_ONE, 1, NULL);
	CONFIG_setOption(&entry->options[1], TEXT_OPTION_FIVE, 5, NULL);
	CONFIG_setOption(&entry->options[2], TEXT_OPTION_NINETY_NINE, 99, NULL);
}

static void createImmunityEntry(MenuEntry* entry) {

	entry->type = ENTRY_CONFIG;
	entry->text = TEXT_ENTRY_IMMUNITY;
	entry->text_pos = MARGIN_L1;
	entry->num_options = 2;
	entry->options = MEM_calloc(sizeof(ConfigOption) * entry->num_options);
	entry->current_option = 0;

	CONFIG_setOption(&entry->options[0], TEXT_OPTION_DISABLED, FALSE, NULL);
	CONFIG_setOption(&entry->options[1], TEXT_OPTION_ENABLED, TRUE, NULL);
}

static void createNukeEntry(MenuEntry* entry) {

	entry->type = ENTRY_CONFIG;
	entry->text = TEXT_ENTRY_NUKE;
	entry->text_pos = MARGIN_L1;
	entry->num_options = 2;
	entry->options = MEM_calloc(sizeof(ConfigOption) * entry->num_options);
	entry->current_option = 0;

	CONFIG_setOption(&entry->options[0], TEXT_OPTION_DISABLED, FALSE, NULL);
	CONFIG_setOption(&entry->options[1], TEXT_OPTION_ENABLED, TRUE, NULL);
}

static void createMaxEnemiesEntry(MenuEntry* entry) {

	entry->type = ENTRY_CONFIG;
	entry->text = TEXT_ENTRY_MAX_ENEMIES;
	entry->text_pos = MARGIN_L1;
	entry->num_options = 4;
	entry->options = MEM_calloc(sizeof(ConfigOption) * entry->num_options);
	entry->current_option = 2;

	CONFIG_setOption(&entry->options[0], TEXT_OPTION_ZERO, 0, NULL);
	CONFIG_setOption(&entry->options[1], TEXT_OPTION_ONE, 1, NULL);
	CONFIG_setOption(&entry->options[2], TEXT_OPTION_TEN, 10, NULL);
	CONFIG_setOption(&entry->options[3], TEXT_OPTION_TWENTY_FIVE, 25, NULL);
}

static void createEnemySpeedEntry(MenuEntry* entry) {

	entry->type = ENTRY_CONFIG;
	entry->text = TEXT_ENTRY_ENEMY_SPEED;
	entry->text_pos = MARGIN_L1;
	entry->num_options = 3;
	entry->options = MEM_calloc(sizeof(ConfigOption) * entry->num_options);
	entry->current_option = 1;

	CONFIG_setOption(&entry->options[0], TEXT_OPTION_SLOW, 0, NULL);
	CONFIG_setOption(&entry->options[1], TEXT_OPTION_NORMAL, 1, NULL);
	CONFIG_setOption(&entry->options[2], TEXT_OPTION_FAST, 2, NULL);
}

static void createLimitAmmoEntry(MenuEntry* entry) {

	entry->type = ENTRY_CONFIG;
	entry->text = TEXT_ENTRY_LIMIT_AMMO;
	entry->text_pos = MARGIN_L1;
	entry->num_options = 2;
	entry->options = MEM_calloc(sizeof(ConfigOption) * entry->num_options);
	entry->current_option = 0;

	CONFIG_setOption(&entry->options[0], TEXT_OPTION_DISABLED, FALSE, NULL);
	CONFIG_setOption(&entry->options[1], TEXT_OPTION_ENABLED, TRUE, NULL);
}

static void createAmmoEntry(MenuEntry* entry) {

	entry->type = ENTRY_CONFIG;
	entry->text = TEXT_ENTRY_AMMO;
	entry->text_pos = MARGIN_L1;
	entry->num_options = 4;
	entry->options = MEM_calloc(sizeof(ConfigOption) * entry->num_options);
	entry->current_option = 3;

	CONFIG_setOption(&entry->options[0], TEXT_OPTION_ZERO, 0, NULL);
	CONFIG_setOption(&entry->options[1], TEXT_OPTION_TEN, 10, NULL);
	CONFIG_setOption(&entry->options[2], TEXT_OPTION_TWENTY_FIVE, 25, NULL);
	CONFIG_setOption(&entry->options[3], TEXT_OPTION_NINETY_NINE, 99, NULL);
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

	// Game config
	sandbox_config.lives = lives_entry->options[lives_entry->current_option].value;
	sandbox_config.immunity = immunity_entry->options[immunity_entry->current_option].value;
	sandbox_config.limited_ammo = limit_ammo_entry->options[limit_ammo_entry->current_option].value;
	sandbox_config.allow_nuke = nuke_entry->options[nuke_entry->current_option].value;
	
	// Planet def
	sandbox_config.system = planet_entry->options[planet_entry->current_option].value;
	sandbox_config.enemy = enemy_entry->options[enemy_entry->current_option].value;
	sandbox_config.enemy_speed = enemy_speed_entry->options[enemy_speed_entry->current_option].value;
	sandbox_config.max_enemies = max_enemies_entry->options[max_enemies_entry->current_option].value;
	sandbox_config.ammo = ammo_entry->options[ammo_entry->current_option].value;
}

