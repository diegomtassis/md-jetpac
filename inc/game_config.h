/*
 * game_config.h
 *
 *  Created on: May 5, 2019
 *      Author: diegomtassis
 */

#ifndef INC_GAME_CONFIG_H_
#define INC_GAME_CONFIG_H_

#include <genesis.h>

#include "elements.h"
#include "game.h"

typedef enum {
	ZX, //
	MD,
} Mode;

typedef enum {
	EASY, //
	NORMAL, //
	HARD, //
	MANIAC,
} Difficulty;

typedef enum {
	ONE_PLAYER = 1, //
	TWO_PLAYERS,
} Players;

struct Config {
	u8 mode;
	u8 difficulty;
	u8 players;
	u8 lives;
	bool limited_ammo;
	Planet* (*const* createPlanet)(void);
	u8 num_planets;
};

extern Config config;

void CONFIG_init(void);
void CONFIG_setUp(void);

#endif /* INC_GAME_CONFIG_H_ */
