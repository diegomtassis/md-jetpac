/*
 * spaceship.h
 *
 *  Created on: May 12, 2019
 *      Author: diegomtassis
 */

#ifndef INC_SPACESHIP_H_
#define INC_SPACESHIP_H_

#include "elements.h"

#include <genesis.h>

#define LANDING 	1
#define UNASSEMBLED 2
#define FUSELAGE_SET		3
#define ASSEMBLED	4
#define FUEL_1		5
#define FUEL_2		6
#define FUEL_3		7
#define FUEL_4		8
#define FUEL_5		9
#define READY		10
#define LIFTING		11
#define ORBITING	12

void startSpaceship(Planet planet[static 1]);
void releaseSpaceship(Planet planet[static 1]);

void handleSpaceship(Planet planet[static 1]);
void dropIfGrabbed(u8 player_id, Spaceship*);
void launch(Spaceship*);

extern const SpaceshipTypeDefinition u1Definition;
extern const SpaceshipTypeDefinition u2Definition;
extern const SpaceshipTypeDefinition u3Definition;
extern const SpaceshipTypeDefinition u4Definition;

#endif /* INC_SPACESHIP_H_ */
