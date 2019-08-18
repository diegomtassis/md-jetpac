/*
 * powerups.h
 *
 *  Created on: Aug 18, 2019
 *      Author: diegomtassis
 */

#ifndef INC_POWERUPS_H_
#define INC_POWERUPS_H_

#include "elements.h"

void startPowerUps(Level level[static 1]);
void releasePowerUps(Level*);

void updatePowerUps(Level level[static 1]);

#endif /* INC_POWERUPS_H_ */
