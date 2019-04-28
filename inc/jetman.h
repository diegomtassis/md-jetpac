/*
 * jetman.h
 *
 *  Created on: Apr 16, 2019
 *      Author: diegomtassis
 */

#ifndef INC_JETMAN_H_
#define INC_JETMAN_H_

#include "elements.h"

void startJetman(Level* level);
void killJetman(Jetman* jetman, u8 release);
void resetJetman(Level* level);

void jetmanActs(const Level* level);

#endif /* INC_JETMAN_H_ */
