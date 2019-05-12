/*
 * level_md_01.c
 *
 *  Created on: May 12, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../inc/level_md_01.h"
#include "../inc/levels.h"

static void createPlatforms(Level* level);
static void defineEnemies(Enemies* enemies);

Level* createLevelMD01() {

	Level* level = allocLevel();

	createPlatforms(level);
	defineEnemies(&level->enemies);

	level->initPos = MEM_alloc(sizeof(Vect2D_u16));
	level->initPos->x = 88;
	level->initPos->y = 72;

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
