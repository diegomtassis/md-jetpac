/*
 * level_md_01.c
 *
 *  Created on: May 12, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../../inc/fwk/commons.h"
#include "../../inc/level.h"
#include "../../inc/planets.h"
#include "../../inc/spaceship.h"

static void createPlatforms(Level* level);
static void defineEnemies(Enemies* enemies);
static void defineSpaceship(Level* level);
static void defineJetman(Level* level);

Level* createLevelMD01() {

	Level* level = allocLevel();

	createPlatforms(level);
	defineEnemies(&level->enemies);
	defineSpaceship(level);
	defineJetman(level);

	level->def.mind_bottom = TRUE;

	return level;
}

static void createPlatforms(Level* level) {

	level->floor = createPlatform(3, 16, 9);

	level->num_platforms = 3;
	level->platforms = MEM_alloc(level->num_platforms * sizeof(Platform*));

	level->platforms[0] = createPlatform(22, 10, 5);
	level->platforms[1] = createPlatform(22, 16, 5);
	level->platforms[2] = createPlatform(22, 22, 5);
}

static void defineSpaceship(Level* level) {

	level->def.spaceship_def.type = U1;
	level->def.spaceship_def.init_step = UNASSEMBLED;
	setV2s16(&level->def.spaceship_def.base_pos, 52, 112);
	setV2s16(&level->def.spaceship_def.middle_pos, 188, 64);
	setV2s16(&level->def.spaceship_def.top_pos, 188, 160);
}

static void defineJetman(Level* level) {

	level->def.jetman_init_pos = MEM_alloc(sizeof *level->def.jetman_init_pos);
	setV2s16(level->def.jetman_init_pos, 88, 72);
}

static void defineEnemies(Enemies* enemies) {

	enemies->current_num_enemies = 0;
	enemies->max_num_enemies = 10;
}

