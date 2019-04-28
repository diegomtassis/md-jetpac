/*
 * enemies.h
 *
 *  Created on: Apr 24, 2019
 *      Author: diegomtassis
 */

#ifndef INC_ENEMIES_H_
#define INC_ENEMIES_H_

#include "elements.h"

void startEnemies(Level* level);
void releaseDeadEnemies(Level* level);
void releaseAllEnemies(Level* level);

void enemiesAct(Level* level);

#endif /* INC_ENEMIES_H_ */
