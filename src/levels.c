/*
 * levels.c
 *
 *  Created on: Apr 7, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

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

static u16 idx_tile_floor;
static u16 idx_tile_platform;

static void loadScreen();
static void loadPlatform(const TileSet * tileset, u16 * idx_tile);

static void drawPlatform(const Platform * platform, u16 idx_tile);

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
	idx_tile_first_available = TILE_USERINDEX;

	VDP_drawText("Ready Player One", 11, 2);

	// load floor & platform
	loadPlatform(&floor, &idx_tile_floor);
	loadPlatform(&platform, &idx_tile_platform);

	// draw floor
	drawPlatform(&FLOOR, idx_tile_floor);

	// draw platforms
	drawPlatform(&PLATFORM_LEFT, idx_tile_platform);
	drawPlatform(&PLATFORM_MIDDLE, idx_tile_platform);
	drawPlatform(&PLATFORM_RIGHT, idx_tile_platform);

	SYS_enableInts();

	// prepare palette
	memcpy(&palette[0], game_palette.data, 16 * 2);

	// fade in
	VDP_fadeIn(0, (1 * 16) - 1, palette, 60, FALSE);
}

static void loadPlatform(const TileSet * tileset, u16 * idx_tile) {

	*idx_tile = idx_tile_first_available;
	VDP_loadTileSet(tileset, *idx_tile, CPU);
	idx_tile_first_available += tileset->numTile;
}

void drawPlatform(const Platform * platform, u16 idx_tile) {

	VDP_setTileMapXY(PLAN_A, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, idx_tile), platform->xPos, platform->yPos);
	VDP_fillTileMapRect(PLAN_A, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, idx_tile + 1), platform->xPos + 1,
			platform->yPos, platform->length - 2, 1);
	VDP_setTileMapXY(PLAN_A, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, idx_tile + 2),
			platform->xPos + platform->length - 1, platform->yPos);
}
