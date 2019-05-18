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
	V2u16 pos_t;
	V2u16 size_t;
	Object_f16 object;
} Platform;

typedef struct {
	Object_f16 object;
	V2s16 order;
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
	V2s16 where;
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
	V2u16 base_pos;
	V2u16 middle_pos;
	V2u16 top_pos;
} SpaceshipDefinition;

typedef struct {
	u8 type;
	u16 step;
	u16 substep;
	Object_f16 base_object;
	Object_f16 mid_object;
	Object_f16 top_object;
	Sprite* base_sprite;
	Sprite* mid_sprite;
	Sprite* top_sprite;
	Object_f16 fuel_object;
	Sprite* fuel_sprite;
} Spaceship;

typedef struct {
	u8 mind_bottom;
	V2u16* jetman_init_pos;
	SpaceshipDefinition spaceship_def;
} LevelDefinition;

typedef struct {
	Platform* floor;
	Platform** platforms;
	u8 num_platforms;
	Enemies enemies;
	Jetman* jetman;
	Spaceship* spaceship;
	Explosions booms;
	LevelDefinition def;
} Level;

#endif /* INC_ELEMENTS_H_ */
