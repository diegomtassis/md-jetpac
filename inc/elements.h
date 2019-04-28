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
	Vect2D_u16 pos_t;
	Vect2D_u16 size_t;
	Object_f16 object;
} Platform;

typedef struct {
	Object_f16 object;
	Vect2D_s16 order;
	u8 walk_step_counter;
	u8 alive;
	Sprite* sprite;
} Jetman;

typedef struct {
	Object_f16 object;
	u8 alive;
	Sprite* sprite;
} Enemy;

typedef struct {
	u8 current_num_enemies;
	u8 max_num_enemies;
	Enemy** objects;
} Enemies;

typedef struct {
	Platform* floor;
	Platform** platforms;
	u8 num_platforms;
	Jetman* jetman;
	Enemies* enemies;
} Level;

#endif /* INC_ELEMENTS_H_ */
