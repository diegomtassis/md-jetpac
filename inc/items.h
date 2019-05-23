/*
 * items.h
 *
 *  Created on: May 19, 2019
 *      Author: diegomtassis
 */

#ifndef INC_ITEMS_H_
#define INC_ITEMS_H_

#include "fwk/physics.h"

#define ITEM_HEIGHT 16
#define ITEM_WIDTH 16

#define ITEM_MIN_POS_H_PX_S16	LEFT_POS_H_PX_S16
#define ITEM_MAX_POS_H_PX_S16	RIGHT_POS_H_PX_S16 - ITEM_WIDTH

#define ITEM_MIN_POS_H_PX_F16	FIX16(ITEM_MIN_POS_H_PX_S16)
#define ITEM_MAX_POS_H_PX_F16	FIX16(ITEM_MAX_POS_H_PX_S16)

#define SPEED_0			FIX16(0)
#define SPEED_V_DOWN	FIX16(0.9)

void dropFromSky(Object_f16* item, Box_s16* avoid);
bool grab(Object_f16* subject, Object_f16* item);

#endif /* INC_ITEMS_H_ */
