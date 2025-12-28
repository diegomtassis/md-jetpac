/*
 * planet_zx_07.c
 *
 *  Created on: Nov 26, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../../inc/elements.h"
#include "../../inc/enemies.h"
#include "../../inc/fwk/commons.h"
#include "../../inc/planet.h"
#include "../../inc/spaceship.h"
#include "../../inc/planet.h"
#include "../../res/zx.h"

Planet* createPlanetZX07() {

	Planet* planet = LOC_allocPlanet();

	LOC_createDefaultPlatforms(planet);
	LOC_defineEnemiesPopulation(planet, crossDefinition, 5);
	LOC_defineSpaceshipInDefaultPlanet(planet, u2Definition, ASSEMBLED);

	LOC_setDefaultPhysicalConstants(planet);

	planet->def->p1_init_pos = 0;
	planet->def->ammo = 50;

	planet->def->mind_bottom = FALSE;

	return planet;
}
