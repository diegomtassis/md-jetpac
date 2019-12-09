/*
 * hud.c
 *
 *  Created on: May 11, 2019
 *      Author: diegomtassis
 */

#include "../inc/hud.h"

#include <string.h>
#include <types.h>
#include <vdp.h>
#include <vdp_bg.h>
#include <vdp_tile.h>

#include "../inc/elements.h"
#include "../inc/config.h"
#include "../inc/fwk/vdp_utils.h"
#include "../res/gfx.h"

static u16 idx_tile_oneup;
static u16 idx_tile_laser;

static u16 highest_score = 0;
static bool showing_ammo = FALSE;

void initHud() {

	VDP_setTextPalette(PAL1);
	VDP_setTextPlan(PLAN_A);

	VDP_setTextPriority(TRUE);

	VDP_drawText("1UP", 3, 2);
	VDP_drawText("3", 9, 2);
	idx_tile_oneup = loadTile(oneup.tileset, &idx_tile_malloc);
	VDP_setTileMapXY(PLAN_A, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, idx_tile_oneup), 10, 2);

	idx_tile_laser = loadTile(laser.tileset, &idx_tile_malloc);

	VDP_drawText("2UP", 27, 2);

	VDP_drawText("HI", 15, 2);
	VDP_drawText("000000", 1, 3);
	VDP_drawText("000000", 13, 3);
	VDP_drawText("000000", 25, 3);
}

void displayAmmo(bool show_ammo) {

	showing_ammo = show_ammo;
	if (showing_ammo) {
		VDP_setTileMapXY(PLAN_A, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, idx_tile_laser), 10, 3);
	} else {
		VDP_clearTileMapRect(PLAN_A, 10, 3, 1, 1);
		VDP_clearText(8, 3, 2);
	}
}

void updateHud(Game* game, Jetman* jetman) {

	// p1 lives
	char lives[2];
	uintToStr(game->p1.lives, lives, 1);
	VDP_drawText(lives, 9, 2);

	// p1 score
	char score[6];
	sprintf(score, "%06d", game->p1.score);
	VDP_drawText(score, 1, 3);

	updateAmmo(jetman);
}

void updateAmmo(Jetman* jetman) {

	if (!jetman || !showing_ammo) {
		return;
	}

	char ammo[2];
	sprintf(ammo, "%02d", jetman->ammo);
	VDP_drawText(ammo, 8, 3);
}

void registerScore(u16 new_score) {

	if (new_score > highest_score) {

		highest_score = new_score;

		char str_score[6];
		sprintf(str_score, "%06d", new_score);
		VDP_drawText(str_score, 13, 3);
	}
}
