/*
 * levels.c
 *
 *  Created on: Apr 7, 2019
 *      Author: diegomtassis
 */

#include "../inc/levels.h"

#include <genesis.h>

#include "../inc/vdp_utils.h"
#include "../res/gfx.h"

static u16 palette[64];
static u16 idx_tile_malloc;

static u16 idx_tile_oneup;
static u16 idx_tile_platform;
static u16 idx_tile_floor;
static u16 idx_tile_platform;

static Platform* createPlatform(u16 pos_x_t, u16 pos_y_t, u16 length_t);

static Enemies* defineEnemies(Level * level);

static void loadLevelResources();

static void drawInfoPanel();
static void drawPlatforms(VDPPlan plan, const Level * level);
static void drawPlatform(VDPPlan plan, const Platform * platform, u16 idx_tile);

Level* createLevel() {

	Level* level = MEM_alloc(sizeof(Level));

	// level static elements
	level->floor = createPlatform(0, 25, 32);

	level->num_platforms = 3;
	level->platforms = MEM_alloc(level->num_platforms * sizeof(Platform*));

	level->platforms[0] = createPlatform(4, 11, 6);
	level->platforms[1] = createPlatform(15, 14, 4);
	level->platforms[2] = createPlatform(24, 8, 6);

	// define enemies
	level->enemies = defineEnemies(level);

	return level;
}

void startLevel(Level* level) {

	SYS_disableInts();

	// initialization
	VDP_clearPlan(PLAN_A, TRUE);
	VDP_clearPlan(PLAN_B, TRUE);

	VDP_setPaletteColors(0, (u16*) palette_black, 16);

	loadLevelResources();

	drawInfoPanel();

	drawPlatforms(PLAN_B, level);

	SYS_enableInts();

	// prepare palette
	memcpy(&palette[0], game_palette.data, 16 * 2);

	// fade in
	VDP_fadeIn(0, (1 * 16) - 1, palette, 60, FALSE);
}

static Platform* createPlatform(u16 pos_x_t, u16 pos_y_t, u16 length_t) {

	Platform* platform = MEM_alloc(sizeof(Platform));

	Vect2D_u16 pos_t = { .x = pos_x_t, .y = pos_y_t };
	platform->pos_t = pos_t;

	Vect2D_u16 size = { .x = length_t, .y = 1 };
	platform->size_t = size;

	Vect2D_f16 pos_px = { .x = FIX16(pos_x_t * 8), .y = FIX16(pos_y_t * 8) };
	platform->object.pos = pos_px;

	Vect2D_u16 size_px = { .x = length_t * 8, .y = 8 };
	platform->object.size = size_px;

	Box_f16* box = MEM_alloc(sizeof(Box_f16));
	box->w = size_px.x;
	box->h = size_px.y;
	platform->object.box = box;
	updateBox(&platform->object);

	return platform;
}

static Enemies* defineEnemies(Level * level) {

	Enemies* enemies = MEM_alloc(sizeof(Enemies));
	enemies->current_num_enemies = 0;
	enemies->max_num_enemies = 10;

	return enemies;
}

static void loadLevelResources() {

	// load background
	idx_tile_malloc = TILE_USERINDEX;

	// load floor & platform
	idx_tile_floor = loadTile(&floor, &idx_tile_malloc);
	idx_tile_platform = loadTile(&platform, &idx_tile_malloc);
}

static void drawInfoPanel() {

	idx_tile_oneup = loadTile(oneup.tileset, &idx_tile_malloc);
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

static void drawPlatforms(VDPPlan plan, const Level * level) {

	// draw floor
	drawPlatform(PLAN_B, level->floor, idx_tile_floor);

	// draw platforms
	for (u8 i = 0; i < level->num_platforms; i++) {
		drawPlatform(PLAN_B, level->platforms[i], idx_tile_platform);
	}
}

static void drawPlatform(VDPPlan plan, const Platform* platform, u16 idx_tile) {

	// left edge
	VDP_setTileMapXY(plan, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, idx_tile), platform->pos_t.x, platform->pos_t.y);

	// middle section
	VDP_fillTileMapRect(plan, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, idx_tile + 1), platform->pos_t.x + 1,
			platform->pos_t.y, platform->size_t.x - 2, 1);

	// right edge
	VDP_setTileMapXY(plan, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, idx_tile + 2),
			platform->pos_t.x + platform->size_t.x - 1, platform->pos_t.y);
}
