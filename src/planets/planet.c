/*
 * planet.c
 *
 *  Created on: Dec 7, 2019
 *      Author: diegomtassis
 */

#include "../../inc/elements.h"
#include "../../inc/fwk/commons.h"
#include "../../inc/level.h"


void createDefaultPlatforms(Level level[static 1]) {

    level->floor = createPlatform(0, 25, 32);

    level->num_platforms = 3;
    level->platforms = MEM_calloc(level->num_platforms * sizeof(Platform*));

    level->platforms[0] = createPlatform(4, 11, 6);
    level->platforms[1] = createPlatform(15, 14, 4);
    level->platforms[2] = createPlatform(24, 8, 6);
}
