/*
 * planet_zx_08.c
 *
 *  Created on: Dec 02, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../../inc/elements.h"
#include "../../inc/enemies.h"
#include "../../inc/fwk/commons.h"
#include "../../inc/planet.h"
#include "../../inc/spaceship.h"
#include "../../inc/planet.h"
#include "../../res/sprite.h"

Planet* createPlanetZX08() {

	Planet* planet = allocPlanet();

	createDefaultPlatforms(planet);
	defineEnemiesPopulation(planet, owlDefinition, 5);
	defineSpaceshipInDefaultPlanet(planet, u2Definition, ASSEMBLED);

	planet->def->p1_init_pos = 0;
	planet->def->ammo = 50;

	planet->def->mind_bottom = FALSE;

	return planet;
}
