/*
 * enemies.h
 *
 *  Created on: Apr 24, 2019
 *      Author: diegomtassis
 */

#ifndef INC_ENEMIES_H_
#define INC_ENEMIES_H_

#include "elements.h"

#define ENEMY_01_WIDTH    16
#define ENEMY_01_HEIGHT    11

#define ENEMY_02_WIDTH    16
#define ENEMY_02_HEIGHT    14

void startEnemies(Level* level);
void releaseEnemies(Level* level);

void enemiesAct(Level* level);
void killEnemy(Enemy* enemy, Level* level, u8 exploding);

#endif /* INC_ENEMIES_H_ */
