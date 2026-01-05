/*
 * planetary_system_kepler186.c
 *
 *  Created on: Jan 5, 2026
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../../inc/planetary_systems.h"
#include "../../res/solfeace.h"
#include "../../inc/fwk/vdp_utils.h"

static u16 idx_tile_bg_kepler186;

static void releasePlanetarySystem();

void initPlanetarySystemKepler186() {

    PAL_setPalette(PAL2, image_solfeace.palette->data, DMA);
    VDP_setBackgroundColor(32);

    idx_tile_bg_kepler186 = idx_tile_malloc;

    VDP_drawImageEx(BG_B, &image_solfeace, TILE_ATTR_FULL(PAL2, FALSE, FALSE, FALSE, idx_tile_bg_kepler186), 40, -4,
            FALSE, TRUE);

    idx_tile_malloc = idx_tile_bg_kepler186 + image_solfeace.tileset->numTile;
}

void releasePlanetarySystemKepler186() {

    releasePlanetarySystem();
}

static void releasePlanetarySystem() {

    VDP_clearPlane(BG_B, TRUE);

    PAL_setPalette(PAL2, palette_black, DMA);

    idx_tile_malloc = idx_tile_bg_kepler186;
}
