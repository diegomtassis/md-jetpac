/*
 * shooting.h
 *
 *  Created on: May 26, 2019
 *      Author: diegomtassis
 */

#ifndef INC_SHOOTING_H_
#define INC_SHOOTING_H_

#include "elements.h"

void initShots(Level level[static 1]);
void releaseShots(Level level[static 1]);

void shoot(V2s16 where, bool right, Level level[static 1]);
void updateShots(Level level[static 1]);

#endif /* INC_SHOOTING_H_ */
