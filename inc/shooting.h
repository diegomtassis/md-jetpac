/*
 * shooting.h
 *
 *  Created on: May 26, 2019
 *      Author: diegomtassis
 */

#ifndef INC_SHOOTING_H_
#define INC_SHOOTING_H_

#include "elements.h"

void initShots(Planet planet[static 1]);
void releaseShots(Planet planet[static 1]);

void shoot(u8 shooter, V2s16 where, bool to_left, Planet planet[static 1]);
void updateShots(Planet planet[static 1]);
u8 checkHit(Box_s16 subject, Planet planet[static 1]);

#endif /* INC_SHOOTING_H_ */
