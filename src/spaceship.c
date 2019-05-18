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

#define WAITING		0x01
#define GRABBED		0x02
#define FALLING		0x04
#define DONE		0x08

#define SPEED_V_DOWN	FIX16(1.1)

static Object_f16* createModule(u8 module, V2u16 pos);

static void handleAssembly(Level* level);
static void handlePart(Object_f16* part, Sprite* sprite, u16 goal, fix16 height, Level* level);

void startSpaceship(Level* level) {

	Spaceship* spaceship = MEM_alloc(sizeof *spaceship);
	level->spaceship = spaceship;

	if (UNASSEMBLED & level->def.spaceship_def.type) {

		spaceship->step = UNASSEMBLED;
		spaceship->substep = WAITING;

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
		spaceship->substep = DONE;

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

	Object_f16* object = MEM_alloc(sizeof *object);

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

	if (spaceship->step == UNASSEMBLED) {
		handlePart(&spaceship->mid_object, spaceship->mid_sprite, MID_SET, FIX16_16, level);

	} else if (spaceship->step == MID_SET) {
		handlePart(&spaceship->top_object, spaceship->top_sprite, ASSEMBLED, FIX16_32, level);
	}
}

static void handlePart(Object_f16* part, Sprite* sprite, u16 goal, fix16 v_offset_px, Level* level) {

	Spaceship* spaceship = level->spaceship;
	V2f16 jetman_pos = level->jetman->object.pos;

	if (isAbove(part->box, spaceship->base_object.box)) {
		if (spaceship->substep & GRABBED) {
			// Release right over the base
			spaceship->substep = FALLING;
			part->pos.x = spaceship->base_object.pos.x;
		} else {
			// Fall
			part->pos.y += SPEED_V_DOWN;
			fix16 v_limit = spaceship->base_object.pos.y - v_offset_px;
			if (part->pos.y >= v_limit) {
				part->pos.y = v_limit;
				spaceship->step = goal;
			}
		}

	} else if ((spaceship->substep & GRABBED) || overlap(level->jetman->object.box, part->box)) {

		// The jetman has grabbed the a section of the rocket
		spaceship->substep = GRABBED;
		part->pos.x = jetman_pos.x;
		part->pos.y = fix16Add(jetman_pos.y, FIX16_8); // the rocket part is 8px shorter than the jetman
	}

	updateBox(part);
	SPR_setPosition(sprite, fix16ToInt(part->pos.x), fix16ToInt(part->pos.y));
}
