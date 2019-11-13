/*
 * level_md_01.c
 *
 *  Created on: May 12, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../../inc/elements.h"
#include "../../inc/enemies.h"
#include "../../inc/fwk/commons.h"
#include "../../inc/level.h"
#include "../../inc/spaceship.h"
#include "../../res/sprite.h"

static void createPlatforms(Level level[static 1]);
static void defineEnemies(Level level[static 1]);
static void defineSpaceship(Level level[static 1]);
static void defineJetman(Level level[static 1]);

Level* createLevelMD01() {

	Level* level = allocLevel();

	createPlatforms(level);
	defineEnemies(level);
	defineSpaceship(level);
	defineJetman(level);

	level->def.mind_bottom = TRUE;
	level->def.ammo = 25;

	return level;
}

static void createPlatforms(Level level[static 1]) {

	level->floor = createPlatform(3, 16, 9);

	level->num_platforms = 3;
	level->platforms = MEM_calloc(level->num_platforms * sizeof(Platform*));

	level->platforms[0] = createPlatform(22, 10, 5);
	level->platforms[1] = createPlatform(22, 16, 5);
	level->platforms[2] = createPlatform(22, 22, 5);
}

static void defineSpaceship(Level level[static 1]) {

	level->def.spaceship_def.type = U1;
	level->def.spaceship_def.init_step = UNASSEMBLED;
	setV2s16(&level->def.spaceship_def.base_pos, 52, 112);
	setV2s16(&level->def.spaceship_def.middle_pos, 188, 64);
	setV2s16(&level->def.spaceship_def.top_pos, 188, 160);
}

static void defineJetman(Level level[static 1]) {

	level->def.jetman_init_pos = MEM_alloc(sizeof *level->def.jetman_init_pos);
	setV2s16(level->def.jetman_init_pos, 140, 72);
}

static void defineEnemies(Level level[static 1]) {

	level->def.enemy_def.type = METEORITE;

	level->def.enemy_def.size_t.x = METEORITE_WIDTH;
	level->def.enemy_def.size_t.y = METEORITE_HEIGHT;

	level->def.enemy_def.createFunc = &createMeteorite;
	level->def.enemy_def.actFunc = &actMeteorite;
	level->def.enemy_def.releaseFunc = &releaseMeteorite;

	level->enemies.count = 0;
	level->enemies.size = 7;
}
