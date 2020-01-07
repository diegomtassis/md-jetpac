/*
 * items.c
 *
 *  Created on: May 19, 2019
 *      Author: diegomtassis
 */

#include "../inc/items.h"

#include "../inc/fwk/commons.h"
#include "../inc/fwk/physics.h"

void dropFromSky(Object_f16* object, const Box_s16* avoid, u16 itemWidth, u16 itemHeight, f16 minHPos, f16 maxHPos) {

	object->pos.y = TOP_POS_V_PX_F16;
	object->size.x = itemWidth;
	object->size.y = itemHeight;
	object->box.w = itemWidth;
	object->box.h = itemHeight;
	object->mov.y = SPEED_V_DOWN;

	do {
		object->pos.x = randomInRangeFix16(minHPos, maxHPos);
		updateBox(object);
	} while (avoid && isAboveBaseUpwardProjection(&object->box, avoid));
}

bool grab(Object_f16* subject, Object_f16* item) {

	if (overlap(&subject->box, &item->box)) {
		item->mov.y = SPEED_0;
		return TRUE;
	}

	return FALSE;
}
