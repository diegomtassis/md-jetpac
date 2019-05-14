/*
 * hud.c
 *
 *  Created on: May 11, 2019
 *      Author: diegomtassis
 */

#include "../inc/hud.h"

#include <genesis.h>

#include "../inc/vdp_utils.h"
#include "../res/gfx.h"

static u16 idx_tile_oneup;

static u16 highest_score = 0;

void initHud() {

	idx_tile_oneup = loadTile(oneup.tileset, &idx_tile_malloc);
	VDP_setTextPalette(PAL1);
	VDP_setTextPlan(PLAN_A);

	VDP_setTextPriority(TRUE);

	VDP_drawText("1UP", 3, 2);
	VDP_drawText("3", 8, 2);
	VDP_setTileMapXY(PLAN_A, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, idx_tile_oneup), 9, 2);
	VDP_drawText("HI", 15, 2);
	VDP_drawText("2UP", 27, 2);
	VDP_drawText("000000", 1, 3);
	VDP_drawText("000000", 13, 3);
	VDP_drawText("000000", 25, 3);
}

void updateHud(Game* game) {

	// p1 lives
	char lives[2];
	uintToStr(game->lives, lives, 1);
	VDP_drawText(lives, 8, 2);

	// p1 score
	char score[6];
	sprintf(score, "%06d", game->score);
	VDP_drawText(score, 1, 3);
}

void registerScore(u16 new_score) {

	if (new_score > highest_score) {

		highest_score = new_score;

		char str_score[6];
		sprintf(str_score, "%06d", new_score);
		VDP_drawText(str_score, 13, 3);
	}
}
