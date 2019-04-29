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
	Box_f16* box;
} Object_f16;

void updateBox(Object_f16*);

Box_f16 targetBox(const Object_f16*, u8, u8);
Box_f16 targetHBox(const Object_f16*, u8, u8);
Box_f16 targetVBox(const Object_f16*, u8, u8);

u8 overlap(Box_f16, Box_f16);

fix16 hitAbove(Box_f16, Box_f16);
fix16 hitUnder(Box_f16, Box_f16);
fix16 hitLeft(Box_f16, Box_f16);
fix16 hitRight(Box_f16, Box_f16);

#endif /* INC_PHYSICS_H_ */
