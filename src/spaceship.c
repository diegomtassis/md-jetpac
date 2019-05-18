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
#include "../inc/level.h"
#include "../res/sprite.h"

#define BASE	0x01
#define MID		0x02
#define TOP		0x04
#define WHOLE	0x08

#define NONE	0x01
#define WAITING		0x02
#define GRABBED		0x04
#define FALLING		0x08
#define DONE		0x10

#define SPEED_0	FIX16(0)
#define SPEED_V_DOWN	FIX16(0.9)

#define FUEL_HEIGHT 16
#define FUEL_WIDTH 16

#define FUEL_MIN_POS_H_PX_S16	LEFT_POS_H_PX_S16
#define FUEL_MAX_POS_H_PX_S16	RIGHT_POS_H_PX_S16 - 16

#define FUEL_MIN_POS_H_PX_F16	FIX16(FUEL_MIN_POS_H_PX_S16)
#define FUEL_MAX_POS_H_PX_F16	FIX16(FUEL_MAX_POS_H_PX_S16)

static Object_f16* createModule(u8 module, V2u16 pos);

static void handleAssembly(Level* level);
static void handleFuelling(Level* level);
static void handlePart(Object_f16* part, Sprite* sprite, u16 goal, fix16 height, Level* level);
static void mergeParts(Spaceship* spaceship);

u16 default_sprite_attrs = TILE_ATTR(PAL0, TRUE, FALSE, FALSE);

void startSpaceship(Level* level) {

	Spaceship* spaceship = MEM_alloc(sizeof *spaceship);
	level->spaceship = spaceship;

	if (UNASSEMBLED & level->def.spaceship_def.type) {

		spaceship->step = UNASSEMBLED;
		spaceship->substep = WAITING;

		spaceship->top_object = *createModule(TOP, level->def.spaceship_def.top_pos);
		spaceship->top_sprite = SPR_addSprite(&u1_top_sprite, fix16ToInt(spaceship->top_object.pos.x),
				fix16ToInt(spaceship->top_object.pos.y), default_sprite_attrs);

		spaceship->mid_object = *createModule(MID, level->def.spaceship_def.middle_pos);
		spaceship->mid_sprite = SPR_addSprite(&u1_middle_sprite, fix16ToInt(spaceship->mid_object.pos.x),
				fix16ToInt(spaceship->mid_object.pos.y), default_sprite_attrs);

		spaceship->base_object = *createModule(BASE, level->def.spaceship_def.base_pos);
		spaceship->base_sprite = SPR_addSprite(&u1_base_sprite, fix16ToInt(spaceship->base_object.pos.x),
				fix16ToInt(spaceship->base_object.pos.y), default_sprite_attrs);

	} else {
		// ASSEMBLED
		spaceship->step = ASSEMBLED;
		spaceship->substep = NONE; // Fuel

		spaceship->base_object = *createModule(WHOLE, level->def.spaceship_def.base_pos);
		spaceship->base_sprite = SPR_addSprite(&u1_sprite, fix16ToInt(spaceship->base_object.pos.x),
				fix16ToInt(spaceship->base_object.pos.y), TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
	}

	spaceship->fuel_sprite = 0;
}

void handleSpaceship(Level* level) {

	if (level->spaceship->step < ASSEMBLED) {
		handleAssembly(level);

	} else if (level->spaceship->step >= ASSEMBLED && level->spaceship->step < READY) {
		handleFuelling(level);
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

	if (level->spaceship->fuel_sprite) {
		SPR_releaseSprite(level->spaceship->fuel_sprite);
		level->spaceship->fuel_sprite = 0;
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

		if (spaceship->step == ASSEMBLED) {
			// Change to the complete sprite when assembled
			mergeParts(spaceship);
			spaceship->substep = NONE;
		}
	}
}

static void handleFuelling(Level* level) {

	Spaceship* spaceship = level->spaceship;

	if (spaceship->step < ASSEMBLED || spaceship->step == READY) {
		return;
	}

	if (spaceship->substep & NONE) {

		// initialize a new fuel load
		spaceship->fuel_object.pos.x = randomInRangeFix16(FUEL_MIN_POS_H_PX_F16, FUEL_MAX_POS_H_PX_F16);
		spaceship->fuel_object.pos.y = TOP_POS_V_PX_F16;
		spaceship->fuel_object.size.x = FUEL_WIDTH;
		spaceship->fuel_object.size.y = FUEL_HEIGHT;
		spaceship->fuel_object.box.w = FUEL_WIDTH;
		spaceship->fuel_object.box.h = FUEL_HEIGHT;
		spaceship->substep = FALLING;
		spaceship->fuel_object.mov.y = SPEED_V_DOWN;
		updateBox(&spaceship->fuel_object);

		if (!spaceship->fuel_sprite) {
			spaceship->fuel_sprite = SPR_addSprite(&fuel_sprite, fix16ToInt(spaceship->fuel_object.pos.x),
					fix16ToInt(spaceship->fuel_object.pos.y), default_sprite_attrs);
		}

	} else if (spaceship->substep & WAITING) {

		if (overlap(level->jetman->object.box, spaceship->fuel_object.box)) {
			// jetman just grabbed the fuel load
			spaceship->substep = GRABBED;
			spaceship->fuel_object.mov.y = SPEED_0;
		}

	} else if (spaceship->substep & GRABBED) {

		// the fuel load is in possession of jetman
		if (isAbove(spaceship->fuel_object.box, spaceship->base_object.box)) {
			spaceship->fuel_object.pos.x = spaceship->base_object.pos.x;
			spaceship->substep = FALLING;
			spaceship->fuel_object.mov.y = SPEED_V_DOWN;

		} else {
			V2f16 jetman_pos = level->jetman->object.pos;
			spaceship->fuel_object.pos.x = jetman_pos.x;
			spaceship->fuel_object.pos.y = fix16Add(jetman_pos.y, FIX16_8); // the fuel sprite is 8px shorter than the jetman
		}

	} else if (spaceship->substep & FALLING) {

		if (overlap(level->jetman->object.box, spaceship->fuel_object.box)
				&& !(isAbove(spaceship->fuel_object.box, spaceship->base_object.box))) {
			// jetman just grabbed the fuel load
			spaceship->substep = GRABBED;
			spaceship->fuel_object.mov.y = SPEED_0;

		} else {
			Box_s16 target_v = targetVBox(spaceship->fuel_object, FUEL_WIDTH, FUEL_HEIGHT);
			if (overlap(target_v, spaceship->base_object.box)) {
				spaceship->step++;
				spaceship->substep = NONE;
				SPR_nextFrame(spaceship->base_sprite);
				if (spaceship->step == READY) {
					SPR_releaseSprite(spaceship->fuel_sprite);
					spaceship->fuel_sprite = 0;
				}

			} else if (landed(target_v, level)) {
				spaceship->substep = WAITING;
				spaceship->fuel_object.mov.y = SPEED_0;

			} else if (level->def.mind_bottom && target_v.pos.y > BOTTOM_POS_V_PX_S16) {
				// fuel lost
				spaceship->substep = NONE;
			} else {

				spaceship->fuel_object.pos.y += spaceship->fuel_object.mov.y;
			}
		}
	}

	updateBox(&spaceship->fuel_object);
	SPR_setPosition(spaceship->fuel_sprite, fix16ToInt(spaceship->fuel_object.pos.x),
			fix16ToInt(spaceship->fuel_object.pos.y));
}

static void handlePart(Object_f16* part, Sprite* sprite, u16 goal, fix16 v_offset_px, Level* level) {

	Spaceship* spaceship = level->spaceship;
	V2f16 jetman_pos = level->jetman->object.pos;

	if (isAbove(part->box, spaceship->base_object.box)) {
		if (spaceship->substep & GRABBED) {
			// release right over the base
			spaceship->substep = FALLING;
			part->pos.x = spaceship->base_object.pos.x;
		} else {
			// falling
			part->pos.y += SPEED_V_DOWN;
			fix16 v_limit = spaceship->base_object.pos.y - v_offset_px;
			if (part->pos.y >= v_limit) {
				part->pos.y = v_limit;
				spaceship->step = goal;
				spaceship->substep = WAITING;
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

static void mergeParts(Spaceship* spaceship) {

// release mid and top modules
	SPR_releaseSprite(spaceship->mid_sprite);
	spaceship->mid_sprite = 0;
	SPR_releaseSprite(spaceship->top_sprite);
	spaceship->top_sprite = 0;

// the base becomes a whole spaceship
	spaceship->base_object.pos.y -= FIX16_32;
	spaceship->base_object.size.y = 48;
	SPR_releaseSprite(spaceship->base_sprite);
	spaceship->base_sprite = SPR_addSprite(&u1_sprite, fix16ToInt(spaceship->base_object.pos.x),
			fix16ToInt(spaceship->base_object.pos.y), default_sprite_attrs);
}
