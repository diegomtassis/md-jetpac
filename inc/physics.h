/*
 * physics.h
 *
 *  Created on: Apr 19, 2019
 *      Author: diegomtassis
 */

#ifndef INC_PHYSICS_H_
#define INC_PHYSICS_H_

#include <genesis.h>

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

typedef struct {
	Object_f16 object;
	Vect2D_s16 order;
} Jetman;

typedef struct {
	Vect2D_u16 pos_t;
	Vect2D_u16 size_t;
	Object_f16 object;
} Platform;

typedef struct {
	Platform* floor;
	Platform** platforms;
	u8 num_platforms;
} Level;

void updateBox(Object_f16*);

fix16 reachedBottonF16(Box_f16, Box_f16);
fix16 reachedTopF16(Box_f16, Box_f16);
fix16 blockedByLeftF16(Box_f16, Box_f16);
fix16 blockedByRightF16(Box_f16, Box_f16);

#endif /* INC_PHYSICS_H_ */
