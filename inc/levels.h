/*
 * levels.h
 *
 *  Created on: Apr 7, 2019
 *      Author: diegomtassis
 */

#ifndef INC_LEVELS_H_
#define INC_LEVELS_H_

#include "elements.h"

Level* allocLevel();
void startLevel(Level* level);
void releaseLevel(Level*);

Platform* createPlatform(u16 pos_x_t, u16 pos_y_t, u16 length_t);

#endif /* INC_LEVELS_H_ */
