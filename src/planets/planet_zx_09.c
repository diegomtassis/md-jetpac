/*
 * planet_zx_09.c
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

Planet* createPlanetZX09() {

	Planet* planet = LOC_allocPlanet();

	LOC_createDefaultPlatforms(planet);
	LOC_defineEnemiesPopulation(planet, meteoriteDefinition, 5);
	LOC_defineSpaceshipInDefaultPlanet(planet, u3Definition, UNASSEMBLED);

	LOC_useEarthGravity(planet);
	planet->def->ammo = 50;

	LOC_setPlayersDefaultInitPos(planet);
	planet->def->mind_bottom = FALSE;

	return planet;
}
