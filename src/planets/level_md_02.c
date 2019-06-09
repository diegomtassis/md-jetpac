/*
 * level_md_02.c
 *
 *  Created on: May 12, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../../inc/elements.h"
#include "../../inc/fwk/commons.h"
#include "../../inc/level.h"
#include "../../inc/spaceship.h"
#include "../../inc/enemies.h"
#include "../../res/sprite.h"

static void createPlatforms(Level level[static 1]);
static void defineEnemies(Level level[static 1]);
static void defineJetman(Level level[static 1]);
static void defineSpaceship(Level level[static 1]);

Level* createLevelMD02() {

	Level* level = allocLevel();

	createPlatforms(level);
	defineEnemies(level);
	defineSpaceship(level);
	defineJetman(level);

	level->def.mind_bottom = TRUE;
	level->def.ammo = 30;

	return level;
}

static void createPlatforms(Level level[static 1]) {

	level->floor = createPlatform(14, 18, 5);

	level->num_platforms = 0;
	level->platforms = MEM_alloc(level->num_platforms * sizeof(Platform*));
	memset(level->platforms, 0, sizeof(Platform*));
}

static void defineJetman(Level level[static 1]) {

	level->def.jetman_init_pos = MEM_alloc(sizeof *level->def.jetman_init_pos);
	setV2s16(level->def.jetman_init_pos, 88, 72);
}

static void defineEnemies(Level level[static 1]) {

	level->def.enemy_def.type = ENEMY_02;
	level->def.enemy_def.size_t.x = ENEMY_02_WIDTH;
	level->def.enemy_def.size_t.y = ENEMY_02_HEIGHT;
	level->def.enemy_def.sprite_def = &enemy_02_sprite;

	level->enemies.count = 0;
	level->enemies.size = 8;
}

static void defineSpaceship(Level level[static 1]) {

	level->def.spaceship_def.type = U1;
	level->def.spaceship_def.init_step = ASSEMBLED;
	setV2s16(&level->def.spaceship_def.base_pos, 124, 96);
}
