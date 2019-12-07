/*
 * planet.h
 *
 *  Created on: Dec 7, 2019
 *      Author: diegomtassis
 */

#ifndef INC_PLANET_H_
#define INC_PLANET_H_

void createDefaultPlatforms(Level level[static 1]);
void defineSpaceshipInDefaultPlanet(Level level[static 1], SpaceshipTypeDefinition type_definition, u16 init_step);

#endif /* INC_PLANET_H_ */
