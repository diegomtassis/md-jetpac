/*
 * planet_zx_05.c
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

Planet* createPlanetZX05() {

	Planet* planet = LOC_allocPlanet();

	LOC_createDefaultPlatforms(planet);
	LOC_defineEnemiesPopulation(planet, saucerDefinition, 5);
	LOC_defineSpaceshipInDefaultPlanet(planet, u2Definition, UNASSEMBLED);

	LOC_useEarthGravity(planet);
	planet->def->ammo = 50;

	LOC_setPlayersDefaultInitPos(planet);
	planet->def->mind_bottom = FALSE;

	return planet;
}
