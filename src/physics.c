/*
 * physics.c
 *
 *  Created on: Apr 19, 2019
 *      Author: diegomtassis
 */

#include "../inc/physics.h"

// relative position
#define IN_BETWEEN		0x01
#define OVERLAPPED			0x01
#define TO_THE_LEFT		0x02
#define TO_THE_RIGHT	0x04
#define ABOVE			0x08
#define UNDER			0x10

static u8 axisXBoxRelativePos(Box_f16, Box_f16);
static u8 axisYBoxRelativePos(Box_f16, Box_f16);
static u8 axisXPxRelativePos(fix16, Box_f16);
static u8 axisYPxRelativePos(fix16, Box_f16);

void updateBox(Object_f16* object) {

	object->box->x = object->pos.x;
	object->box->y = object->pos.y;
}

fix16 hitAbove(Box_f16 subject, Box_f16 object) {

	if (OVERLAPPED & axisXBoxRelativePos(subject, object)) {

		fix16 subject_h_f16 = FIX16(subject.h);
		fix16 subject_bottom = subject.y + subject_h_f16;

		if (IN_BETWEEN & axisYPxRelativePos(subject_bottom, object)) {
			return object.y - subject_h_f16;
		}
	}

	return 0;
}

fix16 hitUnder(Box_f16 subject, Box_f16 object) {

	if (OVERLAPPED & axisXBoxRelativePos(subject, object)) {
		if (IN_BETWEEN & axisYPxRelativePos(subject.y, object)) {
			return object.y + FIX16(object.h);
		}
	}

	return 0;
}

fix16 hitLeft(Box_f16 subject, Box_f16 object) {

	return 0;
}

fix16 hitRight(Box_f16 subject, Box_f16 object) {

	return 0;
}

static u8 axisXBoxRelativePos(Box_f16 subject, Box_f16 object) {

	u8 leftEdgePos = axisXPxRelativePos(subject.x, object);
	u8 rightEdgePos = axisXPxRelativePos(subject.x + FIX16(subject.w), object);

	if (leftEdgePos & TO_THE_RIGHT) {
		return TO_THE_RIGHT;
	}

	if (rightEdgePos & TO_THE_LEFT) {
		return TO_THE_LEFT;
	}

	return OVERLAPPED;
}

static u8 axisYBoxRelativePos(Box_f16 subject, Box_f16 object) {

	u8 topEdgePos = axisYPxRelativePos(subject.y, object);
	u8 bottomEdgePos = axisYPxRelativePos(subject.y + FIX16(subject.h), object);

	if (topEdgePos & UNDER) {
		return UNDER;
	}

	if (bottomEdgePos & ABOVE) {
		return ABOVE;
	}

	return OVERLAPPED;
}

static u8 axisXPxRelativePos(fix16 x_px, Box_f16 object) {

	if (x_px < object.x) {
		return TO_THE_LEFT;
	}

	if (x_px > object.x + FIX16(object.w)) {
		return TO_THE_RIGHT;
	}

	return IN_BETWEEN;
}

static u8 axisYPxRelativePos(fix16 y_px, Box_f16 object) {

	if (y_px < object.y) {
		return ABOVE;
	}

	if (y_px > object.y + FIX16(object.h)) {
		return UNDER;
	}

	return IN_BETWEEN;
}

