/*
 * commons.h
 *
 *  Created on: Apr 27, 2019
 *      Author: diegomtassis
 */

#ifndef INC_COMMONS_H_
#define INC_COMMONS_H_

#include <genesis.h>

#define	FIX16_0	FIX16(0)

void setVect2D_u16(Vect2D_u16* pos, u16 x, u16 y);

fix16 randomInRangeFix16(fix16 lower, fix16 higher);

#endif /* INC_COMMONS_H_ */
