/*
 * items.c
 *
 *  Created on: May 19, 2019
 *      Author: diegomtassis
 */

#include "../inc/items.h"

#include "../inc/commons.h"
#include "../inc/physics.h"

void initFallingItem(Object_f16* item) {

	item->pos.x = randomInRangeFix16(ITEM_MIN_POS_H_PX_F16, ITEM_MAX_POS_H_PX_F16);
	item->pos.y = TOP_POS_V_PX_F16;
	item->size.x = ITEM_WIDTH;
	item->size.y = ITEM_HEIGHT;
	item->box.w = ITEM_WIDTH;
	item->box.h = ITEM_HEIGHT;
	item->mov.y = SPEED_V_DOWN;
	updateBox(item);
}

