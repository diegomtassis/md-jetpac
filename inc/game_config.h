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

void initConfig();
Config setUpGame();

#endif /* INC_GAME_CONFIG_H_ */
