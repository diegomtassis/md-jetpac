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
#include "../../res/sprite.h"

Planet* createPlanetZX01() {

	Planet* planet = allocPlanet();

	createDefaultPlatforms(planet);
	defineEnemiesPopulation(planet, meteoriteDefinition, 6);
	defineSpaceshipInDefaultPlanet(planet, u1Definition, UNASSEMBLED);

	planet->def->p1_init_pos = 0;
	planet->def->ammo = 59;

	planet->def->mind_bottom = FALSE;

	return planet;
}
