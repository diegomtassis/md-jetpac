/*
 * physics.h
 *
 *  Created on: Apr 19, 2019
 *      Author: diegomtassis
 */

#ifndef INC_PHYSICS_H_
#define INC_PHYSICS_H_

#include <genesis.h>

#define LEFT_POS_H_PX_F16    	FIX16(0)
#define RIGHT_POS_H_PX_F16  	FIX16(256)
#define TOP_POS_V_PX_F16 		FIX16(32)
#define BOTTOM_POS_V_PX_F16 	FIX16(224 - 32)

typedef struct {
	fix16 x;
	fix16 y;
	u16 w;
	u16 h;
} Box_f16;

typedef struct {
	Vect2D_f16 pos;
	Vect2D_u16 size;
	Vect2D_f16 mov;
	Box_f16 box;
} Object_f16;

void updateBox(Object_f16*);

Box_f16 targetBox(Object_f16, u8, u8);
Box_f16 targetHBox(Object_f16, u8, u8);
Box_f16 targetVBox(Object_f16, u8, u8);

u8 overlap(Box_f16, Box_f16);

u8 hitAbove(Box_f16, Box_f16);
u8 hitUnder(Box_f16, Box_f16);
u8 hitLeft(Box_f16, Box_f16);
u8 hitRight(Box_f16, Box_f16);

fix16 adjacentYAbove(Box_f16, Box_f16);
fix16 adjacentYUnder(Box_f16, Box_f16);
fix16 adjacentXOnTheLeft(Box_f16, Box_f16);
fix16 adjacentXOnTheRight(Box_f16, Box_f16);

#endif /* INC_PHYSICS_H_ */
