/*
 * enemies.h
 *
 *  Created on: Apr 24, 2019
 *      Author: diegomtassis
 */

#ifndef INC_ENEMIES_H_
#define INC_ENEMIES_H_

#include "elements.h"

#define METEORITE	1
#define ALIEN		2
#define BUBBLE		3
#define FIGHTER		4
#define SAUCER		5
#define CROSS		6
#define FALCON		7
#define OWL			8

#define METEORITE_WIDTH		16
#define METEORITE_HEIGHT  	12

#define ALIEN_WIDTH    		16
#define ALIEN_HEIGHT    	14

#define BUBBLE_WIDTH    	16
#define BUBBLE_HEIGHT    	14

typedef struct {
	Enemy* enemy;
} Meteorite;

typedef struct {
	Enemy* enemy;
} Alien;

typedef struct {
	Enemy* enemy;
} Bubble;

void startEnemies(Level level[static 1]);
void releaseEnemies(Level level[static 1]);

void enemiesAct(Level level[static 1]);
void killEnemy(Enemy* enemy, Level level[static 1], u8 exploding);

Enemy* createMeteorite(EnemyDefinition definition[static 1]);
void actMeteorite(Enemy enemy[static 1], Level level[static 1]);
void releaseMeteorite(Enemy enemy[static 1]);

Enemy* createAlien(EnemyDefinition definition[static 1]);
void actAlien(Enemy enemy[static 1], Level level[static 1]);
void releaseAlien(Enemy enemy[static 1]);

Enemy* createBubble(EnemyDefinition definition[static 1]);
void actBubble(Enemy enemy[static 1], Level level[static 1]);
void releaseBubble(Enemy enemy[static 1]);

#endif /* INC_ENEMIES_H_ */
