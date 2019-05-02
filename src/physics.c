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

Box_f16 targetBox(Object_f16 object, u8 width, u8 height) {

	fix16 target_x = fix16Add(object.pos.x, object.mov.x);
	fix16 target_y = fix16Add(object.pos.y, object.mov.y);
	Box_f16 box = { .x = target_x, .y = target_y, .w = width, .h = height };

	return box;
}

Box_f16 targetHBox(Object_f16 object, u8 width, u8 height) {

	fix16 target_x = fix16Add(object.pos.x, object.mov.x);
	Box_f16 box = { .x = target_x, .y = object.pos.y, .w = width, .h = height };

	return box;
}

Box_f16 targetVBox(Object_f16 object, u8 width, u8 height) {

	fix16 target_y = fix16Add(object.pos.y, object.mov.y);
	Box_f16 box = { .x = object.pos.x, .y = target_y, .w = width, .h = height };

	return box;
}

u8 overlap(Box_f16 subject_box, Box_f16 object_box) {

	return (OVERLAPPED & axisXBoxRelativePos(subject_box, object_box))
			&& (OVERLAPPED & axisYBoxRelativePos(subject_box, object_box));
}

u8 hitAbove(Box_f16 subject_box, Box_f16 object_box) {

	if (OVERLAPPED & axisXBoxRelativePos(subject_box, object_box)) {

		if (IN_BETWEEN & axisYPxRelativePos(fix16ToInt(subject_box.y) + subject_box.h, object_box)) {
			return TRUE;
		}
	}

	return FALSE;
}

u8 hitUnder(Box_f16 subject_box, Box_f16 object_box) {

	if (OVERLAPPED & axisXBoxRelativePos(subject_box, object_box)) {
		if (IN_BETWEEN & axisYPxRelativePos(fix16ToInt(subject_box.y), object_box)) {
			return TRUE;
		}
	}

	return FALSE;
}

u8 hitLeft(Box_f16 subject_box, Box_f16 object_box) {

	if (OVERLAPPED & axisYBoxRelativePos(subject_box, object_box)) {

		s16 subject_right_edge = fix16ToInt(subject_box.x) + subject_box.w;

		if (IN_BETWEEN & axisXPxRelativePos(subject_right_edge, object_box)) {
			return TRUE;
		}
	}

	return FALSE;
}

u8 hitRight(Box_f16 subject_box, Box_f16 object_box) {

	if (OVERLAPPED & axisYBoxRelativePos(subject_box, object_box)) {
		if (IN_BETWEEN & axisXPxRelativePos(fix16ToInt(subject_box.x), object_box)) {
			return TRUE;
		}
	}

	return FALSE;
}

fix16 adjacentYAbove(Box_f16 subject_box, Box_f16 object_box) {
	return fix16Sub(object_box.y, FIX16(subject_box.h));
}

fix16 adjacentYUnder(Box_f16 subject_box, Box_f16 object_box) {
	return fix16Add(object_box.y, FIX16(object_box.h));
}

fix16 adjacentXOnTheLeft(Box_f16 subject_box, Box_f16 object_box) {
	return fix16Sub(object_box.x, intToFix16(subject_box.w));
}

fix16 adjacentXOnTheRight(Box_f16 subject_box, Box_f16 object_box) {
	return fix16Add(object_box.x, FIX16(object_box.w));
}

static u8 axisXBoxRelativePos(Box_f16 subject_box, Box_f16 object_box) {

	if (TO_THE_RIGHT & axisXPxRelativePos(fix16ToInt(subject_box.x), object_box)) {
		return TO_THE_RIGHT;
	}

	if (TO_THE_LEFT & axisXPxRelativePos(fix16ToInt(subject_box.x) + subject_box.w, object_box)) {
		return TO_THE_LEFT;
	}

	return OVERLAPPED;
}

static u8 axisYBoxRelativePos(Box_f16 subject_box, Box_f16 object_box) {

	if (UNDER & axisYPxRelativePos(fix16ToInt(subject_box.y), object_box)) {
		return UNDER;
	}

	if (ABOVE & axisYPxRelativePos(fix16ToInt(subject_box.y) + subject_box.h, object_box)) {
		return ABOVE;
	}

	return OVERLAPPED;
}

static u8 axisXPxRelativePos(s16 x_px, Box_f16 object_box) {

	s16 x_s16 = fix16ToInt(object_box.x);
	if (x_px <= x_s16) {
		return TO_THE_LEFT;
	}

	if (x_px >= x_s16 + object_box.w) {
		return TO_THE_RIGHT;
	}

	return IN_BETWEEN;
}

static u8 axisYPxRelativePos(s16 y_px, Box_f16 object_box) {

	s16 y_s16 = fix16ToInt(object_box.y);
	if (y_px <= y_s16) {
		return ABOVE;
	}

	if (y_px >= y_s16 + object_box.h) {
		return UNDER;
	}

	return IN_BETWEEN;
}

