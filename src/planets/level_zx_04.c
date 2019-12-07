/*
 * level_zx_04.c
 *
 *  Created on: Nov 25, 2019
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

Level* createLevelZX04() {

	Level* level = allocLevel();

	createDefaultPlatforms(level);
	defineEnemies(level);
	defineSpaceshipInDefaultPlanet(level, u1Definition, ASSEMBLED);

	level->def.jetman_init_pos = 0;
	level->def.ammo = 50;

	level->def.mind_bottom = FALSE;

	return level;
}

static void defineEnemies(Level level[static 1]) {

	level->def.enemy_def = fighterDefinition;

	level->enemies.count = 0;
	level->enemies.size = 7;
}
