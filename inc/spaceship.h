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

#define UNASSEMBLED	1
#define MID_SET		2
#define ASSEMBLED	3
#define FUEL_1		4
#define FUEL_2		5
#define FUEL_3		6
#define FUEL_4		7
#define FUEL_5		8
#define READY		9

void startSpaceship(Level* level);

void handleSpaceship(Level*);

void releaseSpaceship(Level* level);

#endif /* INC_SPACESHIP_H_ */
