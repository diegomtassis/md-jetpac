/*
 * planetary_system_pcentauri.c
 *
 *  Created on: Jun 6, 2023
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../../inc/planetary_systems.h"
#include "../../res/ansimuz.h"
#include "../../inc/fwk/vdp_utils.h"

static u16 idx_tile_bg_ansimuz;

static Map *bg;

static void releasePlanetarySystem();

void initPlanetarySystemPCentauri() {

    PAL_setPalette(PAL2, palette_ansimuz_sf.data, DMA);
    VDP_setBackgroundColor(32);

    idx_tile_bg_ansimuz = idx_tile_malloc;
    u16 vram_idx = idx_tile_bg_ansimuz;

    VDP_loadTileSet(&tileset_ansimuz_sf, vram_idx, DMA);
    vram_idx += tileset_ansimuz_sf.numTile;

    bg = MAP_create(&map_ansimuz_sf, BG_B, TILE_ATTR_FULL(PAL2, FALSE, FALSE, FALSE, idx_tile_bg_ansimuz));

    MAP_scrollTo(bg, 100, 0);
}

void releasePlanetarySystemPCentauri() {

    releasePlanetarySystem();
}

void releasePlanetarySystem() {

    VDP_clearPlane(BG_B, TRUE);

    PAL_setPalette(PAL2, palette_black, DMA);

    MEM_free(bg);

    idx_tile_malloc = idx_tile_bg_ansimuz;
}
