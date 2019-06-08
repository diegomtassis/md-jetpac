/*
 * enemies.h
 *
 *  Created on: Apr 24, 2019
 *      Author: diegomtassis
 */

#ifndef INC_ENEMIES_H_
#define INC_ENEMIES_H_

#include "elements.h"

typedef enum {
	ENEMY_01, //
	ENEMY_02, //
	ENEMY_03, //
	ENEMY_04, //
	ENEMY_05, //
} EnemyType;

#define ENEMY_01    16
#define ENEMY_01    16
#define ENEMY_01    16
#define ENEMY_01    16

#define ENEMY_01_WIDTH    16
#define ENEMY_01_HEIGHT    12

#define ENEMY_02_WIDTH    16
#define ENEMY_02_HEIGHT    14

#define ENEMY_03_WIDTH    16
#define ENEMY_03_HEIGHT    14

void startEnemies(Level level[static 1]);
void releaseEnemies(Level level[static 1]);

void enemiesAct(Level level[static 1]);
void killEnemy(Enemy* enemy, Level level[static 1], u8 exploding);

#endif /* INC_ENEMIES_H_ */
