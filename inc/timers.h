/*
 * timers.h
 *
 *  Created on: May 4, 2019
 *      Author: diegomtassis
 */

#ifndef INC_TIMERS_H_
#define INC_TIMERS_H_

#include <genesis.h>

#define ENEMY_CREATION_TIMER 0
#define EXPLOSIONS_TIMER 1
#define COLLECTABLE_CREATION_TIMER 2

void startCountDown(u16 numTimer, u32 subticks);

void startCountDownRandom(u16 numTimer, u32 minSubticks, u32 maxSubticks);

bool isCountDownFinished(u16 numTimer, bool restart);

#endif /* INC_TIMERS_H_ */
