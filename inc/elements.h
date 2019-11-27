/*
 * elements.h
 *
 *  Created on: Apr 24, 2019
 *      Author: diegomtassis
 */

#ifndef INC_ELEMENTS_H_
#define INC_ELEMENTS_H_

#include <genesis.h>

#include "fwk/blinker.h"
#include "fwk/physics.h"
#include "fwk/types.h"

#define ALIVE		0x01
#define DEAD		0x02

#define U1		0x01
#define U2		0x02
#define U3		0x04
#define U4		0x08

typedef struct Game Game;
typedef struct LevelDefinition LevelDefinition;
typedef struct Level Level;
typedef struct SpaceshipDefinition SpaceshipDefinition;
typedef struct Spaceship Spaceship;
typedef struct Platform Platform;
typedef struct Jetman Jetman;
typedef struct EnemyDefinition EnemyDefinition;
typedef struct Enemy Enemy;
typedef struct Collectable Collectable;
typedef struct Grape Grape;
typedef struct Shot Shot;
typedef struct Explosion Explosion;

struct Game {
	u8 mode;
	u8 lives;
	u8 level;
	u16 score;
	Level* (**createLevel)(void);
	u8 num_levels;
};

struct SpaceshipDefinition {
	u8 type;
	u16 init_step;
	V2s16 base_pos;
	V2s16 middle_pos;
	V2s16 top_pos;
};

struct Spaceship {
	u8 type;
	u16 step;
	u16 substep;
	Object_f16* base_object;
	Object_f16* mid_object;
	Object_f16* top_object;
	Sprite* base_sprite;
	Sprite* mid_sprite;
	Sprite* top_sprite;
	Object_f16* fuel_object;
	Sprite* fuel_sprite;
	Sprite* fire_sprite;
};

struct Platform {
	V2u16 pos_t;
	V2u16 size_t;
	Object_f16 object;
};

struct Jetman {
	Object_f16 object;
	V2s16 order;
	u8 walk_step_counter;
	u8 health;
	bool head_back;
	bool limited_ammo;
	u16 ammo;
	Sprite* sprite;
};

struct Enemy {
	const EnemyDefinition* definition;
	Object_f16 object;
	Sprite* sprite;
	void* extension;
	u8 health;
};

typedef Enemy* (*EnemyCreateFunc)();
typedef void (*EnemyActFunc)(Enemy enemy[static 1], Level* level);
typedef void (*EnemyReleaseFunc)(Enemy enemy[static 1]);

struct EnemyDefinition {
	u8 type;
	EnemyCreateFunc createFunc;
	EnemyActFunc actFunc;
	EnemyReleaseFunc releaseFunc;
	V2u16 size_t;
};

struct Collectable {
	Object_f16 object;
	Sprite* sprite;
	Blinker* blinker;
	u8 type;
	u8 step;
};

struct Shot {
	V2s16 where;
	bool to_left;
	u8 type;
	u8 range;
	u8 grapes_count;
	u8 grapes_created;
	u8 grapes_size;
	u16 distance_to_last;
	Grape** grapes;
};

struct Grape {
	Object_f16* object;
	u8 life_left;
	Sprite* sprite;
};

struct Explosion {
	V2s16 where;
	u8 step;
	Sprite* sprite;
};

struct LevelDefinition {
	u8 mind_bottom;
	V2s16* jetman_init_pos;
	u16 ammo;
	SpaceshipDefinition spaceship_def;
	EnemyDefinition enemy_def;
};

struct Level {
	Platform* floor;
	Platform** platforms;
	u8 num_platforms;
	List enemies;
	List collectables;
	Jetman* jetman;
	Spaceship* spaceship;
	List booms;
	List shots;
	LevelDefinition def;
	Game* game;
};

#endif /* INC_ELEMENTS_H_ */
