/*
 * planet_zx_03.c
 *
 *  Created on: Nov 17, 2019
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

Planet* createPlanetZX03() {

	Planet* planet = allocPlanet();

	createDefaultPlatforms(planet);
	defineEnemiesPopulation(planet, bubbleDefinition, 6);
	defineSpaceshipInDefaultPlanet(planet, u1Definition, ASSEMBLED);

	planet->def.jetman_init_pos = 0;
	planet->def.ammo = 50;

	planet->def.mind_bottom = FALSE;

	return planet;
}
