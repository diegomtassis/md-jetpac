/*
 * jetman.h
 *
 *  Created on: Apr 16, 2019
 *      Author: diegomtassis
 */

#ifndef INC_JETMAN_H_
#define INC_JETMAN_H_

#include "elements.h"

void startJetman(Planet planet[static 1], bool limit_ammo);
void releaseJetman(Planet planet[static 1]);
void killJetman(Planet planet[static 1], bool exploding);
void resetJetman(Planet planet[static 1]);

void jetmanActs(Planet planet[static 1]);

#endif /* INC_JETMAN_H_ */
