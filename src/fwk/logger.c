/*
 * logger.c
 *
 *  Created on: May 25, 2019
 *      Author: diegomtassis
 */

#include "../inc/fwk/logger.h"

#include <genesis.h>

#define DEFAULT_LOGGING_LINE 6

#define DEFAULT_LOGGING_WAIT 1000

void log_info_xy(const char *message, u16 x, u16 y) {

	VDP_clearTextLine(y);
	VDP_drawText(message, x, y);
	waitMs(DEFAULT_LOGGING_WAIT);
	VDP_clearTextLine(y);
}

void log_info_x(const char *message, u16 x) {

	log_info_xy(message, x, DEFAULT_LOGGING_LINE);
}

void log_memory() {

	char message[20];
	sprintf(message, "Free %06u", MEM_getFree());

	VDP_clearTextLine(DEFAULT_LOGGING_LINE);
	VDP_drawText(message, 1, DEFAULT_LOGGING_LINE);
}
