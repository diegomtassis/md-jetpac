/*
 * levels.c
 *
 *  Created on: Apr 7, 2019
 *      Author: diegomtassis
 */

#include "../inc/levels.h"

#include <genesis.h>

#include "../inc/jetman.h"
#include "../res/gfx.h"

static Platform FLOOR = { .xPos = 0, .yPos = 25, .length = 32 };

static Level* current_level;

static u16 palette[64];
static u16 idx_tile_malloc;

static u16 idx_tile_oneup;
static u16 idx_tile_platform;
static u16 idx_tile_floor;
static u16 idx_tile_platform;

static Level* createLevel();
static void fillInPlatformBox(Platform*);
static void startLevel(const Level* level);

static void loadLevelResources();
static void loadTile(const TileSet * tileset, u16 * idx_tile);

static void drawInfoPanel();
static void drawLevel(VDPPlan plan, const Level * level);
static void drawPlatform(VDPPlan plan, const Platform * platform, u16 idx_tile);

void runGame() {

	current_level = createLevel();
	startLevel(current_level);

	startJetman(current_level);
}

static Level* createLevel() {

	Level* level = MEM_alloc(sizeof(Level));

	level->floor = &FLOOR;
	fillInPlatformBox(level->floor);

	level->num_platforms = 3;
	level->platforms = MEM_alloc(level->num_platforms * sizeof(Platform));

	Platform* platform;

	platform = &level->platforms[0];
	platform->xPos = 4;
	platform->yPos = 11;
	platform->length = 6;
	fillInPlatformBox(platform);

	platform = &level->platforms[1];
	platform->xPos = 15;
	platform->yPos = 14;
	platform->length = 4;
	fillInPlatformBox(platform);

	platform = &level->platforms[2];
	platform->xPos = 24;
	platform->yPos = 8;
	platform->length = 6;
	fillInPlatformBox(platform);

	return level;
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

static void fillInPlatformBox(Platform* platform) {

	platform->box = MEM_alloc(sizeof(Box));

	platform->box->x = 0;
	platform->box->y = 0;
	platform->box->w = 0;
	platform->box->h = 0;
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
	for (u8 i = 0; i < level->num_platforms; i++) {
		drawPlatform(PLAN_B, &level->platforms[i], idx_tile_platform);
	}
}

static void drawPlatform(VDPPlan plan, const Platform* platform, u16 idx_tile) {

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
