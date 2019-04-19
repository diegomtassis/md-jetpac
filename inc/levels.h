/*
 * levels.h
 *
 *  Created on: Apr 7, 2019
 *      Author: diegomtassis
 */

#ifndef INC_LEVELS_H_
#define INC_LEVELS_H_

#include <genesis.h>

#include "physics.h"

typedef struct {

	Vect2D_u16 pos_t;
	Vect2D_u16 size_t;
	ObjectF16 object;

} Platform;

typedef struct {
	Platform* floor;
	Platform** platforms;
	u8 num_platforms;
} Level;

void runGame();

#endif /* INC_LEVELS_H_ */
