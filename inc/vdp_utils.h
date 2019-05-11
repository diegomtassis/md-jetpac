/*
 * vdp_utils.h
 *
 *  Created on: Apr 20, 2019
 *      Author: diegomtassis
 */

#ifndef INC_VDP_UTILS_H_
#define INC_VDP_UTILS_H_

#include <genesis.h>

#include "physics.h"

void resetTileMemory();

u16 loadTile(const TileSet *, u16*);

u16 idx_tile_malloc;

#endif /* INC_VDP_UTILS_H_ */

