/*
 * main.c
 *
 *  Created on: Apr 06, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

void showDisclaimer();
void showSplashScreen();

int main() {

	// JOY_init();
	// JOY_setEventHandler(&inputHandler);

	showDisclaimer();

	while (1) {

		VDP_waitVSync();
	}

	return (0);
}



void showDisclaimer() {

	VDP_setTextPalette(PAL2);

	u16 xPos = 1;
	u16 yPos = 1;

	VDP_drawText("Video Game Program: JetPac MD", xPos, yPos++);

	yPos++;

	VDP_drawText("Port by Diego Martin, 2019", xPos, yPos++);

	yPos++;

	VDP_drawText("Original program written by Tim and", xPos, yPos++);
	VDP_drawText("Chris Stamper, 1983", xPos, yPos++);

	yPos++;

	VDP_drawText("JetPac is a Ultimate Play the Game", xPos, yPos++);
	VDP_drawText("/ RARE property", xPos, yPos++);

	yPos++;

	VDP_drawText("This information priority one", xPos, yPos++);

	yPos++;

	VDP_drawText("End of Line", xPos, yPos++);
}

void showSplashScreen() {

}
