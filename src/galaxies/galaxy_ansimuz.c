/*
 * galaxy_leynos.c
 *
 *  Created on: Jun 6, 2023
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../../inc/galaxies.h"
#include "../../res/ansimuz.h"
#include "../../inc/fwk/vdp_utils.h"

static u16 idx_tile_bg_ansimuz;

static Map *bg;

static void releaseGalaxy();

void initGalaxyAnsimuzLegacy() {

    PAL_setPalette(PAL2, palette_ansimuz_legacy.data, DMA);
    VDP_setBackgroundColor(32);

    idx_tile_bg_ansimuz = idx_tile_malloc;
    u16 vram_idx = idx_tile_bg_ansimuz;

    VDP_loadTileSet(&tileset_ansimuz_legacy, vram_idx, DMA);
    vram_idx += tileset_ansimuz_legacy.numTile;

    bg = MAP_create(&map_ansimuz_legacy, BG_B, TILE_ATTR_FULL(PAL2, FALSE, FALSE, FALSE, idx_tile_bg_ansimuz));

    MAP_scrollTo(bg, 0, 0);
}

void releaseGalaxyAnsimuzLegacy() {

    releaseGalaxy();
}

void initGalaxyAnsimuzSF() {

    PAL_setPalette(PAL2, palette_ansimuz_sf.data, DMA);
    VDP_setBackgroundColor(32);

    idx_tile_bg_ansimuz = idx_tile_malloc;
    u16 vram_idx = idx_tile_bg_ansimuz;

    VDP_loadTileSet(&tileset_ansimuz_sf, vram_idx, DMA);
    vram_idx += tileset_ansimuz_sf.numTile;

    bg = MAP_create(&map_ansimuz_sf, BG_B, TILE_ATTR_FULL(PAL2, FALSE, FALSE, FALSE, idx_tile_bg_ansimuz));

    MAP_scrollTo(bg, 100, 0);
}

void releaseGalaxyAnsimuzSF() {

    releaseGalaxy();
}

void releaseGalaxy() {

    VDP_clearPlane(BG_B, TRUE);

    PAL_setPalette(PAL2, palette_black, DMA);

    MEM_free(bg);

    idx_tile_malloc = idx_tile_bg_ansimuz;
}