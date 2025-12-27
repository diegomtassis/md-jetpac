/*
 * hud.h
 *
 *  Created on: May 11, 2019
 *      Author: diegomtassis
 */

#ifndef INC_HUD_H_
#define INC_HUD_H_

#include "game.h"

void initHud();
void setupAmmoCounter(bool showAmmo);
void updatePlayerHud(Player* player, u16 ammo);
void updateAmmo(u8 player_id, u16 ammo);
void registerScore(u16 highest_score);

#endif /* INC_HUD_H_ */
