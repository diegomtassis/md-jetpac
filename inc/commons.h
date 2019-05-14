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

void setV2u16(V2u16* pos, u16 x, u16 y);

f16 randomInRangeFix16(f16 lower, f16 higher);

#endif /* INC_COMMONS_H_ */
