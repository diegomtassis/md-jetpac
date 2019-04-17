/*
 * levels.h
 *
 *  Created on: Apr 7, 2019
 *      Author: diegomtassis
 */

#ifndef INC_LEVELS_H_
#define INC_LEVELS_H_

typedef struct {
	u8 xPos;
	u8 yPos;
	u8 length;
} Platform;

typedef struct {
	const Platform* floor;
	const Platform* platforms[3];
} Level;

void runGame();

#endif /* INC_LEVELS_H_ */
