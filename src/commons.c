/*
 * commons.c
 *
 *  Created on: Apr 27, 2019
 *      Author: diegomtassis
 */

#include "../inc/commons.h"

#include <genesis.h>

void setVect2D_u16(Vect2D_u16* pos, u16 x, u16 y) {

	pos->x = x;
	pos->y = y;
}

fix16 randomInRangeFix16(fix16 lower, fix16 higher) {

	int min = abs(fix16ToInt(lower));
	int max = abs(fix16ToInt(higher));

	return FIX16(min + random() % (max + 1 - min));
}
