/*
 * jetman.h
 *
 *  Created on: Apr 16, 2019
 *      Author: diegomtassis
 */

#ifndef INC_JETMAN_H_
#define INC_JETMAN_H_

#include "elements.h"

void startJetman(Level level[static 1]);
void releaseJetman(Level level[static 1]);
void killJetman(Level level[static 1], bool exploding);
void resetJetman(Level level[static 1]);

void jetmanActs(Level level[static 1]);

#endif /* INC_JETMAN_H_ */
