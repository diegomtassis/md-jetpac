/*
 * level_zx_01.c
 *
 *  Created on: May 2, 2019
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

Level* createLevelZX02() {

	Level* level = allocLevel();

	createPlatforms(level);
	defineEnemies(level);
	defineSpaceship(level);

	level->def.jetman_init_pos = 0;
	level->def.ammo = 50;

	level->def.mind_bottom = FALSE;

	return level;
}

static void createPlatforms(Level level[static 1]) {

	level->floor = createPlatform(0, 25, 32);

	level->num_platforms = 3;
	level->platforms = MEM_calloc(level->num_platforms * sizeof(Platform*));

	level->platforms[0] = createPlatform(4, 11, 6);
	level->platforms[1] = createPlatform(15, 14, 4);
	level->platforms[2] = createPlatform(24, 8, 6);
}

static void defineEnemies(Level level[static 1]) {

	level->def.enemy_def.type = ALIEN;
	level->def.enemy_def.size_t.x = ALIEN_WIDTH;
	level->def.enemy_def.size_t.y = ALIEN_HEIGHT;

	level->def.enemy_def.growEnemyFunc = &growAlien;
	level->def.enemy_def.actEnemyFunc = &actAlien;
	level->def.enemy_def.clearEnemyFunc = &clearAlien;

	level->enemies.count = 0;
	level->enemies.size = 14;
}

static void defineSpaceship(Level level[static 1]) {

	level->def.spaceship_def.type = U1;
	level->def.spaceship_def.init_step = ASSEMBLED;
	setV2s16(&level->def.spaceship_def.base_pos, 168, 152);
}
