/*
 * splash_screen.c
 *
 *  Created on: Apr 06, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../res/gfx.h"

static u16 palette[64];
static u16 ind;

void showSplashScreen() {

	SYS_disableInts();

	// initialization
	VDP_setScreenWidth256();

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
