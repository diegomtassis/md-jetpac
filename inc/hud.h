/*
 * hud.h
 *
 *  Created on: May 11, 2019
 *      Author: diegomtassis
 */

#ifndef INC_HUD_H_
#define INC_HUD_H_

#include "game.h"

void initializeHud();
void updateHud(Game*);
void registerScore(u16 highest_score);

#endif /* INC_HUD_H_ */
