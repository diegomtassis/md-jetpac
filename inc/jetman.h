/*
 * jetman.h
 *
 *  Created on: Apr 16, 2019
 *      Author: diegomtassis
 */

#ifndef INC_JETMAN_H_
#define INC_JETMAN_H_

#include "elements.h"

void JM_start(Planet planet[static 1]);
void JM_release(Planet planet[static 1]);
void JM_kill(Jetman* jetman, Planet planet[static 1], bool exploding);
void JM_reset(Jetman* jetman, Planet planet[static 1]);
bool JM_resurrectOrRelease(Jetman* jetman, Planet planet[static 1]);
bool JM_isAlive(Jetman* jetman);

void JM_acts(Jetman* jetman, Planet planet[static 1]);
void JM_updateStatus(Jetman* jetman, bool* alive, Planet planet[static 1]);

extern Jetman* j1;
extern Jetman* j2;

#endif /* INC_JETMAN_H_ */
