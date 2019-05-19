/*
 * items.c
 *
 *  Created on: May 19, 2019
 *      Author: diegomtassis
 */

#include "../inc/items.h"

#include "../inc/commons.h"
#include "../inc/physics.h"

void dropFromSky(Object_f16* item, Box_s16* avoid) {

	item->pos.y = TOP_POS_V_PX_F16;
	item->size.x = ITEM_WIDTH;
	item->size.y = ITEM_HEIGHT;
	item->box.w = ITEM_WIDTH;
	item->box.h = ITEM_HEIGHT;
	item->mov.y = SPEED_V_DOWN;

	do {
		item->pos.x = randomInRangeFix16(ITEM_MIN_POS_H_PX_F16, ITEM_MAX_POS_H_PX_F16);
		updateBox(item);
	} while (avoid && isAbove(item->box, *avoid));
}

bool grab(Object_f16* subject, Object_f16* item) {

	if (overlap(subject->box, item->box)) {
		item->mov.y = SPEED_0;
		return TRUE;
	}

	return FALSE;
}
