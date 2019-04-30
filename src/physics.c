/*
 * physics.c
 *
 *  Created on: Apr 19, 2019
 *      Author: diegomtassis
 */

#include "../inc/physics.h"

// relative position
#define IN_BETWEEN		0x01
#define OVERLAPPED		0x02
#define TO_THE_LEFT		0x04
#define TO_THE_RIGHT	0x08
#define ABOVE			0x10
#define UNDER			0x20

static u8 axisXBoxRelativePos(Box_f16, Box_f16);
static u8 axisYBoxRelativePos(Box_f16, Box_f16);
static u8 axisXPxRelativePos(s16, Box_f16);
static u8 axisYPxRelativePos(s16, Box_f16);

void updateBox(Object_f16* object) {

	object->box.x = object->pos.x;
	object->box.y = object->pos.y;
}

Box_f16 targetBox(const Object_f16 object, u8 width, u8 height) {

	fix16 target_x = fix16Add(object.pos.x, object.mov.x);
	fix16 target_y = fix16Add(object.pos.y, object.mov.y);
	Box_f16 box = { .x = target_x, .y = target_y, .w = width, .h = height };

	return box;
}

Box_f16 targetHBox(const Object_f16 object, u8 width, u8 height) {

	fix16 target_x = fix16Add(object.pos.x, object.mov.x);
	Box_f16 box = { .x = target_x, .y = object.pos.y, .w = width, .h = height };

	return box;
}

Box_f16 targetVBox(const Object_f16 object, u8 width, u8 height) {

	fix16 target_y = fix16Add(object.pos.y, object.mov.y);
	Box_f16 box = { .x = object.pos.x, .y = target_y, .w = width, .h = height };

	return box;
}

u8 overlap(Box_f16 subject, Box_f16 object) {

	if (OVERLAPPED & axisXBoxRelativePos(subject, object)) {
		if (IN_BETWEEN & axisYPxRelativePos(fix16ToInt(subject.y), object)) {
			return TRUE;
		}

		if (IN_BETWEEN & axisYPxRelativePos(fix16ToInt(subject.y) + subject.h, object)) {
			return TRUE;
		}
	}

	if (OVERLAPPED & axisYBoxRelativePos(subject, object)) {
		if (IN_BETWEEN & axisXPxRelativePos(fix16ToInt(subject.x), object)) {
			return TRUE;
		}

		if (IN_BETWEEN & axisXPxRelativePos(fix16ToInt(subject.x) + subject.w, object)) {
			return TRUE;
		}
	}

	return FALSE;
}

fix16 hitAbove(Box_f16 subject, Box_f16 object) {

	if (OVERLAPPED & axisXBoxRelativePos(subject, object)) {

		if (IN_BETWEEN & axisYPxRelativePos(fix16ToInt(subject.y) + subject.h, object)) {
			return fix16Sub(object.y, FIX16(subject.h));
		}
	}

	return 0;
}

fix16 hitUnder(Box_f16 subject, Box_f16 object) {

	if (OVERLAPPED & axisXBoxRelativePos(subject, object)) {
		if (IN_BETWEEN & axisYPxRelativePos(fix16ToInt(subject.y), object)) {
			return fix16Add(object.y, FIX16(object.h));
		}
	}

	return 0;
}

fix16 hitLeft(Box_f16 subject, Box_f16 object) {

	if (OVERLAPPED & axisYBoxRelativePos(subject, object)) {

		s16 subject_right_edge = fix16ToInt(subject.x) + subject.w;

		if (IN_BETWEEN & axisXPxRelativePos(subject_right_edge, object)) {
			return fix16Sub(object.x, intToFix16(subject.w));
		}
	}

	return 0;
}

fix16 hitRight(Box_f16 subject, Box_f16 object) {

	if (OVERLAPPED & axisYBoxRelativePos(subject, object)) {
		if (IN_BETWEEN & axisXPxRelativePos(fix16ToInt(subject.x), object)) {
			return fix16Add(object.x, FIX16(object.w));
		}
	}

	return 0;
}

static u8 axisXBoxRelativePos(Box_f16 subject, Box_f16 object) {

	if (TO_THE_RIGHT & axisXPxRelativePos(fix16ToInt(subject.x), object)) {
		return TO_THE_RIGHT;
	}

	if (TO_THE_LEFT & axisXPxRelativePos(fix16ToInt(subject.x) + subject.w, object)) {
		return TO_THE_LEFT;
	}

	return OVERLAPPED;
}

static u8 axisYBoxRelativePos(Box_f16 subject, Box_f16 object) {

	if (UNDER & axisYPxRelativePos(fix16ToInt(subject.y), object)) {
		return UNDER;
	}

	if (ABOVE & axisYPxRelativePos(fix16ToInt(subject.y) + subject.h, object)) {
		return ABOVE;
	}

	return OVERLAPPED;
}

static u8 axisXPxRelativePos(s16 x_px, Box_f16 object) {

	if (fix16ToInt(object.x) >= x_px) {
		return TO_THE_LEFT;
	}

	if (x_px >= fix16ToInt(object.x) + object.w) {
		return TO_THE_RIGHT;
	}

	return IN_BETWEEN;
}

static u8 axisYPxRelativePos(s16 y_px, Box_f16 object) {

	if (fix16ToInt(object.y) >= y_px) {
		return ABOVE;
	}

	if ((fix16ToInt(object.y) + object.h) <= y_px) {
		return UNDER;
	}

	return IN_BETWEEN;
}

