/*
 * commons.c
 *
 *  Created on: Apr 27, 2019
 *      Author: diegomtassis
 */

#include "../inc/commons.h"

#include <genesis.h>

fix16 randomInRangeFix16(fix16 lower, fix16 higher) {

	int min = abs(fix16ToInt(lower));
	int max = abs(fix16ToInt(higher));

	return FIX16(min + random() % (max + 1 - min));
}
