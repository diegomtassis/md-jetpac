/*
 * events.c
 *
 *  Created on: Jun 8, 2019
 *      Author: diegomtassis
 */

#include "../inc/events.h"

#include "../inc/game.h"

void onEvent(GameEvent game_event, u8 player_id) {

	GAME_scoreByEvent(game_event, player_id);
}
