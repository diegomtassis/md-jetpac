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

typedef struct {
	u8 mode;
	u8 lives;
	u8 level;
	u16 score;
	Level* (**createLevel)(void);
	u8 num_levels;
} Game;

void runGame(Game*);
void releaseGame(Game* game);

#endif /* INC_GAME_H_ */
