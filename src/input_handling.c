/*
 * input-handling.c
 *
 *  Created on: Mar 19, 2019
 *      Author: diegomtassis
 */

#include "../inc/input_handling.h"

#include <genesis.h>

#include "../inc/text_handling.h"

void logButtonStatus(const char *buttonId, const char *isPushed, u16 x, u16 y);

void inputHandler(u16 joy, u16 changed, u16 state) {

	u16 yTop = 10;
	u16 xFirstColumn = 5;
	u16 xSecondColumn = 20;

	logInputButton(joy, BUTTON_START, changed, state, "Button Start", xFirstColumn, yTop);

	logInputButton(joy, BUTTON_UP, changed, state, "D-Pad Up", xFirstColumn, yTop + 2);
	logInputButton(joy, BUTTON_DOWN, changed, state, "D-Pad Down", xFirstColumn, yTop + 3);
	logInputButton(joy, BUTTON_LEFT, changed, state, "D-Pad Left", xFirstColumn, yTop + 4);
	logInputButton(joy, BUTTON_RIGHT, changed, state, "D-Pad Right", xFirstColumn, yTop + 5);

	logInputButton(joy, BUTTON_A, changed, state, "Button A", xFirstColumn, yTop + 7);
	logInputButton(joy, BUTTON_B, changed, state, "Button B", xFirstColumn, yTop + 8);
	logInputButton(joy, BUTTON_C, changed, state, "Button C", xFirstColumn, yTop + 9);

	logInputButton(joy, BUTTON_X, changed, state, "Button X", xSecondColumn, yTop + 7);
	logInputButton(joy, BUTTON_Y, changed, state, "Button Y", xSecondColumn, yTop + 8);
	logInputButton(joy, BUTTON_Z, changed, state, "Button Z", xSecondColumn, yTop + 9);
}

void logInputButton(u16 joy, u16 button, u16 changed, u16 state, const char *buttonName, u16 x, u16 y) {

	if (joy == JOY_1) {

		if (state & button) {
			logButtonStatus(buttonName, " 1", x, y);

		} else if (changed & button) {
			logButtonStatus(buttonName, " 0", x, y);
		}
	}
}

void logButtonStatus(const char *buttonName, const char *isPushed, u16 x, u16 y) {

	u16 xOffset = x;
	u16 yOffset = y;

	appendAndDrawText(buttonName, &xOffset, &yOffset);
	appendAndDrawText(isPushed, &xOffset, &yOffset);
}
