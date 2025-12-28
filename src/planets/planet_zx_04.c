/*
 * planet_zx_04.c
 *
 *  Created on: Nov 25, 2019
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

Planet* createPlanetZX04() {

	Planet* planet = LOC_allocPlanet();

	LOC_createDefaultPlatforms(planet);
	LOC_defineEnemiesPopulation(planet, fighterDefinition, 11);
	LOC_defineSpaceshipInDefaultPlanet(planet, u1Definition, ASSEMBLED);

	LOC_setDefaultPhysicalConstants(planet);

	planet->def->p1_init_pos = 0;
	planet->def->ammo = 50;

	planet->def->mind_bottom = FALSE;

	return planet;
}
