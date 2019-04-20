/*
 * game.c
 *
 *  Created on: Apr 20, 2019
 *      Author: diegomtassis
 */

#include "../inc/game.h"

#include "../inc/levels.h"
#include "../inc/jetman.h"

static Level* current_level;

void startGame() {

	current_level = createLevel();
	startLevel(current_level);

	startJetman(current_level);
}
