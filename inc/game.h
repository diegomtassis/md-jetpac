/*
 * game.h
 *
 *  Created on: Apr 20, 2019
 *      Author: diegomtassis
 */

#ifndef INC_GAME_H_
#define INC_GAME_H_

#include <genesis.h>
#include "elements.h"

typedef enum {
	KILLED_ENEMY_01, //
	KILLED_ENEMY_02, //
	KILLED_ENEMY_03, //
	GRABBED_SPACESHIP_PART, //
	GRABBED_FUEL, //
	GRABBED_BONUS, //
} GameEvent;

void runGame(Game* game);
void releaseGame(Game* game);

void score(Game* game, GameEvent event);

#endif /* INC_GAME_H_ */
