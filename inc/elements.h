/*
 * elements.h
 *
 *  Created on: Apr 24, 2019
 *      Author: diegomtassis
 */

#ifndef INC_ELEMENTS_H_
#define INC_ELEMENTS_H_

#include <genesis.h>

#include "fwk/array_fixed_list.h"
#include "fwk/blinker.h"
#include "fwk/physics.h"

#define ALIVE 0x01
#define DEAD 0x02

typedef struct Config Config;
typedef struct Game Game;
typedef struct GameResult GameResult;
typedef struct Player Player;
typedef struct PlanetDefinition PlanetDefinition;
typedef struct Planet Planet;
typedef struct SpaceshipTypeDefinition SpaceshipTypeDefinition;
typedef struct SpaceshipDefinition SpaceshipDefinition;
typedef struct Spaceship Spaceship;
typedef struct Platform Platform;
typedef struct Jetman Jetman;
typedef struct EnemyDefinition EnemyDefinition;
typedef struct EnemiesDefinition EnemiesDefinition;
typedef struct Enemy Enemy;
typedef struct Collectable Collectable;
typedef struct Grape Grape;
typedef struct Shot Shot;
typedef struct Explosion Explosion;

struct Player {
    u8 id;
    u8 lives;
    u16 score;
};

struct GameResult {
    u16 p1_score;
    u16 p2_score;
};

struct SpaceshipTypeDefinition {
    const SpriteDefinition* sprite_def;
    const SpriteDefinition* base_sprite_def;
    const SpriteDefinition* middle_sprite_def;
    const SpriteDefinition* top_sprite_def;
};

struct SpaceshipDefinition {
    SpaceshipTypeDefinition type_definition;
    u16 init_step;
    V2s16 base_pos;
    V2s16 middle_pos;
    V2s16 top_pos;
};

struct Spaceship {
    SpaceshipDefinition definition;
    u16 step;
    u16 substep;
    Jetman* grabbedBy;
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
    u8 id;
    u8 joystick;
    Player* player;
    V2s16 order;
    u8 walk_step_counter;
    u8 health;
    u8 finished;
    bool airborne;
    bool head_back;
    bool limited_ammo;
    u16 ammo;
    u8 shots;
    Shot* last_shot;
    bool immunity;
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
typedef void (*EnemyActFunc)(Enemy enemy[static 1], Planet* planet);
typedef void (*EnemyReleaseFunc)(Enemy enemy[static 1]);

struct EnemyDefinition {
    u8 type;
    EnemyCreateFunc createFunc;
    EnemyActFunc actFunc;
    EnemyReleaseFunc releaseFunc;
    V2u16 size_t;
};

struct EnemiesDefinition {
    EnemyDefinition enemy_def;
    u8 num_enemies;
};

struct Collectable {
    Object_f16 object;
    Sprite* sprite;
    Blinker* blinker;
    u8 type;
    u8 step;
};

struct Shot {
    Jetman* shooter;
    V2s16 where;
    bool to_left;
    u8 type;
    u8 range;
    ArrayFixedList grapes;
    u8 grapes_left;
    u16 gap_to_start;  // distance from the shot place to the tail of the last
                       // grape
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

struct Planet {
    PlanetDefinition* def;
    Platform* floor;
    Platform** platforms;
    u8 num_platforms;
    Spaceship* spaceship;
    Jetman* j1;
    Jetman* j2;
    ArrayFixedList enemies;
    ArrayFixedList collectables;
    ArrayFixedList booms;
    ArrayFixedList shots;
};

typedef void (*PlanetInitFunc)(Planet planet[static 1]);
typedef void (*PlanetReleaseFunc)(Planet planet[static 1]);

struct PlanetDefinition {
    SpaceshipDefinition spaceship_def;
    EnemiesDefinition enemies_def;
    u8 mind_bottom;
    V2s16* p1_init_pos;
    V2s16* p2_init_pos;
    u16 ammo;
	PlanetInitFunc planet_init_func;
	PlanetReleaseFunc planet_release_func;
};

#endif /* INC_ELEMENTS_H_ */
