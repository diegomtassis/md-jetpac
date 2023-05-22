/*
 * planet_md_02.c
 *
 *  Created on: May 12, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../../inc/elements.h"
#include "../../inc/fwk/commons.h"
#include "../../inc/planet.h"
#include "../../inc/planet.h"
#include "../../inc/spaceship.h"
#include "../../inc/enemies.h"
#include "../../res/sprite.h"

static void createPlatforms(Planet planet[static 1]);
static void defineJetman(Planet planet[static 1]);
static void defineSpaceship(Planet planet[static 1]);

Planet* createPlanetMD02() {

	Planet* planet = allocPlanet();

	createPlatforms(planet);
	defineEnemiesPopulation(planet, alienDefinition, 5);
	defineSpaceship(planet);
	defineJetman(planet);

	planet->def->mind_bottom = TRUE;
	planet->def->ammo = 30;

	return planet;
}

static void createPlatforms(Planet planet[static 1]) {

	planet->floor = createPlatform(14, 18, 5);

	planet->num_platforms = 0;
	planet->platforms = MEM_calloc(planet->num_platforms * sizeof(Platform*));
}

static void defineJetman(Planet planet[static 1]) {

	planet->def->p1_init_pos = MEM_calloc(sizeof *planet->def->p1_init_pos);
	setV2s16(planet->def->p1_init_pos, 88, 72);
}

static void defineSpaceship(Planet planet[static 1]) {

	planet->def->spaceship_def.type_definition = u1Definition;
	planet->def->spaceship_def.init_step = ASSEMBLED;
	setV2s16(&planet->def->spaceship_def.base_pos, 124, 96);
}
