/*
 * level_md_01.c
 *
 *  Created on: May 12, 2019
 *      Author: diegomtassis
 */

#include "../../inc/planets/level_md_01.h"

#include <genesis.h>

#include "../../inc/fwk/commons.h"
#include "../../inc/level.h"
#include "../../inc/spaceship.h"

static void createPlatforms(Level* level);
static void defineEnemies(Enemies* enemies);
static void defineSpaceship(Level* level);

Level* createLevelMD02() {

	Level* level = allocLevel();

	createPlatforms(level);
	defineEnemies(&level->enemies);
	defineSpaceship(level);

	level->def.jetman_init_pos = MEM_alloc(sizeof *level->def.jetman_init_pos);
	setV2s16(level->def.jetman_init_pos, 88, 72);

	level->def.mind_bottom = TRUE;

	return level;
}

static void createPlatforms(Level* level) {

	level->floor = createPlatform(14, 18, 5);

	level->num_platforms = 0;
	level->platforms = MEM_alloc(level->num_platforms * sizeof(Platform*));
}

static void defineEnemies(Enemies* enemies) {

	enemies->current_num_enemies = 0;
	enemies->max_num_enemies = 12;
}

static void defineSpaceship(Level* level) {

	level->def.spaceship_def.type = U1;
	level->def.spaceship_def.init_step = ASSEMBLED;
	setV2s16(&level->def.spaceship_def.base_pos, 124, 96);
}
