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
#include "../inc/fwk/vdp_utils.h"
#include "../inc/config/game_config.h"
#include "../inc/players.h"
#include "../res/zx.h"

#define P1_LIVES_X 8
#define P2_LIVES_X 20

#define P1_SCORE_X 1
#define P2_SCORE_X 25

static u16 idx_tile_oneup;
static u16 idx_tile_laser;

static u16 highest_score = 0;
static bool showing_ammo = FALSE;

const char *default_lives = "0";

static void updateLives(u8 lives, u16 lives_x);
static void updateScore(int score, u16 score_x);
static void updateAmmo(u8 ammo, u16 ammo_x);

void HUD_init() {

    VDP_setTextPalette(PAL1);
    VDP_setTextPlane(BG_A);    
    VDP_setTextPriority(TRUE);
    idx_tile_oneup = loadTile(oneup.tileset, &idx_tile_malloc);

    // P1 score
    VDP_drawText("1UP", 3, 2);
    VDP_drawText("000000", 1, 3);

    // P1 lives
    VDP_drawText(default_lives, P1_LIVES_X + 1, 2);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, idx_tile_oneup), P1_LIVES_X + 2, 2);

    // Highest score
    VDP_drawText("HI", 15, 2);
    VDP_drawText("000000", 13, 3);
    
    // P2 lives
    VDP_drawText(default_lives, P2_LIVES_X + 1, 2);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, idx_tile_oneup), P2_LIVES_X + 2, 2);    

    // P2 score
    VDP_drawText("2UP", 27, 2);
    VDP_drawText("000000", 25, 3);
        
    idx_tile_laser = loadTile(laser.tileset, &idx_tile_malloc);

}

void HUD_setupAmmoCounter(bool show_ammo) {

    showing_ammo = show_ammo;
    if (showing_ammo) {
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, idx_tile_laser), P1_LIVES_X + 2, 3);
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, idx_tile_laser), P2_LIVES_X + 2, 3);
    } else {
        VDP_clearTileMapRect(BG_A, 10, 3, 1, 1);
        VDP_clearText(8, 3, 2);
    }
}

void HUD_updatePlayerHud(Player *player, u16 ammo) {

    if (!player) {
        return;
    }

    if (player->id == P1) {
        updateLives(player->lives, P1_LIVES_X);
        updateScore(player->score, P1_SCORE_X);
        updateAmmo(ammo, P1_LIVES_X);
    } else {
        updateLives(player->lives, P2_LIVES_X);
        updateScore(player->score, P2_SCORE_X);
        updateAmmo(ammo, P2_LIVES_X);
    }
}

void HUD_updateAmmo(u8 player_id, u16 ammo) {

    updateAmmo(ammo, player_id == P1 ? P1_LIVES_X : P2_LIVES_X);
}

void HUD_registerScore(u16 new_score) {

    if (new_score > highest_score) {

        highest_score = new_score;

        char str_score[6];
        sprintf(str_score, "%06d", new_score);
        VDP_drawText(str_score, 13, 3);
    }
}

static void updateLives(u8 lives, u16 lives_x) {

    char lives_text[3];
    sprintf(lives_text, "%02d", lives);

    if (lives > 9) {
        VDP_drawText(lives_text, lives_x, 2);
    } else {
        if (lives == 9) {
            VDP_clearText(lives_x, 2, 1);
        }
        VDP_drawText(lives_text + 1, lives_x + 1, 2);
    }
}

static void updateScore(int score, u16 score_x) {

    char score_text[7];
    sprintf(score_text, "%06d", score);
    VDP_drawText(score_text, score_x, 3);
}

void updateAmmo(u8 ammo, u16 ammo_x) {

    if (!showing_ammo) {
        return;
    }

    char text_ammo[2];
    sprintf(text_ammo, "%02d", ammo);

    VDP_drawText(text_ammo, ammo_x, 3);
}
