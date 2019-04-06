/*
 * levels.c
 *
 *  Created on: Apr 7, 2019
 *      Author: diegomtassis
 */

#include <dma.h>
#include <memory.h>
#include <sys.h>
#include <types.h>
#include <vdp.h>
#include <vdp_bg.h>
#include <vdp_pal.h>
#include <vdp_tile.h>

#include "../res/gfx.h"

static const int FLOOR_Y = 25;

static void loadScreen();

static u16 palette[64];
static u16 idx_first_available;

static u16 idx_platform;
static u16 idx_floor;

void runGame() {

	loadScreen();

	while (TRUE) {

		VDP_waitVSync();
	}
}

static void loadScreen() {

	SYS_disableInts();

	// initialization
	VDP_clearPlan(PLAN_A, TRUE);
	VDP_clearPlan(PLAN_B, TRUE);
	VDP_setScreenWidth320();
	VDP_setScreenHeight224();

	VDP_setPaletteColors(0, (u16*) palette_black, 16);

	// load background
	idx_first_available = TILE_USERINDEX;

	VDP_drawText("Ready Player One", 11, 13);

	// load floor
	idx_floor = idx_first_available;
	VDP_loadTileSet(&floor, idx_floor, CPU);
	idx_first_available += floor.numTile;

	// draw floor

	VDP_setTileMapXY(PLAN_A, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, idx_floor), 4, FLOOR_Y);
	VDP_fillTileMapRect(PLAN_A, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, idx_floor + 1), 5, FLOOR_Y, 30, 1);
	VDP_setTileMapXY(PLAN_A, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, idx_floor + 2), 34, FLOOR_Y);

	// load platform
	idx_platform = idx_first_available;
	VDP_loadTileSet(&platform, idx_platform, CPU);
	idx_first_available += platform.numTile;

	// draw platforms

	SYS_enableInts();

	// prepare palettes
	memcpy(&palette[0], splash_image.palette->data, 16 * 2);

	// fade in
	VDP_fadeIn(0, (1 * 16) - 1, palette, 60, FALSE);
}
