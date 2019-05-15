/*
 * spaceship.c
 *
 *  Created on: May 12, 2019
 *      Author: diegomtassis
 */

#include "../inc/spaceship.h"

#include <genesis.h>

#include "../inc/commons.h"
#include "../inc/physics.h"
#include "../res/sprite.h"

#define BASE	0x01
#define MID		0x02
#define TOP		0x04
#define WHOLE	0x08

static Object_f16* createModule(u8 module, V2u16 pos);

static void handleAssembly(Level* level);

void startSpaceship(Level* level) {

	Spaceship* spaceship = MEM_alloc(sizeof(Spaceship));
	level->spaceship = spaceship;
	spaceship->grabbed = FALSE;

	if (UNASSEMBLED & level->def.spaceship_def.type) {

		spaceship->step = UNASSEMBLED;

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
		spaceship->step = ASSEMBLED;
		spaceship->base_object = *createModule(WHOLE, level->def.spaceship_def.base_pos);
		spaceship->base_sprite = SPR_addSprite(&u1_sprite, fix16ToInt(spaceship->base_object.pos.x),
				fix16ToInt(spaceship->base_object.pos.y), TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
	}
}

void handleSpaceship(Level* level) {

	if (level->spaceship->step < ASSEMBLED) {
		handleAssembly(level);
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

static void handleAssembly(Level* level) {

	Spaceship* spaceship = level->spaceship;
	V2f16 jetman_pos = level->jetman->object.pos;

	if (spaceship->step == UNASSEMBLED) {
		if (isAbove(spaceship->mid_object.box, spaceship->base_object.box)) {
			if (spaceship->grabbed) {
				// Release right over the base
				spaceship->grabbed = FALSE;
				spaceship->mid_object.pos.x = spaceship->base_object.pos.x;
				spaceship->mid_object.pos.y = spaceship->base_object.pos.y - FIX16_16; // Directly above the base
				spaceship->step = MID_SET;
			} else {
				// Fall
			}

		} else if (spaceship->grabbed || overlap(level->jetman->object.box, spaceship->mid_object.box)) {

			// The jetman has grabbed the middle section of the rocket
			spaceship->grabbed = TRUE;
			spaceship->mid_object.pos.x = jetman_pos.x;
			spaceship->mid_object.pos.y = fix16Add(jetman_pos.y, FIX16_8); // the rocket part is 8px shorter than the jetman
		}
		updateBox(&spaceship->mid_object);
		SPR_setPosition(spaceship->mid_sprite, fix16ToInt(spaceship->mid_object.pos.x),
				fix16ToInt(spaceship->mid_object.pos.y));

	} else if (spaceship->step == MID_SET) {

		if (isAbove(spaceship->top_object.box, spaceship->mid_object.box)) {
			if (spaceship->grabbed) {
				// Release right over the base
				spaceship->grabbed = FALSE;
				spaceship->top_object.pos.x = spaceship->base_object.pos.x;
				spaceship->top_object.pos.y = spaceship->base_object.pos.y - FIX16_32; // Directly above the base
				spaceship->step = ASSEMBLED;
			} else {
				// Fall
			}

		} else if (spaceship->grabbed || overlap(level->jetman->object.box, spaceship->top_object.box)) {

			// The jetman has grabbed the top section of the rocket
			spaceship->grabbed = TRUE;
			spaceship->top_object.pos.x = jetman_pos.x;
			spaceship->top_object.pos.y = fix16Add(jetman_pos.y, FIX16_8); // the rocket part is 8px shorter than the jetman
		}
		updateBox(&spaceship->top_object);
		SPR_setPosition(spaceship->top_sprite, fix16ToInt(spaceship->top_object.pos.x),
				fix16ToInt(spaceship->top_object.pos.y));
	}
}
