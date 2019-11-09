/*
 * events.h
 *
 *  Created on: Jun 8, 2019
 *      Author: diegomtassis
 */

#ifndef INC_EVENTS_H_
#define INC_EVENTS_H_

typedef enum {
	KILLED_ENEMY, //
	GRABBED_SPACESHIP_PART, //
	GRABBED_FUEL, //
	GRABBED_COLLECTABLE, //
	LOST_FUEL, //
	LOST_COLLECTABLE, //
} GameEvent;

void onEvent(GameEvent event);

#endif /* INC_EVENTS_H_ */
