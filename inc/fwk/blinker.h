/*
 * blinker.h
 *
 *  Created on: Nov 25, 2019
 *      Author: diegomtassis
 */

#ifndef INC_BLINKER_H_
#define INC_BLINKER_H_

#include <genesis.h>

typedef struct {
	u16 counter;
	u16 init_value;
	bool visible;
} Blinker;

void blink(Blinker* blinker, Sprite* sprite);

#endif /* INC_BLINKER_H_ */
