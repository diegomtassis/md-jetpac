/*
 * jetman.h
 *
 *  Created on: Apr 16, 2019
 *      Author: diegomtassis
 */

#ifndef INC_JETMAN_H_
#define INC_JETMAN_H_

#include "elements.h"

void startPlayers(Planet planet[static 1], bool limit_ammo);
void releasePlayers(Planet planet[static 1]);
void killPlayer(Planet planet[static 1], bool exploding);
void resetPlayers(Planet planet[static 1]);

void playersAct(Planet planet[static 1]);

#endif /* INC_JETMAN_H_ */
