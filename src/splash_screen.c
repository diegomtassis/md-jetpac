/*
 * splash_screen.c
 *
 *  Created on: Apr 06, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../res/gfx.h"

static u16 idx_tile_alloc;
static u16 palette[64];

void showSplashScreen() {

	SYS_disableInts();

	// clear screen
	VDP_clearPlane(BG_B, TRUE);

	PAL_setPalette(PAL0, palette_black, DMA);

	// load background
	idx_tile_alloc = TILE_USER_INDEX;

//	u16 yOffset = (VDP_getScreenHeight() - splash_image.map->h) / (2 * 8);
	u16 v_offset = 3; // (224 - 176) / (8 * 2) tiles

	VDP_drawImageEx(BG_B, &splash_image, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, idx_tile_alloc), 0, v_offset, FALSE,
			TRUE);
	idx_tile_alloc += splash_image.tileset->numTile;

	SYS_enableInts();

	// prepare palettes
	memcpy(&palette[0], splash_image.palette->data, 16 * 2);

	// fade in
	PAL_fadeIn(0, (1 * 16) - 1, palette, 50, FALSE);
}

void clearSplashScreen() {

	VDP_clearPlane(BG_B, TRUE);
}
