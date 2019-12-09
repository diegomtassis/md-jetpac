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
#include "../../res/sprite.h"

static void createPlatforms(Planet planet[static 1]);
static void defineJetman(Planet planet[static 1]);
static void defineSpaceship(Planet planet[static 1]);

Planet* createPlanetMD04() {

	Planet* planet = allocPlanet();

	createPlatforms(planet);
	defineEnemiesPopulation(planet, fighterDefinition, 7);
	defineSpaceship(planet);
	defineJetman(planet);

	planet->def.mind_bottom = TRUE;
	planet->def.ammo = 5;

	return planet;
}

static void createPlatforms(Planet planet[static 1]) {

	planet->floor = createPlatform(0, 25, 32);

	planet->num_platforms = 18;
	planet->platforms = MEM_calloc(planet->num_platforms * sizeof(Platform*));

	planet->platforms[0] = createPlatform(2, 22, 4);
	planet->platforms[1] = createPlatform(2, 19, 4);
	planet->platforms[2] = createPlatform(2, 16, 4);
	planet->platforms[3] = createPlatform(2, 13, 4);

	planet->platforms[4] = createPlatform(10, 22, 4);
	planet->platforms[5] = createPlatform(10, 19, 4);
	planet->platforms[6] = createPlatform(10, 16, 4);
	planet->platforms[7] = createPlatform(10, 13, 4);
	planet->platforms[16] = createPlatform(10, 10, 4);

	planet->platforms[8] = createPlatform(18, 22, 4);
	planet->platforms[9] = createPlatform(18, 19, 4);
	planet->platforms[10] = createPlatform(18, 16, 4);
	planet->platforms[11] = createPlatform(18, 13, 4);
	planet->platforms[17] = createPlatform(18, 10, 4);

	planet->platforms[12] = createPlatform(26, 22, 4);
	planet->platforms[13] = createPlatform(26, 19, 4);
	planet->platforms[14] = createPlatform(26, 16, 4);
	planet->platforms[15] = createPlatform(26, 13, 4);
}

static void defineSpaceship(Planet planet[static 1]) {

	planet->def.spaceship_def.type_definition = u1Definition;
	planet->def.spaceship_def.init_step = UNASSEMBLED;
	setV2s16(&planet->def.spaceship_def.base_pos, 120, 184);
	setV2s16(&planet->def.spaceship_def.middle_pos, 56, 184);
	setV2s16(&planet->def.spaceship_def.top_pos, 184, 184);
}

static void defineJetman(Planet planet[static 1]) {

	planet->def.p1_init_pos = MEM_alloc(sizeof *planet->def.p1_init_pos);
	setV2s16(planet->def.p1_init_pos, 92, 30);
}
