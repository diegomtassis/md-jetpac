/*
 * elements.h
 *
 *  Created on: Apr 24, 2019
 *      Author: diegomtassis
 */

#ifndef INC_ELEMENTS_H_
#define INC_ELEMENTS_H_

#include "physics.h"

#include <genesis.h>

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

#endif /* INC_ELEMENTS_H_ */
