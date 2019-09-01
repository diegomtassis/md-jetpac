/*
 * collectables.h
 *
 *  Created on: Aug 18, 2019
 *      Author: diegomtassis
 */

#ifndef INC_COLLECTABLES_H_
#define INC_COLLECTABLES_H_

#include "elements.h"

void startCollectables(Level level[static 1]);
void releaseCollectables(Level*);

void updateCollectables(Level level[static 1]);

#endif /* INC_COLLECTABLES_H_ */
