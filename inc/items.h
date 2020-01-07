/*
 * items.h
 *
 *  Created on: May 19, 2019
 *      Author: diegomtassis
 */

#ifndef INC_ITEMS_H_
#define INC_ITEMS_H_

#include "fwk/physics.h"

#define ITEM_DEFAULT_HEIGHT 16
#define ITEM_DEFAULT_WIDTH 16

#define ITEM_DEFAULT_MIN_POS_H_PX_S16	LEFT_POS_H_PX_S16
#define ITEM_DEFAULT_MAX_POS_H_PX_S16	RIGHT_POS_H_PX_S16 - ITEM_DEFAULT_WIDTH

#define ITEM_DEFAULT_MIN_POS_H_PX_F16	FIX16(ITEM_DEFAULT_MIN_POS_H_PX_S16)
#define ITEM_DEFAULT_MAX_POS_H_PX_F16	FIX16(ITEM_DEFAULT_MAX_POS_H_PX_S16)

#define SPEED_0			FIX16(0)
#define SPEED_V_DOWN	FIX16(0.9)

void dropFromSky(Object_f16* object, const Box_s16* avoid, u16 itemWidth, u16 itemHeight, f16 minHPos, f16 maxHPos);

bool grab(Object_f16* subject, Object_f16* object);

#endif /* INC_ITEMS_H_ */
