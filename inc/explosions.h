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

#define BOOM_ANIMATION_SPEED SUBTICKPERSECOND / 32

void initExplosions(Level*);
void updateExplosions(Level*);
void releaseAllExplosions(Level*);
void releaseFinishedExplosions(Level*);

void explode(Level* level, Vect2D_f16 where);

#endif /* INC_EXPLOSIONS_H_ */
