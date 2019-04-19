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
} BoxF16;

typedef struct {
	Vect2D_f16 pos;
	Vect2D_u16 size;
	Vect2D_f16 mov;
	BoxF16* box;
} ObjectF16;


u8 reachedBottonF16(BoxF16, BoxF16);
u8 reachedTopF16(BoxF16, BoxF16);
u8 blockedByLeftF16(BoxF16, BoxF16);
u8 blockedByRightF16(BoxF16, BoxF16);

#endif /* INC_PHYSICS_H_ */
