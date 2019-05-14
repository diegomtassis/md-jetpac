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

void updateBox(Object_f16*);

Box_s16 targetBox(Object_f16, u8, u8);
Box_s16 targetHBox(Object_f16, u8, u8);
Box_s16 targetVBox(Object_f16, u8, u8);

u8 overlap(Box_s16, Box_s16);

u8 hitAbove(Box_s16, Box_s16);
u8 hitUnder(Box_s16, Box_s16);
u8 hitLeft(Box_s16, Box_s16);
u8 hitRight(Box_s16, Box_s16);

s16 adjacentYAbove(Box_s16, Box_s16);
s16 adjacentYUnder(Box_s16, Box_s16);
s16 adjacentXOnTheLeft(Box_s16, Box_s16);
s16 adjacentXOnTheRight(Box_s16, Box_s16);

#endif /* INC_PHYSICS_H_ */
