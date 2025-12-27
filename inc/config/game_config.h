/*
 * game_config.h
 *
 *  Created on: May 5, 2019
 *      Author: diegomtassis
 */

#ifndef INC_CONFIG_GAME_CONFIG_H_
#define INC_CONFIG_GAME_CONFIG_H_

#include <genesis.h>

#include "../elements.h"
#include "../game.h"

typedef enum {
	MODE_ZX, //
	MODE_MD, //
	MODE_SANDBOX,
} Mode;

typedef enum {
	DIFFICULTY_EASY, //
	DIFFICULTY_NORMAL, //
	DIFFICULTY_HARD, //
	DIFFICULTY_MANIAC,
} Difficulty;

typedef enum {
	ONE_PLAYER = 1, //
	TWO_PLAYERS,
} Players;

struct GameConfig {
	u8 players;
	u8 lives;
	bool bonus;
    bool immunity;
	bool limited_ammo;
    bool allow_nuke;
	Planet* (*const* createPlanet)(void);
	u8 num_planets;
};

extern GameConfig game_config;

void CONFIG_GAME_init(void);
void CONFIG_GAME_setUp(void);

#endif /* INC_CONFIG_GAME_CONFIG_H_ */
