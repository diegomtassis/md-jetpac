/*
 * levels.c
 *
 *  Created on: Apr 7, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../inc/levels.h"
#include "../inc/jetman.h"
#include "../res/gfx.h"

static const Platform PLATFORM_LEFT = { .xPos = 4, .yPos = 11, .length = 6 };
static const Platform PLATFORM_MIDDLE = { .xPos = 15, .yPos = 14, .length = 4 };
static const Platform PLATFORM_RIGHT = { .xPos = 24, .yPos = 8, .length = 6 };
static const Platform FLOOR = { .xPos = 0, .yPos = 25, .length = 32 };

static Level current_level;

static u16 palette[64];
static u16 idx_tile_malloc;

static u16 idx_tile_oneup;
static u16 idx_tile_platform;
static u16 idx_tile_floor;
static u16 idx_tile_platform;

static void createLevel(Level* level);
static void startLevel(const Level* level);

static void loadLevelResources();
static void loadTile(const TileSet * tileset, u16 * idx_tile);

static void drawInfoPanel();
static void drawLevel(VDPPlan plan, const Level * level);
static void drawPlatform(VDPPlan plan, const Platform * platform, u16 idx_tile);

void runGame() {

	createLevel(&current_level);
	startLevel(&current_level);

	startJetman(&current_level);
}

static void createLevel(Level* level) {

	level->floor = &FLOOR;

	level->platforms[0] = &PLATFORM_LEFT;
	level->platforms[1] = &PLATFORM_MIDDLE;
	level->platforms[2] = &PLATFORM_RIGHT;
}

static void startLevel(const Level* level) {

	SYS_disableInts();

	// initialization
	VDP_clearPlan(PLAN_A, TRUE);
	VDP_clearPlan(PLAN_B, TRUE);

	VDP_setPaletteColors(0, (u16*) palette_black, 16);

	loadLevelResources();

	drawInfoPanel();

	drawLevel(PLAN_B, level);

	SYS_enableInts();

	// prepare palette
	memcpy(&palette[0], game_palette.data, 16 * 2);

	// fade in
	VDP_fadeIn(0, (1 * 16) - 1, palette, 60, FALSE);
}

static void loadLevelResources() {

	// load background
	idx_tile_malloc = TILE_USERINDEX;

	// load floor & platform
	loadTile(&floor, &idx_tile_floor);
	loadTile(&platform, &idx_tile_platform);
}

static void drawInfoPanel() {

	loadTile(oneup.tileset, &idx_tile_oneup);
	VDP_setTextPalette(PAL3);
	VDP_setTextPlan(PLAN_A);

	VDP_drawText("1UP", 3, 2);
	VDP_drawText("3", 8, 2);
	VDP_setTileMapXY(PLAN_A, TILE_ATTR_FULL(PAL3, TRUE, FALSE, FALSE, idx_tile_oneup), 9, 2);
	VDP_drawText("HI", 15, 2);
	VDP_drawText("2UP", 27, 2);
	VDP_drawText("000000", 1, 3);
	VDP_drawText("000000", 13, 3);
	VDP_drawText("000000", 25, 3);
}

static void drawLevel(VDPPlan plan, const Level * level) {

	// draw floor
	drawPlatform(PLAN_B, level->floor, idx_tile_floor);

	// draw platforms
	drawPlatform(PLAN_B, level->platforms[0], idx_tile_platform);
	drawPlatform(PLAN_B, level->platforms[1], idx_tile_platform);
	drawPlatform(PLAN_B, level->platforms[2], idx_tile_platform);
}

static void drawPlatform(VDPPlan plan, const Platform * platform, u16 idx_tile) {

	VDP_setTileMapXY(plan, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, idx_tile), platform->xPos, platform->yPos);
	VDP_fillTileMapRect(plan, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, idx_tile + 1), platform->xPos + 1,
			platform->yPos, platform->length - 2, 1);
	VDP_setTileMapXY(plan, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, idx_tile + 2),
			platform->xPos + platform->length - 1, platform->yPos);
}

static void loadTile(const TileSet * tileset, u16 * idx_tile) {

	*idx_tile = idx_tile_malloc;
	VDP_loadTileSet(tileset, *idx_tile, CPU);
	idx_tile_malloc += tileset->numTile;
}
