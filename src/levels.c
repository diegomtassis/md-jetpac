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

static u16 idx_tile_platform;
static u16 idx_tile_floor;
static u16 idx_tile_platform;

static void releasePlatform(Platform*);

static void loadLevelResources();

static void drawPlatforms(VDPPlan plan, const Level * level);
static void drawPlatform(VDPPlan plan, const Platform * platform, u16 idx_tile);

Level* allocLevel() {

	return (Level*) MEM_alloc(sizeof(Level));
}

void startLevel(Level* level) {

	SYS_disableInts();

	// initialization
	VDP_clearPlan(PLAN_B, TRUE);

	VDP_setPaletteColors(0, (u16*) palette_black, 16);

	loadLevelResources();

	drawPlatforms(PLAN_B, level);

	SYS_enableInts();

	// prepare palette
	memcpy(&palette[0], game_palette.data, 16 * 2);

	// fade in
	VDP_fadeIn(0, (1 * 16) - 1, palette, 60, FALSE);
}

void releaseLevel(Level* level) {

	// release floor
	releasePlatform(level->floor);
	level->floor = 0;

	// release platforms
	for (u8 i = 0; i < level->num_platforms; i++) {
		releasePlatform(level->platforms[i]);
		level->platforms[i] = 0;
	}

	if (level->initPos) {
		MEM_free(level->initPos);
		level->initPos = 0;
	}

	MEM_free(level);
}

Platform* createPlatform(u16 pos_x_t, u16 pos_y_t, u16 length_t) {

	Platform* platform = MEM_alloc(sizeof(Platform));

	Vect2D_u16 pos_t = { .x = pos_x_t, .y = pos_y_t };
	platform->pos_t = pos_t;

	Vect2D_u16 size = { .x = length_t, .y = 1 };
	platform->size_t = size;

	Vect2D_f16 pos_px = { .x = FIX16(pos_x_t * 8), .y = FIX16(pos_y_t * 8) };
	platform->object.pos = pos_px;

	Vect2D_u16 size_px = { .x = length_t * 8, .y = 8 };
	platform->object.size = size_px;

	platform->object.box.w = size_px.x;
	platform->object.box.h = size_px.y;
	updateBox(&platform->object);

	return platform;
}

static void releasePlatform(Platform* platform) {

	MEM_free(platform);
}

static void loadLevelResources() {

	// load floor & platform
	idx_tile_floor = loadTile(&floor, &idx_tile_malloc);
	idx_tile_platform = loadTile(&platform, &idx_tile_malloc);
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
