/*
 * collectables.h
 *
 *  Created on: Aug 18, 2019
 *      Author: diegomtassis
 */

#ifndef INC_COLLECTABLES_H_
#define INC_COLLECTABLES_H_

#include "elements.h"

#define GOLD    1
#define BEAN    2
#define NUKE    3

#define COLLECTABLE_HEIGHT 8
#define COLLECTABLE_WIDTH 16

#define COLLECTABLE_MIN_POS_H_PX_S16	LEFT_POS_H_PX_S16
#define COLLECTABLE_MAX_POS_H_PX_S16	RIGHT_POS_H_PX_S16 - COLLECTABLE_WIDTH

#define COLLECTABLE_MIN_POS_H_PX_F16	FIX16(COLLECTABLE_MIN_POS_H_PX_S16)
#define COLLECTABLE_MAX_POS_H_PX_F16	FIX16(COLLECTABLE_MAX_POS_H_PX_S16)

void startCollectables(Level level[static 1]);
void releaseCollectables(Level level[static 1]);

void updateCollectables(Level level[static 1]);

#endif /* INC_COLLECTABLES_H_ */
