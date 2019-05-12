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
void releaseJetman(Jetman* jetman);
void killJetman(Level* level, u8 exploding);
void resetJetman(Level* level);

void jetmanActs(Level* level);

#endif /* INC_JETMAN_H_ */
