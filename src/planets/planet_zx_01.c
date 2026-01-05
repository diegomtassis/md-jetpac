/*
 * planet_zx_01.c
 *
 *  Created on: May 2, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../../inc/elements.h"
#include "../../inc/enemies.h"
#include "../../inc/fwk/commons.h"
#include "../../inc/planet.h"
#include "../../inc/planet.h"
#include "../../inc/spaceship.h"
#include "../../inc/planet.h"
#include "../../res/zx.h"

Planet* createPlanetZX01() {

	Planet* planet = LOC_allocPlanet();

	LOC_createDefaultPlatforms(planet);
	LOC_defineEnemiesPopulation(planet, meteoriteDefinition, 6);
	LOC_defineSpaceshipInDefaultPlanet(planet, u1Definition, UNASSEMBLED);

	LOC_useEarthGravity(planet);
	planet->def->ammo = 59;

	LOC_setPlayersDefaultInitPos(planet);
	planet->def->mind_bottom = FALSE;

	return planet;
}
