/*
 * vdp_utils.c
 *
 *  Created on: Apr 20, 2019
 *      Author: diegomtassis
 */

#include "../../inc/fwk/vdp_utils.h"

#include <genesis.h>

void resetTileMemory() {

	idx_tile_malloc = TILE_USERINDEX;
}

u16 loadTile(const TileSet * tileset, u16* idx_tile_malloc) {

	u16 idx_tile = *idx_tile_malloc;
	VDP_loadTileSet(tileset, idx_tile, CPU);
	*idx_tile_malloc += tileset->numTile;

	return idx_tile;
}
