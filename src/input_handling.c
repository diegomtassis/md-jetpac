/*
 * input-handling.c
 *
 *  Created on: Mar 19, 2019
 *      Author: diegomtassis
 */

#include "../inc/input_handling.h"

#include <joy.h>
#include <vdp.h>
#include <vdp_bg.h>

static void recordButtonEvents(u16 joy, u16 changed, u16 state);

static void resetButtonEvents();

static u16 pressed;

static void doNothing(u16 joy, u16 changed, u16 state) {

}

void IH_waitForPress(u16 joy, u16 button) {

	// joy ignored for the moment, only JOY_1 supported

	JOY_setEventHandler(&recordButtonEvents);

	while (!(pressed & button)) {

		VDP_drawText("", 0, 0); // without this line the handling does not work
		VDP_waitVSync();
	}

	JOY_setEventHandler(&doNothing);
	resetButtonEvents();
}

static void recordButtonEvents(u16 joy, u16 changed, u16 state) {

	if (joy == JOY_1) {
		pressed |= (changed & state);
	}
}

static void resetButtonEvents() {

	pressed = 0;
}
