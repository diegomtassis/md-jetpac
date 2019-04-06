/*
 * main.c
 *
 *  Created on: Apr 06, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../res/gfx.h"

static void handleInput();
static void joyEvent(u16 joy, u16 changed, u16 state);

static void showDisclaimer();
static void showSplashScreen();

u16 palette[64];
u16 ind;

int main() {

//	showDisclaimer();

	showSplashScreen();

//	JOY_setEventHandler(joyEvent);

	while (TRUE) {
//		handleInput();

		VDP_waitVSync();
	}

	return 0;
}

static void showDisclaimer() {

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

static void showSplashScreen() {

	SYS_disableInts();

	// initialization
	VDP_setScreenWidth256();
	VDP_setScreenHeight224();

	VDP_setPaletteColors(0, (u16*) palette_black, 16);

	// load background
	ind = TILE_USERINDEX;

//	u16 yOffset = (VDP_getScreenHeight() - splash_image.map->h) / (2 * 8);
	u16 yOffset = 3;

	VDP_drawImageEx(PLAN_B, &splash_image, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, yOffset, FALSE, TRUE);
	ind += splash_image.tileset->numTile;

	SYS_enableInts();

	// prepare palettes
	memcpy(&palette[0], splash_image.palette->data, 16 * 2);

	// fade in
	VDP_fadeIn(0, (1 * 16) - 1, palette, 60, FALSE);
}

static void handleInput() {

}

static void joyEvent(u16 joy, u16 changed, u16 state) {

}
