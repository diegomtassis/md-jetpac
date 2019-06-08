/*
 * game.h
 *
 *  Created on: Apr 20, 2019
 *      Author: diegomtassis
 */

#ifndef INC_GAME_H_
#define INC_GAME_H_

#include "elements.h"
#include "events.h"

void runGame(Game* game);
void releaseGame(Game* game);

void score(GameEvent event);

#endif /* INC_GAME_H_ */
