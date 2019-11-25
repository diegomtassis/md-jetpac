/*
 * blinker.c
 *
 *  Created on: Nov 25, 2019
 *      Author: diegomtassis
 */

#include "../../inc/fwk/blinker.h"

#include <genesis.h>

void blink(Blinker* blinker, Sprite* sprite) {

	blinker->counter--;
	if (!blinker->counter) {
		blinker->counter = blinker->init_value;
		blinker->visible = !blinker->visible;
		SPR_setVisibility(sprite, blinker->visible ? VISIBLE : HIDDEN);
	}
}

