/*
 * levels.h
 *
 *  Created on: Apr 7, 2019
 *      Author: diegomtassis
 */

#ifndef INC_LEVELS_H_
#define INC_LEVELS_H_

#include <genesis.h>

#include "physics.h"

Level* createLevel();

void startLevel(const Level* level);

#endif /* INC_LEVELS_H_ */
