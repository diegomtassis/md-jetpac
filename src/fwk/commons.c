/*
 * commons.c
 *
 *  Created on: Apr 27, 2019
 *      Author: diegomtassis
 */

#include "../../inc/fwk/commons.h"

#include <genesis.h>

void* MEM_calloc(u16 size) {

	void* ptr = MEM_alloc(size);
	memset(ptr, 0, size);

	return ptr;
}

void setV2u16(V2u16* pos, u16 x, u16 y) {

	pos->x = x;
	pos->y = y;
}

void setV2s16(V2s16* pos, s16 x, s16 y) {

	pos->x = x;
	pos->y = y;
}

f16 randomInRangeFix16(f16 lower, f16 higher) {

	int min = abs(fix16ToInt(lower));
	int max = abs(fix16ToInt(higher));

	return FIX16(min + random() % (max + 1 - min));
}

u16 randomInRangeU16(u16 lower, u16 higher) {

	return lower + random() % (higher + 1 - lower);
}

u32 randomInRangeU32(u32 lower, u32 higher) {

	return lower + random() % (higher + 1 - lower);
}
