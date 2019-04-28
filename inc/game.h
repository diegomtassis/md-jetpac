/*
 * game.h
 *
 *  Created on: Apr 20, 2019
 *      Author: diegomtassis
 */

#ifndef INC_GAME_H_
#define INC_GAME_H_

#include <genesis.h>

typedef struct {
	u8 lives;
	u8 level;
	u16 score;
} Game;

void startGame(Game*);

#endif /* INC_GAME_H_ */
