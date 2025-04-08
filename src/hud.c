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
#include "../inc/players.h"
#include "../inc/game_config.h"
#include "../inc/fwk/vdp_utils.h"
#include "../res/zx.h"

#define P1_LIVES_X 	8
#define P2_LIVES_X 	20

static u16 idx_tile_oneup;
static u16 idx_tile_laser;

static u16 highest_score = 0;
static bool showing_ammo = FALSE;

const char* default_lives = "0";

void initHud() {

	VDP_setTextPalette(PAL1);
	VDP_setTextPlane(BG_A);

	VDP_setTextPriority(TRUE);

	VDP_drawText("1UP", 3, 2);
	VDP_drawText(default_lives, P1_LIVES_X + 1, 2);
	idx_tile_oneup = loadTile(oneup.tileset, &idx_tile_malloc);
	VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, idx_tile_oneup), P1_LIVES_X + 2, 2);

	idx_tile_laser = loadTile(laser.tileset, &idx_tile_malloc);

	VDP_drawText(default_lives, P2_LIVES_X + 1, 2);
	VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, idx_tile_oneup), P2_LIVES_X + 2, 2);

	VDP_drawText("2UP", 27, 2);

	VDP_drawText("HI", 15, 2);
	VDP_drawText("000000", 1, 3);
	VDP_drawText("000000", 13, 3);
	VDP_drawText("000000", 25, 3);
}

void displayAmmo(bool show_ammo) {

	showing_ammo = show_ammo;
	if (showing_ammo) {
		VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, idx_tile_laser), P1_LIVES_X + 2, 3);
		VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, idx_tile_laser), P2_LIVES_X + 2, 3);
	} else {
		VDP_clearTileMapRect(BG_A, 10, 3, 1, 1);
		VDP_clearText(8, 3, 2);
	}
}

void updatePlayerHud(Player* player, u16 ammo) {

	if (!player) {
		return;
	}

	u8 lives = player->lives;
	char lives_text[2];
	uintToStr(lives, lives_text, 1);

	char score[6];
	sprintf(score, "%06d", player->score);

	if (player->id == P1) {

		VDP_drawText(lives_text, lives > 9 ? P1_LIVES_X : P1_LIVES_X + 1, 2);
		if (lives == 9) {
			VDP_clearText(P1_LIVES_X, 2, 1);
		}

		VDP_drawText(score, 1, 3);

	} else {

		VDP_drawText(lives_text, lives > 9 ? P2_LIVES_X : P2_LIVES_X + 1, 2);
		if (lives == 9) {
			VDP_clearText(P2_LIVES_X, 2, 1);
		}

		VDP_drawText(score, 25, 3);
	}

	updateAmmo(player->id, ammo);
}

void updateAmmo(u8 player_id, u16 ammo) {

	if (!showing_ammo) {
		return;
	}

	char text_ammo[2];
	sprintf(text_ammo, "%02d", ammo);

	VDP_drawText(text_ammo, player_id == P1 ? P1_LIVES_X : P2_LIVES_X, 3);
}

void registerScore(u16 new_score) {

	if (new_score > highest_score) {

		highest_score = new_score;

		char str_score[6];
		sprintf(str_score, "%06d", new_score);
		VDP_drawText(str_score, 13, 3);
	}
}
