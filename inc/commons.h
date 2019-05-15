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
#define	FIX16_16	FIX16(16)
#define	FIX16_32	FIX16(32)

void setV2u16(V2u16* pos, u16 x, u16 y);

void setV2s16(V2s16* pos, s16 x, s16 y);

f16 randomInRangeFix16(f16 lower, f16 higher);

#endif /* INC_COMMONS_H_ */
