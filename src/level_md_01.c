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

	return level;
}

static void createPlatforms(Level* level) {

	level->floor = createPlatform(10, 20, 12);

	level->num_platforms = 2;
	level->platforms = MEM_alloc(level->num_platforms * sizeof(Platform*));

	level->platforms[0] = createPlatform(5, 12, 5);
	level->platforms[1] = createPlatform(22, 12, 5);
}

static void defineEnemies(Enemies* enemies) {

	enemies->current_num_enemies = 0;
	enemies->max_num_enemies = 6;
}
