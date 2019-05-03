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

static u8 axisXBoxRelativePos(Box_s16, Box_s16);
static u8 axisYBoxRelativePos(Box_s16, Box_s16);
static u8 axisXPxRelativePos(s16, Box_s16);
static u8 axisYPxRelativePos(s16, Box_s16);

void updateBox(Object_f16* object) {

	object->box.pos.x = fix16ToInt(object->pos.x);
	object->box.pos.y = fix16ToInt(object->pos.y);
}

Box_s16 targetBox(Object_f16 object, u8 width, u8 height) {

	fix16 target_x_f16 = fix16Add(object.pos.x, object.mov.x);
	fix16 target_y_f16 = fix16Add(object.pos.y, object.mov.y);
	Box_s16 box = { .w = width, .h = height };
	box.pos.x = fix16ToInt(target_x_f16);
	box.pos.y = fix16ToInt(target_y_f16);

	return box;
}

Box_s16 targetHBox(Object_f16 object, u8 width, u8 height) {

	fix16 target_x_f16 = fix16Add(object.pos.x, object.mov.x);
	Box_s16 box = { .w = width, .h = height };
	box.pos.x = fix16ToInt(target_x_f16);
	box.pos.y = fix16ToInt(object.pos.y);

	return box;
}

Box_s16 targetVBox(Object_f16 object, u8 width, u8 height) {

	fix16 target_y_f16 = fix16Add(object.pos.y, object.mov.y);
	Box_s16 box = { .w = width, .h = height };
	box.pos.x = fix16ToInt(object.pos.x);
	box.pos.y = fix16ToInt(target_y_f16);

	return box;
}

u8 overlap(Box_s16 subject_box, Box_s16 object_box) {

	return (OVERLAPPED & axisXBoxRelativePos(subject_box, object_box))
			&& (OVERLAPPED & axisYBoxRelativePos(subject_box, object_box));
}

u8 hitAbove(Box_s16 subject_box, Box_s16 object_box) {

	if (OVERLAPPED & axisXBoxRelativePos(subject_box, object_box)) {

		if (IN_BETWEEN & axisYPxRelativePos(subject_box.pos.y + subject_box.h, object_box)) {
			return TRUE;
		}
	}

	return FALSE;
}

u8 hitUnder(Box_s16 subject_box, Box_s16 object_box) {

	if (OVERLAPPED & axisXBoxRelativePos(subject_box, object_box)) {
		if (IN_BETWEEN & axisYPxRelativePos(subject_box.pos.y, object_box)) {
			return TRUE;
		}
	}

	return FALSE;
}

u8 hitLeft(Box_s16 subject_box, Box_s16 object_box) {

	if (OVERLAPPED & axisYBoxRelativePos(subject_box, object_box)) {

		s16 subject_right_edge = subject_box.pos.x + subject_box.w;

		if (IN_BETWEEN & axisXPxRelativePos(subject_right_edge, object_box)) {
			return TRUE;
		}
	}

	return FALSE;
}

u8 hitRight(Box_s16 subject_box, Box_s16 object_box) {

	if (OVERLAPPED & axisYBoxRelativePos(subject_box, object_box)) {
		if (IN_BETWEEN & axisXPxRelativePos(subject_box.pos.x, object_box)) {
			return TRUE;
		}
	}

	return FALSE;
}

fix16 adjacentYAbove(Box_s16 subject_box, Box_s16 object_box) {
	return FIX16(object_box.pos.y - subject_box.h);
}

fix16 adjacentYUnder(Box_s16 subject_box, Box_s16 object_box) {
	return FIX16(object_box.pos.y + object_box.h);
}

fix16 adjacentXOnTheLeft(Box_s16 subject_box, Box_s16 object_box) {
	return FIX16(object_box.pos.x - subject_box.w);
}

fix16 adjacentXOnTheRight(Box_s16 subject_box, Box_s16 object_box) {
	return FIX16(object_box.pos.x + object_box.w);
}

static u8 axisXBoxRelativePos(Box_s16 subject_box, Box_s16 object_box) {

	if (TO_THE_RIGHT & axisXPxRelativePos(subject_box.pos.x, object_box)) {
		return TO_THE_RIGHT;
	}

	if (TO_THE_LEFT & axisXPxRelativePos(subject_box.pos.x + subject_box.w, object_box)) {
		return TO_THE_LEFT;
	}

	return OVERLAPPED;
}

static u8 axisYBoxRelativePos(Box_s16 subject_box, Box_s16 object_box) {

	if (UNDER & axisYPxRelativePos(subject_box.pos.y, object_box)) {
		return UNDER;
	}

	if (ABOVE & axisYPxRelativePos(subject_box.pos.y + subject_box.h, object_box)) {
		return ABOVE;
	}

	return OVERLAPPED;
}

static u8 axisXPxRelativePos(s16 x_px, Box_s16 object_box) {

	s16 x_s16 = object_box.pos.x;
	if (x_px <= x_s16) {
		return TO_THE_LEFT;
	}

	if (x_px >= x_s16 + object_box.w) {
		return TO_THE_RIGHT;
	}

	return IN_BETWEEN;
}

static u8 axisYPxRelativePos(s16 y_px, Box_s16 object_box) {

	s16 y_s16 = object_box.pos.y;
	if (y_px <= y_s16) {
		return ABOVE;
	}

	if (y_px >= y_s16 + object_box.h) {
		return UNDER;
	}

	return IN_BETWEEN;
}

