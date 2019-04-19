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

	u8 pos_h_t;
	u8 pos_v_t;
	u8 length_t;
	u8 height_t;
	Box* box;
} Platform;

typedef struct {
	Platform* floor;
	Platform* platforms;
	u8 num_platforms;
} Level;

void runGame();

#endif /* INC_LEVELS_H_ */
