/*
 * text-handling.c
 *
 *  Created on: Mar 20, 2019
 *      Author: diegomtassis
 */

#include "../inc/text_handling.h"

#include <genesis.h>

void appendAndDrawText(const char *text, u16 *xOffset, u16 *yOffset) {

	const char *left = text;

//	while (strlen(left) > 0) {
//		VDP_drawText(text, *xOffset, *yOffset);
//	}

	VDP_drawText(text, *xOffset, *yOffset);
	*xOffset += strlen(text);
}
