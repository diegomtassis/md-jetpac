/*
 * shooting.h
 *
 *  Created on: May 26, 2019
 *      Author: diegomtassis
 */

#ifndef INC_SHOOTING_H_
#define INC_SHOOTING_H_

#include "elements.h"

#define MAX_SHOTS_PER_PLAYER	3

void initShots(Planet planet[static 1]);
void releaseShots(Planet planet[static 1]);

void shoot(Jetman* shooter, Planet planet[static 1]);
void updateShots(Planet planet[static 1]);
u8 checkHit(Box_s16 subject, Planet planet[static 1]);

#endif /* INC_SHOOTING_H_ */
