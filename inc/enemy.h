/*
 * enemy.h
 *
 *  Created on: Nov 13, 2019
 *      Author: diegomtassis
 */

#ifndef INC_ENEMY_H_
#define INC_ENEMY_H_

#include "../inc/constants.h"

#include "elements.h"

#define ENEMY_DEFAULT_MIN_POS_H_PX_S16			LEFT_POS_H_PX_S16 + 8
#define ENEMY_DEFAULT_MAX_POS_H_PX_S16			RIGHT_POS_H_PX_S16 - 8
#define ENEMY_DEFAULT_MAX_POS_V_PX_S16			BOTTOM_POS_V_PX_S16 - 16

#define ENEMY_DEFAULT_MIN_POS_H_PX_F16			FIX16(ENEMY_DEFAULT_MIN_POS_H_PX_S16)
#define ENEMY_DEFAULT_MAX_POS_H_PX_F16			FIX16(ENEMY_DEFAULT_MAX_POS_H_PX_S16)
#define ENEMY_DEFAULT_MAX_POS_V_PX_F16			FIX16(MAX_POS_V_PX_S16)
#define ENEMY_DEFAULT_MAX_POS_START_V_PX_F16	FIX16(ENEMY_DEFAULT_MAX_POS_V_PX_S16 - 32)

Enemy* createEnemy(const EnemyDefinition definition[static 1]);
void releaseEnemy(Enemy enemy[static 1]);

void initPosAndMov(Enemy enemy[static 1], fix16 mov_x, fix16 mov_y);
void initBox(Enemy enemy[static 1]);

void updatePosition(Enemy* enemy, Box_s16 target);
bool crashedIntoPlatform(Box_s16 subject_box, const Planet planet[static 1]);

#endif /* INC_ENEMIES_H_ */
