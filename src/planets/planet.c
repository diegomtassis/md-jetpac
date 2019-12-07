/*
 * planet.c
 *
 *  Created on: Dec 7, 2019
 *      Author: diegomtassis
 */

#include <types.h>

#include "../../inc/elements.h"
#include "../../inc/fwk/commons.h"
#include "../../inc/level.h"
#include "../../inc/spaceship.h"

void createDefaultPlatforms(Level level[static 1]) {

	level->floor = createPlatform(0, 25, 32);

	level->num_platforms = 3;
	level->platforms = MEM_calloc(level->num_platforms * sizeof(Platform*));

	level->platforms[0] = createPlatform(4, 11, 6);
	level->platforms[1] = createPlatform(15, 14, 4);
	level->platforms[2] = createPlatform(24, 8, 6);
}

void defineSpaceshipInDefaultPlanet(Level level[static 1], SpaceshipTypeDefinition type_definition, u16 init_step) {

	level->def.spaceship_def.type_definition = type_definition;
	level->def.spaceship_def.init_step = init_step;

	if (init_step == UNASSEMBLED) {
		setV2s16(&level->def.spaceship_def.base_pos, 168, 184);
		setV2s16(&level->def.spaceship_def.middle_pos, 128, 96);
		setV2s16(&level->def.spaceship_def.top_pos, 48, 72);

	} else {
		setV2s16(&level->def.spaceship_def.base_pos, 168, 152);
	}
}
