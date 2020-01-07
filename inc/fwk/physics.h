/*
 * physics.h
 *
 *  Created on: Apr 19, 2019
 *      Author: diegomtassis
 */

#ifndef INC_PHYSICS_H_
#define INC_PHYSICS_H_

#include <genesis.h>

#define LEFT_POS_H_PX_S16    	0
#define RIGHT_POS_H_PX_S16  	256
#define TOP_POS_V_PX_S16 		32
#define BOTTOM_POS_V_PX_S16 	224

#define LEFT_POS_H_PX_F16    	FIX16(LEFT_POS_H_PX_S16)
#define RIGHT_POS_H_PX_F16  	FIX16(RIGHT_POS_H_PX_S16)
#define TOP_POS_V_PX_F16 		FIX16(TOP_POS_V_PX_S16)
#define BOTTOM_POS_V_PX_F16 	FIX16(BOTTOM_POS_V_PX_S16)

#define IN_BETWEEN		0x01
#define OVERLAPPED		0x02
#define TO_THE_LEFT		0x04
#define TO_THE_RIGHT	0x08
#define ABOVE			0x10
#define UNDER			0x20

typedef struct {
	V2s16 min;
	V2s16 max;
	u16 w;
	u16 h;
} Box_s16;

typedef struct {
	V2f16 pos;
	V2u16 size;
	V2f16 mov;
	Box_s16 box;
} Object_f16;

void updateBox(Object_f16* object);
void updateBoxMax(Box_s16* box);

Box_s16 targetBox(const Object_f16* subject);
Box_s16 targetHBox(const Object_f16* subject);
Box_s16 targetVBox(const Object_f16* subject);

bool contained(V2s16 subject_pos, const Box_s16* object_box);
bool overlap(const Box_s16* subject_box, const Box_s16* object_box);

bool isAboveBaseUpwardProjection(const Box_s16* subject_box, const Box_s16* object_box);
bool shareBase(const Box_s16* subject_box, const Box_s16* object_box);

bool hitAbove(const Box_s16* subject_box, const Box_s16* object_box);
bool hitUnder(const Box_s16* subject_box, const Box_s16* object_box);
bool hitLeft(const Box_s16* subject_box, const Box_s16* object_box);
bool hitRight(const Box_s16* subject_box, const Box_s16* object_box);

s16 adjacentYAbove(const Box_s16* subject_box, const Box_s16* object_box);
s16 adjacentYUnder(const Box_s16* subject_box, const Box_s16* object_box);
s16 adjacentXOnTheLeft(const Box_s16* subject_box, const Box_s16* object_box);
s16 adjacentXOnTheRight(const Box_s16* subject_box, const Box_s16* object_box);

u8 axisXBoxRelativePos(const Box_s16*, const Box_s16*);
u8 axisYBoxRelativePos(const Box_s16*, const Box_s16*);
u8 axisXPxRelativePos(s16, const Box_s16*);
u8 axisYPxRelativePos(s16, const Box_s16*);

#endif /* INC_PHYSICS_H_ */
