/*
 * explosions.h
 *
 *  Created on: May 4, 2019
 *      Author: diegomtassis
 */

#ifndef INC_EXPLOSIONS_H_
#define INC_EXPLOSIONS_H_

#include <genesis.h>

#include "elements.h"

#define BOOM_ANIMATION_SPEED SUBTICKPERSECOND / 16

void initExplosions(Planet planet[static 1]);
void updateExplosions(Planet planet[static 1]);
void releaseExplosions(Planet planet[static 1]);

void explode(Box_s16 what, Planet planet[static 1]);

void boost(Box_s16 what, Planet planet[static 1]);

#endif /* INC_EXPLOSIONS_H_ */
