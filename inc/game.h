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

struct Game {
	Player* p1;
	Player* p2;
	Planet* planet;
};

GameResult GAME_run(const Config *config);

void GAME_scoreByEvent(GameEvent event, u8 player_id);

extern Game game;

#endif /* INC_GAME_H_ */
