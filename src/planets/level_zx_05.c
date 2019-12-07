/*
 * level_zx_05.c
 *
 *  Created on: Nov 26, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../../inc/elements.h"
#include "../../inc/enemies.h"
#include "../../inc/fwk/commons.h"
#include "../../inc/level.h"
#include "../../inc/spaceship.h"
#include "../../inc/planet.h"
#include "../../res/sprite.h"

static void defineEnemies(Level level[static 1]);
static void defineSpaceship(Level level[static 1]);

Level* createLevelZX05() {

	Level* level = allocLevel();

	createDefaultPlatforms(level);
	defineEnemies(level);
	defineSpaceship(level);

	level->def.jetman_init_pos = 0;
	level->def.ammo = 50;

	level->def.mind_bottom = FALSE;

	return level;
}

static void defineEnemies(Level level[static 1]) {

	level->def.enemy_def = saucerDefinition;

	level->enemies.count = 0;
	level->enemies.size = 5;
}

static void defineSpaceship(Level level[static 1]) {

	level->def.spaceship_def.type_definition = u2Definition;
	level->def.spaceship_def.init_step = UNASSEMBLED;
	setV2s16(&level->def.spaceship_def.base_pos, 168, 184);
	setV2s16(&level->def.spaceship_def.middle_pos, 128, 96);
	setV2s16(&level->def.spaceship_def.top_pos, 48, 72);
}
