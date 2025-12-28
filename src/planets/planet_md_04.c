/*
 * planet_md_04.c
 *
 *  Created on: May 24, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../../inc/elements.h"
#include "../../inc/enemies.h"
#include "../../inc/fwk/commons.h"
#include "../../inc/planet.h"
#include "../../inc/planet.h"
#include "../../inc/spaceship.h"
#include "../../res/zx.h"
#include "../../inc/planetary_systems.h"

static void createPlatforms(Planet planet[static 1]);
static void defineJetman(Planet planet[static 1]);
static void defineSpaceship(Planet planet[static 1]);

static void initMD04(Planet planet[static 1]);
static void releaseMD04(Planet planet[static 1]);

Planet* createPlanetMD04() {

	Planet* planet = LOC_allocPlanet();

	createPlatforms(planet);
	LOC_defineEnemiesPopulation(planet, fighterDefinition, 7);
	defineSpaceship(planet);
	defineJetman(planet);

	LOC_setDefaultPhysicalConstants(planet);

	planet->def->mind_bottom = TRUE;
	planet->def->ammo = 5;

    planet->def->planet_init_func = &initMD04;
    planet->def->planet_release_func = &releaseMD04;

	return planet;
}

static void createPlatforms(Planet planet[static 1]) {

	planet->floor = LOC_createPlatform(0, 25, 32);

	planet->num_platforms = 18;
	planet->platforms = MEM_calloc(planet->num_platforms * sizeof(Platform*));

	planet->platforms[0] = LOC_createPlatform(2, 22, 4);
	planet->platforms[1] = LOC_createPlatform(2, 19, 4);
	planet->platforms[2] = LOC_createPlatform(2, 16, 4);
	planet->platforms[3] = LOC_createPlatform(2, 13, 4);

	planet->platforms[4] = LOC_createPlatform(10, 22, 4);
	planet->platforms[5] = LOC_createPlatform(10, 19, 4);
	planet->platforms[6] = LOC_createPlatform(10, 16, 4);
	planet->platforms[7] = LOC_createPlatform(10, 13, 4);
	planet->platforms[16] = LOC_createPlatform(10, 10, 4);

	planet->platforms[8] = LOC_createPlatform(18, 22, 4);
	planet->platforms[9] = LOC_createPlatform(18, 19, 4);
	planet->platforms[10] = LOC_createPlatform(18, 16, 4);
	planet->platforms[11] = LOC_createPlatform(18, 13, 4);
	planet->platforms[17] = LOC_createPlatform(18, 10, 4);

	planet->platforms[12] = LOC_createPlatform(26, 22, 4);
	planet->platforms[13] = LOC_createPlatform(26, 19, 4);
	planet->platforms[14] = LOC_createPlatform(26, 16, 4);
	planet->platforms[15] = LOC_createPlatform(26, 13, 4);
}

static void defineSpaceship(Planet planet[static 1]) {

	planet->def->spaceship_def.type_definition = u1Definition;
	planet->def->spaceship_def.init_step = UNASSEMBLED;
	setV2s16(&planet->def->spaceship_def.base_pos, 120, 184);
	setV2s16(&planet->def->spaceship_def.middle_pos, 56, 184);
	setV2s16(&planet->def->spaceship_def.top_pos, 184, 184);
}

static void defineJetman(Planet planet[static 1]) {

	planet->def->p1_init_pos = MEM_calloc(sizeof *planet->def->p1_init_pos);
	setV2s16(planet->def->p1_init_pos, 92, 30);
}

static void initMD04(Planet planet[static 1]) {
	initPlanetarySystemLalande();
}

static void releaseMD04(Planet planet[static 1]) {
	releasePlanetarySystemLalande();
}
