/*
 * spaceship.c
 *
 *  Created on: May 12, 2019
 *      Author: diegomtassis
 */

#include "../inc/spaceship.h"

#include <genesis.h>

#include "../inc/fwk/commons.h"
#include "../inc/fwk/physics.h"
#include "../inc/level.h"
#include "../inc/items.h"
#include "../inc/events.h"
#include "../res/sprite.h"

#define BASE	0x01
#define MID		0x02
#define TOP		0x04
#define WHOLE	0x08

#define NONE	0x01
#define WAITING		0x02
#define GRABBED		0x04
#define FALLING		0x08
#define ASSEMBLING	0x10
#define DONE		0x20

#define SPACESHIP_SPEED_V	FIX16(0.6)

static Object_f16* createModule(u8 module, V2s16 pos);

static void handleAssembly(Level level[static 1]);
static void handleFuelling(Level level[static 1]);
static void handlePart(Object_f16* part, Sprite* sprite, u16 goal, fix16 height, Level level[static 1]);
static void mergeParts(Spaceship* spaceship);
static void handleFlight(Spaceship* spaceship, Level level[static 1]);
static void handleLanding(Spaceship* spaceship, Level level[static 1]);

static bool leftPlanet(Spaceship* spaceship);

u16 default_sprite_attrs = TILE_ATTR(PAL0, TRUE, FALSE, FALSE);

void startSpaceship(Level level[static 1]) {

	Spaceship* spaceship = MEM_alloc(sizeof *spaceship);
	memset(spaceship, 0, sizeof *spaceship);

	level->spaceship = spaceship;

	if (UNASSEMBLED == level->def.spaceship_def.init_step) {

		spaceship->step = UNASSEMBLED;
		spaceship->substep = WAITING;

		spaceship->top_object = createModule(TOP, level->def.spaceship_def.top_pos);
		spaceship->top_sprite = SPR_addSprite(&u1_top_sprite, fix16ToInt(spaceship->top_object->pos.x),
				fix16ToInt(spaceship->top_object->pos.y), default_sprite_attrs);

		spaceship->mid_object = createModule(MID, level->def.spaceship_def.middle_pos);
		spaceship->mid_sprite = SPR_addSprite(&u1_middle_sprite, fix16ToInt(spaceship->mid_object->pos.x),
				fix16ToInt(spaceship->mid_object->pos.y), default_sprite_attrs);

		spaceship->base_object = createModule(BASE, level->def.spaceship_def.base_pos);
		spaceship->base_sprite = SPR_addSprite(&u1_base_sprite, fix16ToInt(spaceship->base_object->pos.x),
				fix16ToInt(spaceship->base_object->pos.y), default_sprite_attrs);

		spaceship->fire_sprite = 0;

	} else {
		// LANDING
		spaceship->step = LANDING;
		spaceship->substep = NONE;

		// override whatever comes as vertical starting position
		V2s16* base_pos = &level->def.spaceship_def.base_pos;
		base_pos->y = TOP_POS_V_PX_S16 - 48;

		spaceship->base_object = createModule(WHOLE, *base_pos);
		spaceship->base_object->mov.y = SPACESHIP_SPEED_V;

		spaceship->base_sprite = SPR_addSprite(&u1_sprite, base_pos->x, base_pos->y,
				TILE_ATTR(PAL0, FALSE, FALSE, FALSE));

		// fire
		spaceship->fire_sprite = SPR_addSprite(&fire_sprite, base_pos->x, base_pos->y + 52,
				TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
	}

	spaceship->fuel_sprite = 0;
}

void releaseSpaceship(Level level[static 1]) {

	if (!level->spaceship) {
		return;
	}

	if (level->spaceship->base_object) {
		MEM_free(level->spaceship->base_object);
		level->spaceship->base_object = 0;
	}

	if (level->spaceship->base_sprite) {
		SPR_releaseSprite(level->spaceship->base_sprite);
		level->spaceship->base_sprite = 0;
	}

	if (level->spaceship->mid_object) {
		MEM_free(level->spaceship->mid_object);
		level->spaceship->mid_object = 0;
	}

	if (level->spaceship->mid_sprite) {
		SPR_releaseSprite(level->spaceship->mid_sprite);
		level->spaceship->mid_sprite = 0;
	}

	if (level->spaceship->top_object) {
		MEM_free(level->spaceship->top_object);
		level->spaceship->top_object = 0;
	}

	if (level->spaceship->top_sprite) {
		SPR_releaseSprite(level->spaceship->top_sprite);
		level->spaceship->top_sprite = 0;
	}

	if (level->spaceship->fuel_object) {
		MEM_free(level->spaceship->fuel_object);
		level->spaceship->fuel_object = 0;
	}

	if (level->spaceship->fuel_sprite) {
		SPR_releaseSprite(level->spaceship->fuel_sprite);
		level->spaceship->fuel_sprite = 0;
	}

	if (level->spaceship->fire_sprite) {
		SPR_releaseSprite(level->spaceship->fire_sprite);
		level->spaceship->fire_sprite = 0;
	}

	MEM_free(level->spaceship);
	level->spaceship = 0;
}

void handleSpaceship(Level level[static 1]) {

	Spaceship* spaceship = level->spaceship;

	if (level->spaceship->step == LANDING) {
		handleLanding(level->spaceship, level);

	} else if (level->spaceship->step == LIFTING) {
		handleFlight(level->spaceship, level);

	} else if (spaceship->step >= UNASSEMBLED && spaceship->step < ASSEMBLED) {
		handleAssembly(level);

	} else if (spaceship->step >= ASSEMBLED && spaceship->step < READY) {
		handleFuelling(level);
	}
}

void dropIfGrabbed(Spaceship* spaceship) {

	if ((spaceship->substep & GRABBED) && (spaceship->step >= ASSEMBLED)) {
		spaceship->substep = FALLING;
		spaceship->fuel_object->mov.y = SPEED_V_DOWN;
	}
}

void launch(Spaceship* spaceship) {

	if (spaceship->step != READY) {
		return;
	}

	spaceship->step = LIFTING;
	spaceship->base_object->mov.y = -SPACESHIP_SPEED_V;
}

static Object_f16* createModule(u8 module, V2s16 pos) {

	Object_f16* object = MEM_alloc(sizeof *object);
	memset(object, 0, sizeof *object);

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

static void handleAssembly(Level level[static 1]) {

	Spaceship* spaceship = level->spaceship;

	if (spaceship->step == UNASSEMBLED) {
		handlePart(spaceship->mid_object, spaceship->mid_sprite, FUSELAGE_SET, FIX16_16, level);

	} else if (spaceship->step == FUSELAGE_SET) {
		handlePart(spaceship->top_object, spaceship->top_sprite, ASSEMBLED, FIX16_32, level);

		if (spaceship->step == ASSEMBLED) {
			// Change to the complete sprite when assembled
			mergeParts(spaceship);
			spaceship->substep = NONE;
		}
	}
}

static void handleFuelling(Level level[static 1]) {

	Spaceship* spaceship = level->spaceship;

	if (spaceship->step < ASSEMBLED || spaceship->step == READY) {
		return;
	}

	if (spaceship->substep & NONE) {

		if (!spaceship->fuel_object) {
			// first time, initialize a new fuel load
			spaceship->fuel_object = MEM_alloc(sizeof *spaceship->fuel_object);
			memset(spaceship->fuel_object, 0, sizeof *spaceship->fuel_object);
		}
		dropFromSky(spaceship->fuel_object, &spaceship->base_object->box);
		if (isAbove(spaceship->fuel_object->box, spaceship->base_object->box)) {
			spaceship->substep = ASSEMBLING;
		} else {
			spaceship->substep = FALLING;
		}

		if (!spaceship->fuel_sprite) {
			spaceship->fuel_sprite = SPR_addSprite(&fuel_sprite, fix16ToInt(spaceship->fuel_object->pos.x),
					fix16ToInt(spaceship->fuel_object->pos.y), default_sprite_attrs);
		}

	} else if (spaceship->substep & WAITING) {

		if (grab(&level->jetman->object, spaceship->fuel_object)) {
			// fuel grabbed while waiting
			spaceship->substep = GRABBED;
			onEvent(GRABBED_FUEL);
		}

	} else if (spaceship->substep & GRABBED) {

		// fuel in jetman possession
		if (isAbove(spaceship->fuel_object->box, spaceship->base_object->box)) {
			spaceship->fuel_object->pos.x = spaceship->base_object->pos.x;
			spaceship->substep = ASSEMBLING;
			spaceship->fuel_object->mov.y = SPEED_V_DOWN;

		} else {
			V2f16 jetman_pos = level->jetman->object.pos;
			spaceship->fuel_object->pos.x = jetman_pos.x;
			spaceship->fuel_object->pos.y = fix16Add(jetman_pos.y, FIX16_8); // the fuel sprite is 8px shorter than the jetman
		}

	} else if (spaceship->substep & FALLING) {

		if (grab(&level->jetman->object, spaceship->fuel_object)) {
			// fuel grabbed while falling
			spaceship->substep = GRABBED;

		} else {
			Box_s16 target_v = targetVBox(*spaceship->fuel_object, ITEM_WIDTH, ITEM_HEIGHT);
			if (landed(target_v, level)) {
				spaceship->substep = WAITING;
				spaceship->fuel_object->mov.y = SPEED_0;

			} else if (level->def.mind_bottom && target_v.pos.y > BOTTOM_POS_V_PX_S16) {
				// fuel lost
				spaceship->substep = NONE;

			} else {
				spaceship->fuel_object->pos.y += spaceship->fuel_object->mov.y;
			}
		}

	} else if (spaceship->substep & ASSEMBLING) {

		Box_s16 target_v = targetVBox(*spaceship->fuel_object, ITEM_WIDTH, ITEM_HEIGHT);
		if (overlap(target_v, spaceship->base_object->box)) {
			spaceship->step++;
			spaceship->substep = NONE;
			SPR_nextFrame(spaceship->base_sprite);
			if (spaceship->step == READY) {
				MEM_free(spaceship->fuel_object);
				spaceship->fuel_object = 0;
				SPR_releaseSprite(spaceship->fuel_sprite);
				spaceship->fuel_sprite = 0;
			}
		} else {
			spaceship->fuel_object->pos.y += spaceship->fuel_object->mov.y;
		}
	}

	updateBox(spaceship->fuel_object);
	SPR_setPosition(spaceship->fuel_sprite, fix16ToInt(spaceship->fuel_object->pos.x),
			fix16ToInt(spaceship->fuel_object->pos.y));
}

static void handlePart(Object_f16* part, Sprite* sprite, u16 goal, fix16 v_offset_px, Level level[static 1]) {

	Spaceship* spaceship = level->spaceship;
	V2f16 jetman_pos = level->jetman->object.pos;

	if (isAbove(part->box, spaceship->base_object->box)) {
		if (spaceship->substep & GRABBED) {
			// release right over the base
			spaceship->substep = ASSEMBLING;
			part->pos.x = spaceship->base_object->pos.x;
		} else {
			// falling
			part->pos.y += SPEED_V_DOWN;
			fix16 v_limit = spaceship->base_object->pos.y - v_offset_px;
			if (part->pos.y >= v_limit) {
				part->pos.y = v_limit;
				spaceship->step = goal;
				spaceship->substep = WAITING;
			}
		}

	} else if ((spaceship->substep & GRABBED) || overlap(level->jetman->object.box, part->box)) {

		// The jetman is already in possession or has just grabbed the a section of the rocket
		if (!(spaceship->substep & GRABBED)) {
			spaceship->substep = GRABBED;
			onEvent(GRABBED_SPACESHIP_PART);
		}

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
	spaceship->base_object->pos.y -= FIX16_32;
	spaceship->base_object->size.y = 48;
	SPR_releaseSprite(spaceship->base_sprite);
	spaceship->base_sprite = SPR_addSprite(&u1_sprite, fix16ToInt(spaceship->base_object->pos.x),
			fix16ToInt(spaceship->base_object->pos.y), TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
}

static void handleLanding(Spaceship* spaceship, Level level[static 1]) {

	if (spaceship->step != LANDING) {
		return;
	}

	// spaceship
	Box_s16 target_v = targetVBox(*spaceship->base_object, spaceship->base_object->box.w,
			spaceship->base_object->box.h);

	f16 landed_y = landed(target_v, level);
	if (landed_y) {
		spaceship->step = ASSEMBLED;
		spaceship->base_object->pos.y = landed_y;

	} else {
		spaceship->base_object->pos.y += spaceship->base_object->mov.y;
	}

	updateBox(spaceship->base_object);

	u16 h_pos_u16 = fix16ToInt(spaceship->base_object->pos.x);
	u16 v_pos_u16 = fix16ToInt(spaceship->base_object->pos.y);
	SPR_setPosition(spaceship->base_sprite, h_pos_u16, v_pos_u16);

	// fire
	if (spaceship->fire_sprite) {
		u16 v_fire_u16 = v_pos_u16 + 52;
		if (v_fire_u16 + 12 >= level->floor->object.box.pos.y) {
			SPR_releaseSprite(spaceship->fire_sprite);
			spaceship->fire_sprite = 0;

		} else {
			SPR_setPosition(spaceship->fire_sprite, h_pos_u16, v_fire_u16);
		}
	}
}

static void handleFlight(Spaceship* spaceship, Level level[static 1]) {

	if (spaceship->step == LIFTING) {

		if (leftPlanet(spaceship)) {
			spaceship->step = ORBITING;
			spaceship->substep = NONE;

		} else {
			// spaceship
			spaceship->base_object->pos.y += spaceship->base_object->mov.y;
			updateBox(spaceship->base_object);
			u16 h_pos_u16 = fix16ToInt(spaceship->base_object->pos.x);
			u16 v_pos_u16 = fix16ToInt(spaceship->base_object->pos.y);
			SPR_setPosition(spaceship->base_sprite, h_pos_u16, v_pos_u16);

			// fire
			u16 v_fire_u16 = v_pos_u16 + 52;
			if (!spaceship->fire_sprite) {
				if (v_fire_u16 + 12 < level->floor->object.box.pos.y) { // leave room for the fire
					spaceship->fire_sprite = SPR_addSprite(&fire_sprite, h_pos_u16, v_fire_u16,
							TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
				}
			} else {
				SPR_setPosition(spaceship->fire_sprite, h_pos_u16, v_fire_u16);
			}
		}
	}
}

static bool leftPlanet(Spaceship* spaceship) {

	return spaceship->base_object->pos.y < TOP_POS_V_PX_F16 - FIX16(48);
}
