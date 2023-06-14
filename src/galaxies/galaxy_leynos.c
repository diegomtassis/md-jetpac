/*
 * galaxy_leynos.c
 *
 *  Created on: Jun 6, 2023
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../../inc/galaxies.h"
#include "../../res/leynos.h"
#include "../../inc/fwk/vdp_utils.h"

u16 idx_tile_bg_leynos;

Map *bg;

void initGalaxyLeynos() {

    PAL_setPalette(PAL2, palette_leynos.data, DMA);

    idx_tile_bg_leynos = idx_tile_malloc;
    u16 vram_idx = idx_tile_bg_leynos;

    VDP_loadTileSet(&tileset_leynos_stars, vram_idx, DMA);
    vram_idx += tileset_leynos_stars.numTile;

    VDP_loadTileSet(&tileset_leynos_planet, vram_idx, DMA);
    vram_idx += tileset_leynos_planet.numTile;

    bg = MAP_create(&map_leynos_all, BG_B, TILE_ATTR_FULL(PAL2, FALSE, FALSE, FALSE, idx_tile_bg_leynos));

    MAP_scrollTo(bg, 0, 0);
}

void releaseGalaxyLeynos() {

    VDP_clearPlane(BG_B, TRUE);

    PAL_setPalette(PAL2, &palette_black, DMA);

    MEM_free(bg);

    idx_tile_malloc = idx_tile_bg_leynos;
}
