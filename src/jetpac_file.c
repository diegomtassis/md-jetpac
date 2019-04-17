/*
 * jetpac_file.c
 *
 *  Created on: Apr 06, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>


void showDisclaimer() {

	VDP_setTextPalette(PAL2);

	u16 xPos = 0;
	u16 yPos = 3;

	VDP_drawText("Video Game Program: JetPac MD", xPos, yPos++);

	yPos++;

	VDP_drawText("Port by Diego Martin, 2019", xPos, yPos++);

	yPos++;

	VDP_drawText("Original program written by Tim", xPos, yPos++);
	VDP_drawText("and Chris Stamper, 1983", xPos, yPos++);

	yPos++;

	VDP_drawText("JetPac is a Ultimate Play the", xPos, yPos++);
	VDP_drawText("Game / RARE property", xPos, yPos++);

	yPos++;

	VDP_drawText("This information priority one", xPos, yPos++);

	yPos++;

	VDP_drawText("End of Line", xPos, yPos++);
}
