/*
 * level_md_01.c
 *
 *  Created on: May 12, 2019
 *      Author: diegomtassis
 */

#include "../inc/level_md_01.h"

#include <genesis.h>

#include "../inc/commons.h"
#include "../inc/level.h"
#include "../inc/spaceship.h"

static void createPlatforms(Level* level);
static void defineEnemies(Enemies* enemies);
static void defineSpaceship(Level* level);

Level* createLevelMD01() {

	Level* level = allocLevel();

	createPlatforms(level);
	defineEnemies(&level->enemies);
	defineSpaceship(level);

	level->def.jetman_init_pos = MEM_alloc(sizeof(Vect2D_u16));
	level->def.jetman_init_pos->x = 88;
	level->def.jetman_init_pos->y = 72;

	level->def.check_bottom = TRUE;

	return level;
}

static void createPlatforms(Level* level) {

	level->floor = createPlatform(5, 16, 9);

	level->num_platforms = 3;
	level->platforms = MEM_alloc(level->num_platforms * sizeof(Platform*));

	level->platforms[0] = createPlatform(22, 10, 5);
	level->platforms[1] = createPlatform(22, 16, 5);
	level->platforms[2] = createPlatform(22, 22, 5);
}

static void defineEnemies(Enemies* enemies) {

	enemies->current_num_enemies = 0;
	enemies->max_num_enemies = 12;
}

static void defineSpaceship(Level* level) {

	level->def.spaceshipDef.type = U1;
	level->def.spaceshipDef.init_step = UNASSEMBLED;
	setVect2D_u16(&level->def.spaceshipDef.basePos, 72, 112);
	setVect2D_u16(&level->def.spaceshipDef.middlePos, 188, 64);
	setVect2D_u16(&level->def.spaceshipDef.topPos, 188, 160);
}
