/*
 * hud.h
 *
 *  Created on: May 11, 2019
 *      Author: diegomtassis
 */

#ifndef INC_HUD_H_
#define INC_HUD_H_

#include "game.h"

void HUD_init();
void HUD_setupAmmoCounter(bool showAmmo);
void HUD_updatePlayerHud(Player* player, u16 ammo);
void HUD_updateAmmo(u8 player_id, u16 ammo);
void HUD_registerScore(u16 highest_score);

#endif /* INC_HUD_H_ */
