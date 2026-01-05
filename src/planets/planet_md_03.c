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
#include "../../res/zx.h"
#include "../../inc/planetary_systems.h"


static void createPlatforms(Planet planet[static 1]);
static void defineSpaceship(Planet planet[static 1]);

static void initMD03(Planet planet[static 1]);
static void releaseMD03(Planet planet[static 1]);

Planet* createPlanetMD03() {

	Planet* planet = LOC_allocPlanet();

	createPlatforms(planet);
	LOC_defineEnemiesPopulation(planet, bubbleDefinition, 6);
	defineSpaceship(planet);

	LOC_setPlayersDefaultInitPos(planet);

	LOC_useEarthGravity(planet);

	planet->def->mind_bottom = TRUE;
	planet->def->ammo = 20;

    planet->def->planet_init_func = &initMD03;
    planet->def->planet_release_func = &releaseMD03;

	return planet;
}

static void createPlatforms(Planet planet[static 1]) {

	planet->floor = LOC_createPlatform(1, 22, 11);

	planet->num_platforms = 4;
	planet->platforms = MEM_calloc(planet->num_platforms * sizeof(Platform*));

	planet->platforms[0] = LOC_createPlatform(0, 7, 4);
	planet->platforms[1] = LOC_createPlatform(8, 7, 2);
	planet->platforms[2] = LOC_createPlatform(9, 10, 22);
	planet->platforms[3] = LOC_createPlatform(30, 7, 2);
}

static void defineSpaceship(Planet planet[static 1]) {

	planet->def->spaceship_def.type_definition = u1Definition;
	planet->def->spaceship_def.init_step = ASSEMBLED;
	setV2s16(&planet->def->spaceship_def.base_pos, 40, 96);
}

static void initMD03(Planet planet[static 1]) {
	initPlanetarySystemLalande();
}

static void releaseMD03(Planet planet[static 1]) {
	releasePlanetarySystemLalande();
}
