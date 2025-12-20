/*
 * planet_sandbox.c
 *
 *  Created on: Dec 20, 2025
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

#include "../../inc/config/sandbox_config.h"

Planet* createPlanetSandbox() {

	Planet* planet = allocPlanet();

	createDefaultPlatforms(planet);

	EnemyDefinition enemyDefinition;
	switch (sandbox_config.enemy)
	{
	case SANDBOX_ENEMY_ALIEN:
		enemyDefinition = alienDefinition;
		break;
	case SANDBOX_ENEMY_BUBBLE:
		enemyDefinition = bubbleDefinition;
		break;	
	case SANDBOX_ENEMY_CROSS:
		enemyDefinition = crossDefinition;
		break;
	case SANDBOX_ENEMY_FALCON:
		enemyDefinition = falconDefinition;
		break;
	case SANDBOX_ENEMY_FIGHTER:
		enemyDefinition = fighterDefinition;
		break;	
	case SANDBOX_ENEMY_METEORITE:
		enemyDefinition = meteoriteDefinition;
		break;
	case SANDBOX_ENEMY_OWL:
		enemyDefinition = owlDefinition;
		break;
	case SANDBOX_ENEMY_SAUCER:
		enemyDefinition = saucerDefinition;
		break;
	default:
		break;
	}

	defineEnemiesPopulation(planet, enemyDefinition, 6);
	defineSpaceshipInDefaultPlanet(planet, u1Definition, UNASSEMBLED);

	planet->def->p1_init_pos = 0;
	planet->def->ammo = 59;

	planet->def->mind_bottom = FALSE;

	return planet;
}
