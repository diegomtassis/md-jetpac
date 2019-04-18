/*
 * collisions.h
 *
 *  Created on: Apr 18, 2019
 *      Author: diegomtassis
 */

#ifndef INC_COLLISIONS_H_
#define INC_COLLISIONS_H_

#include <types.h>

static u8 reachedBotton(Box, Box);
static u8 reachedTop(Box, Box);
static u8 blockedByLeft(Box, Box);
static u8 blockedByRight(Box, Box);

#endif /* INC_COLLISIONS_H_ */
