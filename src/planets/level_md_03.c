/*
 * level_md_03.c
 *
 *  Created on: May 23, 2019
 *      Author: diegomtassis
 */

#include "../../inc/planets.h"

#include <genesis.h>

#include "../../inc/fwk/commons.h"
#include "../../inc/level.h"
#include "../../inc/spaceship.h"
#include "../../inc/enemies.h"
#include "../../res/sprite.h"

static void createPlatforms(Level level[static 1]);
static void defineEnemies(Level level[static 1]);
static void defineSpaceship(Level level[static 1]);

Level* createLevelMD03() {

	Level* level = allocLevel();

	createPlatforms(level);
	defineEnemies(level);
	defineSpaceship(level);

	level->def.jetman_init_pos = MEM_alloc(sizeof *level->def.jetman_init_pos);
	setV2s16(level->def.jetman_init_pos, 68, 176);

	level->def.mind_bottom = TRUE;
	level->def.ammo = 20;

	return level;
}

static void createPlatforms(Level level[static 1]) {

	level->floor = createPlatform(1, 22, 11);

	level->num_platforms = 4;
	level->platforms = MEM_calloc(level->num_platforms * sizeof(Platform*));

	level->platforms[0] = createPlatform(0, 7, 4);
	level->platforms[1] = createPlatform(8, 7, 2);
	level->platforms[2] = createPlatform(9, 10, 22);
	level->platforms[3] = createPlatform(30, 7, 2);
}

static void defineEnemies(Level level[static 1]) {

	level->def.enemy_def.type = BUBBLE;
	level->def.enemy_def.size_t.x = BUBBLE_WIDTH;
	level->def.enemy_def.size_t.y = BUBBLE_HEIGHT;

	level->def.enemy_def.growFunc = &growBubble;
	level->def.enemy_def.actFunc = &actBubble;
	level->def.enemy_def.dieFunc = &dieBubble;

	level->enemies.count = 0;
	level->enemies.size = 8;
}

static void defineSpaceship(Level level[static 1]) {

	level->def.spaceship_def.type = U1;
	level->def.spaceship_def.init_step = ASSEMBLED;
	setV2s16(&level->def.spaceship_def.base_pos, 40, 96);
}
