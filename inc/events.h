/*
 * events.h
 *
 *  Created on: Jun 8, 2019
 *      Author: diegomtassis
 */

#ifndef INC_EVENTS_H_
#define INC_EVENTS_H_

typedef enum {
	KILLED_ENEMY_01, //
	KILLED_ENEMY_02, //
	KILLED_ENEMY_03, //
	GRABBED_SPACESHIP_PART, //
	GRABBED_FUEL, //
	GRABBED_BONUS, //
} GameEvent;

void onEvent(GameEvent event);

#endif /* INC_EVENTS_H_ */
