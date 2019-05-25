/*
 * level_00.c
 *
 *  Created on: May 2, 2019
 *      Author: diegomtassis
 */

#include "../../inc/planets.h"

#include <genesis.h>

#include "../../inc/level.h"

static void createPlatforms(Level level[static 1]);
static void defineEnemies(Enemies* enemies);

Level* createLevel00() {

	Level* level = allocLevel();

	createPlatforms(level);
	defineEnemies(&level->enemies);

	return level;
}

static void createPlatforms(Level level[static 1]) {

	level->floor = createPlatform(0, 25, 32);

	level->num_platforms = 0;
}

static void defineEnemies(Enemies* enemies) {

	enemies->current_num_enemies = 0;
	enemies->max_num_enemies = 1;
}
