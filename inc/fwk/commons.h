/*
 * commons.h
 *
 *  Created on: Apr 27, 2019
 *      Author: diegomtassis
 */

#ifndef INC_COMMONS_H_
#define INC_COMMONS_H_

#include <genesis.h>

#define	FIX16_0		FIX16(0)
#define	FIX16_8		FIX16(8)
#define	FIX16_12	FIX16(12)
#define	FIX16_16	FIX16(16)
#define	FIX16_32	FIX16(32)

void* MEM_calloc(u16 size);

void setV2s16(V2s16* pos, s16 x, s16 y);

f16 randomInRangeFix16(f16 lower, f16 higher);

u16 randomInRangeU16(u16 lower, u16 higher);

u32 randomInRangeU32(u32 lower, u32 higher);

#endif /* INC_COMMONS_H_ */
