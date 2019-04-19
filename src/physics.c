/*
 * physics.c
 *
 *  Created on: Apr 19, 2019
 *      Author: diegomtassis
 */

#include "../inc/physics.h"

void updateBox(Object_f16* object) {

	object->box->x = object->pos.x;
	object->box->y = object->pos.y;
}

u8 reachedBottonF16(Box_f16 subject, Box_f16 object) {

	return 0;
}

u8 reachedTopF16(Box_f16 subject, Box_f16 object) {

	return 0;
}

u8 blockedByLeftF16(Box_f16 subject, Box_f16 object) {

	return 0;
}

u8 blockedByRightF16(Box_f16 subject, Box_f16 object) {

	return 0;
}
