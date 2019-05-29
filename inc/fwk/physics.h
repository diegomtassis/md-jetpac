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

typedef struct {
	V2s16 pos;
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

Box_s16 targetBox(Object_f16 subject, u8 width, u8 height);
Box_s16 targetHBox(Object_f16 subject, u8 width, u8 height);
Box_s16 targetVBox(Object_f16 subject, u8 width, u8 height);

bool contained(V2s16 subject_pos, Box_s16 object_box);
bool overlap(Box_s16 subject_box, Box_s16 object_box);

bool isAbove(Box_s16 subject_box, Box_s16 object_box);
bool shareBase(Box_s16 subject_box, Box_s16 object_box);

bool hitAbove(Box_s16 subject_box, Box_s16 object_box);
bool hitUnder(Box_s16 subject_box, Box_s16 object_box);
bool hitLeft(Box_s16 subject_box, Box_s16 object_box);
bool hitRight(Box_s16 subject_box, Box_s16 object_box);

s16 adjacentYAbove(Box_s16 subject_box, Box_s16 object_box);
s16 adjacentYUnder(Box_s16 subject_box, Box_s16 object_box);
s16 adjacentXOnTheLeft(Box_s16 subject_box, Box_s16 object_box);
s16 adjacentXOnTheRight(Box_s16 subject_box, Box_s16 object_box);

#endif /* INC_PHYSICS_H_ */
