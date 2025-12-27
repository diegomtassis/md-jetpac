/*
 * game_config.c
 *
 *  Created on: May 5, 2019
 *      Author: diegomtassis
 */

#include "../../inc/config/game_config.h"

#include <genesis.h>
#include <kdebug.h>

#include "../../inc/config/config.h"
#include "../../inc/config/sandbox_config.h"
#include "../../inc/elements.h"
#include "../../inc/fwk/commons.h"
#include "../../inc/planets.h"

GameConfig game_config;

static const char *TEXT_CONFIGURATION = "GAME OPTIONS";

static const char *TEXT_ENTRY_MODE = "MODE";
static const char *TEXT_OPTION_ZX = "ZX";
static const char *TEXT_OPTION_MD = "MD";
static const char *TEXT_OPTION_SANDBOX = "SANDBOX";

static const char *TEXT_ENTRY_PLAYERS = "PLAYERS";
static const char *TEXT_OPTION_ONE_PLAYER = "1";
static const char *TEXT_OPTION_TWO_PLAYERS = "2";

static const char *TEXT_ENTRY_DIFFICULTY = "DIFFICULTY";
static const char *TEXT_OPTION_EASY = "EASY";
static const char *TEXT_OPTION_NORMAL = "NORMAL";
static const char *TEXT_OPTION_HARD = "HARD";
static const char *TEXT_OPTION_MANIAC = "MANIAC";

static const char *TEXT_ENTRY_START = "START GAME";

static void createModeEntry(MenuEntry *entry);
static void createPlayersEntry(MenuEntry *entry);
static void createDifficultyEntry(MenuEntry *entry);
static void createStartEntry(MenuEntry *entry);

static void expandGameConfig(void);

static MenuView config_view;

static const V2u16 pos_init = {.x = 10, .y = 6};

static MenuEntry *mode_entry = NULL;
static MenuEntry *players_entry = NULL;
static MenuEntry *difficulty_entry = NULL;
static MenuEntry *start_entry = NULL;

static const u8 ZX_NUM_PLANETS = 16;
Planet *(*const zxCreatePlanet[16])(void) = {
    //
    createPlanetZX01, //
    createPlanetZX02, //
    createPlanetZX03, //
    createPlanetZX04, //
    createPlanetZX05, //
    createPlanetZX06, //
    createPlanetZX07, //
    createPlanetZX08, //
    createPlanetZX09, //
    createPlanetZX10, //
    createPlanetZX11, //
    createPlanetZX12, //
    createPlanetZX13, //
    createPlanetZX14, //
    createPlanetZX15, //
    createPlanetZX16, //
};

static const u8 MD_NUM_PLANETS = 5;
Planet *(*const mdCreatePlanet[5])(void) = {
    //
    createPlanetMD01, //
    createPlanetMD02, //
    createPlanetMD03, //
    createPlanetMD04, //
};

static const u8 SANDBOX_NUM_PLANETS = 1;
Planet *(*const sandboxCreatePlanet[1])(void) = {
    //
    createPlanetSandbox, //
};

void CONFIG_GAME_init(void) {

    config_view.compact = FALSE;

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

    CONFIG_SANDBOX_init();
}

void CONFIG_GAME_setUp(void) {

    menu_context.current_menu = &config_view;

    u8 prev_priority = VDP_getTextPriority();

    CONFIG_initScreen();

    menu_context.start = FALSE;
    menu_context.refresh = TRUE;

    JOY_setEventHandler(CONFIG_handleJoyEvent);

    do {
        CONFIG_displayMenu(TEXT_CONFIGURATION, pos_init);
        SYS_doVBlankProcess();
    } while (!menu_context.start);

    menu_context.start = FALSE;

    setRandomSeed(getTick());

    expandGameConfig();

    CONFIG_clearScreen();
    VDP_setTextPriority(prev_priority);
}

static void createModeEntry(MenuEntry *entry) {

    entry->type = ENTRY_CONFIG;
    entry->text = TEXT_ENTRY_MODE;
    entry->text_pos = MARGIN_L1;
    entry->num_options = 3;
    entry->options = MEM_calloc(sizeof(ConfigOption) * entry->num_options);
    entry->current_option = 0;

    CONFIG_setOption(&entry->options[0], TEXT_OPTION_ZX, MODE_ZX, NULL);
    CONFIG_setOption(&entry->options[1], TEXT_OPTION_MD, MODE_MD, NULL);
    CONFIG_setOption(&entry->options[2], TEXT_OPTION_SANDBOX, MODE_SANDBOX, CONFIG_SANDBOX_setUp);
}

static void createPlayersEntry(MenuEntry *entry) {

    entry->type = ENTRY_CONFIG;
    entry->text = TEXT_ENTRY_PLAYERS;
    entry->text_pos = MARGIN_L1;
    entry->num_options = 2;
    entry->options = MEM_calloc(sizeof(ConfigOption) * entry->num_options);
    entry->current_option = 0;

    CONFIG_setOption(&entry->options[0], TEXT_OPTION_ONE_PLAYER, ONE_PLAYER, NULL);
    CONFIG_setOption(&entry->options[1], TEXT_OPTION_TWO_PLAYERS, TWO_PLAYERS, NULL);
}

static void createDifficultyEntry(MenuEntry *entry) {

    entry->type = ENTRY_CONFIG;
    entry->text = TEXT_ENTRY_DIFFICULTY;
    entry->text_pos = MARGIN_L1;
    entry->num_options = 4;
    entry->options = MEM_calloc(sizeof(ConfigOption) * entry->num_options);
    entry->current_option = 1;

    CONFIG_setOption(&entry->options[0], TEXT_OPTION_EASY, DIFFICULTY_EASY, NULL);
    CONFIG_setOption(&entry->options[1], TEXT_OPTION_NORMAL, DIFFICULTY_NORMAL, NULL);
    CONFIG_setOption(&entry->options[2], TEXT_OPTION_HARD, DIFFICULTY_HARD, NULL);
    CONFIG_setOption(&entry->options[3], TEXT_OPTION_MANIAC, DIFFICULTY_MANIAC, NULL);
}

static void createStartEntry(MenuEntry *entry) {

    entry->type = ENTRY_START;
    entry->text = TEXT_ENTRY_START;
    entry->text_pos = MARGIN_L1;
    entry->options = NULL;
    entry->num_options = 0;
    entry->current_option = 0;
}

static void expandGameConfig(void) {

    u8 mode = mode_entry->options[mode_entry->current_option].value;
    u8 difficulty = difficulty_entry->options[difficulty_entry->current_option].value;
    game_config.players = players_entry->options[players_entry->current_option].value;

	game_config.bonus = mode == MODE_MD;

    if (mode == MODE_SANDBOX) {

		// Game
		game_config.lives = sandbox_config.lives;
		game_config.immunity = sandbox_config.immunity;
        game_config.limited_ammo = sandbox_config.limited_ammo;
		game_config.allow_nuke = sandbox_config.allow_nuke;
		
		// Planet
        game_config.num_planets = SANDBOX_NUM_PLANETS;
        game_config.createPlanet = sandboxCreatePlanet;

    } else {
		game_config.immunity = difficulty == DIFFICULTY_EASY;
		game_config.allow_nuke = FALSE;
        if (mode == MODE_ZX) {
            game_config.limited_ammo = FALSE;
            game_config.num_planets = ZX_NUM_PLANETS;
            game_config.createPlanet = zxCreatePlanet;
        } else if (mode == MODE_MD) {
            game_config.limited_ammo = TRUE;
            game_config.num_planets = MD_NUM_PLANETS;
            game_config.createPlanet = mdCreatePlanet;
        }
        switch (difficulty) {
        case DIFFICULTY_MANIAC:
            game_config.lives = 1;
            break;
        case DIFFICULTY_HARD:
            game_config.lives = 3;
            break;
        case DIFFICULTY_NORMAL:
            game_config.lives = 5;
            break;
        default: // EASY
            game_config.lives = 10;
        }
    }
}
