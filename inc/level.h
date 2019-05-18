/*
 * level.h
 *
 *  Created on: Apr 7, 2019
 *      Author: diegomtassis
 */

#ifndef INC_LEVEL_H_
#define INC_LEVEL_H_

#include <genesis.h>

#include "elements.h"
#include "physics.h"

Level* allocLevel();
void startLevel(Level* level);
void releaseLevel(Level*);

Platform* createPlatform(u16 pos_x_t, u16 pos_y_t, u16 length_t);
f16 landed(Box_s16, const Level*);

#endif /* INC_LEVEL_H_ */
