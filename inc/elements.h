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

#define ALIVE		0x01
#define DEAD		0x02

#define U1		0x01
#define U2		0x02
#define U3		0x04
#define U4		0x08

typedef struct {
	Vect2D_u16 pos_t;
	Vect2D_u16 size_t;
	Object_f16 object;
} Platform;

typedef struct {
	Object_f16 object;
	Vect2D_s16 order;
	u8 walk_step_counter;
	u8 health;
	Sprite* sprite;
} Jetman;

typedef struct {
	Object_f16 object;
	u8 health;
	Sprite* sprite;
} Enemy;

typedef struct {
	u8 current_num_enemies;
	u8 max_num_enemies;
	Enemy** objects;
} Enemies;

typedef struct {
	Vect2D_s16 where;
	u8 step;
	Sprite* sprite;
} Explosion;

typedef struct {
	u8 current_num_booms;
	u8 max_num_booms;
	Explosion** objects;
} Explosions;

typedef struct {
	u8 type;
	u16 init_step;
} SpaceshipDefinition;

typedef struct {
	u16 step;
} Spaceship;

typedef struct {
	u8 check_bottom;
	Vect2D_u16* jetman_init_pos;
	SpaceshipDefinition* spaceshipDef;
} LevelDefinition;

typedef struct {
	Platform* floor;
	Platform** platforms;
	u8 num_platforms;
	Enemies enemies;
	Jetman* jetman;
	Explosions booms;
	LevelDefinition def;
} Level;

#endif /* INC_ELEMENTS_H_ */
