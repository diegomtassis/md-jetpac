/*
 * levels.c
 *
 *  Created on: Apr 7, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../inc/jetman.h"
#include "../res/gfx.h"

typedef struct {
	u8 xPos;
	u8 yPos;
	u8 length;
} Platform;

static const Platform PLATFORM_LEFT = { .xPos = 8, .yPos = 11, .length = 6 };
static const Platform PLATFORM_MIDDLE = { .xPos = 19, .yPos = 14, .length = 4 };
static const Platform PLATFORM_RIGHT = { .xPos = 28, .yPos = 8, .length = 6 };
static const Platform FLOOR = { .xPos = 4, .yPos = 25, .length = 32 };

static u16 palette[64];
static u16 idx_tile_first_available;

static u16 idx_tile_oneup;
static u16 idx_tile_platform;
static u16 idx_tile_floor;
static u16 idx_tile_platform;

static void loadLevel();
static void loadTile(const TileSet * tileset, u16 * idx_tile);

static void drawPlatform(VDPPlan plan, const Platform * platform, u16 idx_tile);

void runGame() {

	loadLevel();

	startJetman();

	while (TRUE) {

		VDP_waitVSync();
	}
}

static void loadLevel() {

	SYS_disableInts();

	// initialization
	VDP_clearPlan(PLAN_A, TRUE);
	VDP_clearPlan(PLAN_B, TRUE);
	VDP_setScreenWidth320();
	VDP_setScreenHeight224();

	VDP_setPaletteColors(0, (u16*) palette_black, 16);

	// load background
	idx_tile_first_available = TILE_USERINDEX;

	// info panel
	loadTile(oneup.tileset, &idx_tile_oneup);
	VDP_setTextPalette(PAL3);
	VDP_setTextPlan(PLAN_A);

	VDP_drawText("1UP", 7, 2);
	VDP_drawText("3", 12, 2);
	VDP_setTileMapXY(PLAN_A, TILE_ATTR_FULL(PAL3, TRUE, FALSE, FALSE, idx_tile_oneup), 13, 2);
	VDP_drawText("HI", 19, 2);
	VDP_drawText("2UP", 31, 2);
	VDP_drawText("000000", 5, 3);
	VDP_drawText("000000", 17, 3);
	VDP_drawText("000000", 29, 3);

	// load floor & platform
	loadTile(&floor, &idx_tile_floor);
	loadTile(&platform, &idx_tile_platform);

	// draw floor
	drawPlatform(PLAN_B, &FLOOR, idx_tile_floor);

	// draw platforms
	drawPlatform(PLAN_B, &PLATFORM_LEFT, idx_tile_platform);
	drawPlatform(PLAN_B, &PLATFORM_MIDDLE, idx_tile_platform);
	drawPlatform(PLAN_B, &PLATFORM_RIGHT, idx_tile_platform);

	SYS_enableInts();

	// prepare palette
	memcpy(&palette[0], game_palette.data, 16 * 2);

	// fade in
	VDP_fadeIn(0, (1 * 16) - 1, palette, 60, FALSE);
}

static void loadTile(const TileSet * tileset, u16 * idx_tile) {

	*idx_tile = idx_tile_first_available;
	VDP_loadTileSet(tileset, *idx_tile, CPU);
	idx_tile_first_available += tileset->numTile;
}

void drawPlatform(VDPPlan plan, const Platform * platform, u16 idx_tile) {

	VDP_setTileMapXY(plan, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, idx_tile), platform->xPos, platform->yPos);
	VDP_fillTileMapRect(plan, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, idx_tile + 1), platform->xPos + 1,
			platform->yPos, platform->length - 2, 1);
	VDP_setTileMapXY(plan, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, idx_tile + 2),
			platform->xPos + platform->length - 1, platform->yPos);
}
