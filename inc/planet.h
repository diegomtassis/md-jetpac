/*
 * planet.h
 *
 *  Created on: Dec 7, 2019
 *      Author: diegomtassis
 */

#ifndef INC_PLANET_H_
#define INC_PLANET_H_

#include "elements.h"
#include "fwk/physics.h"

Planet* LOC_allocPlanet();

void LOC_loadPlanetsBaseResources();
void LOC_releasePlanetsBaseResources();

void LOC_startPlanet(Planet planet[static 1]);
void LOC_releasePlanet(Planet*);

void LOC_createDefaultPlatforms(Planet planet[static 1]);
void LOC_defineSpaceshipInDefaultPlanet(Planet planet[static 1], SpaceshipTypeDefinition type_definition, u16 init_step);
void LOC_defineEnemiesPopulation(Planet planet[static 1], const EnemyDefinition enemyDefinition, u16 size);
void LOC_setDefaultPhysicalConstants(Planet planet[static 1]);

Platform* LOC_createPlatform(u16 pos_x_t, u16 pos_y_t, u16 length_t);
void LOC_releasePlatform(Platform*);

f16 LOC_landed(Box_s16, const Planet*);

#endif /* INC_PLANET_H_ */
