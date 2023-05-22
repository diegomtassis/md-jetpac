/*
 * planet_md_03.c
 *
 *  Created on: May 23, 2019
 *      Author: diegomtassis
 */

#include "../../inc/planets.h"

#include <genesis.h>

#include "../../inc/fwk/commons.h"
#include "../../inc/planet.h"
#include "../../inc/planet.h"
#include "../../inc/spaceship.h"
#include "../../inc/enemies.h"
#include "../../res/sprite.h"

static void createPlatforms(Planet planet[static 1]);
static void defineSpaceship(Planet planet[static 1]);

Planet* createPlanetMD03() {

	Planet* planet = allocPlanet();

	createPlatforms(planet);
	defineEnemiesPopulation(planet, bubbleDefinition, 6);
	defineSpaceship(planet);

	planet->def->p1_init_pos = MEM_calloc(sizeof *planet->def->p1_init_pos);
	setV2s16(planet->def->p1_init_pos, 68, 176);

	planet->def->mind_bottom = TRUE;
	planet->def->ammo = 20;

	return planet;
}

static void createPlatforms(Planet planet[static 1]) {

	planet->floor = createPlatform(1, 22, 11);

	planet->num_platforms = 4;
	planet->platforms = MEM_calloc(planet->num_platforms * sizeof(Platform*));

	planet->platforms[0] = createPlatform(0, 7, 4);
	planet->platforms[1] = createPlatform(8, 7, 2);
	planet->platforms[2] = createPlatform(9, 10, 22);
	planet->platforms[3] = createPlatform(30, 7, 2);
}

static void defineSpaceship(Planet planet[static 1]) {

	planet->def->spaceship_def.type_definition = u1Definition;
	planet->def->spaceship_def.init_step = ASSEMBLED;
	setV2s16(&planet->def->spaceship_def.base_pos, 40, 96);
}
