/*
 * planet_zx_16.c
 *
 *  Created on: Dec 06, 2019
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

Planet* createPlanetZX16() {

	Planet* planet = LOC_allocPlanet();

	LOC_createDefaultPlatforms(planet);
	LOC_defineEnemiesPopulation(planet, owlDefinition, 5);
	LOC_defineSpaceshipInDefaultPlanet(planet, u4Definition, ASSEMBLED);

	LOC_setDefaultPhysicalConstants(planet);

	planet->def->p1_init_pos = 0;
	planet->def->ammo = 50;

	planet->def->mind_bottom = FALSE;

	return planet;
}
