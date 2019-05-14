/*
 * spaceship.c
 *
 *  Created on: May 12, 2019
 *      Author: diegomtassis
 */

#include "../inc/spaceship.h"

#include <maths.h>
#include <memory.h>
#include <sprite_eng.h>
#include <vdp_tile.h>

#include "../inc/physics.h"
#include "../res/sprite.h"

#define BASE	0x01
#define MID		0x02
#define TOP		0x04
#define WHOLE	0x08

static Object_f16* createModule(u8 module, V2u16 pos);

void startSpaceship(Level* level) {

	Spaceship* spaceship = MEM_alloc(sizeof(Spaceship));
	level->spaceship = spaceship;

	if (UNASSEMBLED & level->def.spaceship_def.type) {

		spaceship->top_object = *createModule(TOP, level->def.spaceship_def.top_pos);
		spaceship->top_sprite = SPR_addSprite(&u1_top_sprite, fix16ToInt(spaceship->top_object.pos.x),
				fix16ToInt(spaceship->top_object.pos.y), TILE_ATTR(PAL0, TRUE, FALSE, FALSE));

		spaceship->mid_object = *createModule(MID, level->def.spaceship_def.middle_pos);
		spaceship->mid_sprite = SPR_addSprite(&u1_middle_sprite, fix16ToInt(spaceship->mid_object.pos.x),
				fix16ToInt(spaceship->mid_object.pos.y), TILE_ATTR(PAL0, TRUE, FALSE, FALSE));

		spaceship->base_object = *createModule(BASE, level->def.spaceship_def.base_pos);
		spaceship->base_sprite = SPR_addSprite(&u1_base_sprite, fix16ToInt(spaceship->base_object.pos.x),
				fix16ToInt(spaceship->base_object.pos.y), TILE_ATTR(PAL0, TRUE, FALSE, FALSE));

	} else {
		// ASSEMBLED
		spaceship->base_object = *createModule(WHOLE, level->def.spaceship_def.base_pos);
		spaceship->base_sprite = SPR_addSprite(&u1_sprite, fix16ToInt(spaceship->base_object.pos.x),
				fix16ToInt(spaceship->base_object.pos.y), TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
	}
}

void releaseSpaceship(Level* level) {

	if (level->spaceship->base_sprite) {
		SPR_releaseSprite(level->spaceship->base_sprite);
		level->spaceship->base_sprite = 0;
	}

	if (level->spaceship->mid_sprite) {
		SPR_releaseSprite(level->spaceship->mid_sprite);
		level->spaceship->mid_sprite = 0;
	}

	if (level->spaceship->top_sprite) {
		SPR_releaseSprite(level->spaceship->top_sprite);
		level->spaceship->top_sprite = 0;
	}

	MEM_free(level->spaceship);
	level->spaceship = 0;
}

static Object_f16* createModule(u8 module, V2u16 pos) {

	Object_f16* object = MEM_alloc(sizeof(Object_f16));

	object->pos.x = FIX16(pos.x);
	object->pos.y = FIX16(pos.y);

	object->size.x = 16;
	if (WHOLE & module) {
		object->size.y = 48;
	} else {
		object->size.y = 16;
	}

	object->box.w = object->size.x;
	object->box.h = object->size.y;

	updateBox(object);

	return object;
}

