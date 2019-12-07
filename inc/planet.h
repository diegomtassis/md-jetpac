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

Planet* allocPlanet();
void startPlanet(Planet planet[static 1]);
void releasePlanet(Planet*);

void createDefaultPlatforms(Planet planet[static 1]);
void defineSpaceshipInDefaultPlanet(Planet planet[static 1], SpaceshipTypeDefinition type_definition, u16 init_step);
void defineEnemiesPopulation(Planet planet[static 1], const EnemyDefinition enemyDefinition, u16 size);

Platform* createPlatform(u16 pos_x_t, u16 pos_y_t, u16 length_t);
void releasePlatform(Platform*);

f16 landed(Box_s16, const Planet*);

#endif /* INC_PLANET_H_ */
