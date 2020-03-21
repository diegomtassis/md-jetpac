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
#include "../inc/planet.h"
#include "../inc/items.h"
#include "../inc/events.h"
#include "../res/sprite.h"
#include "../inc/players.h"
#include "../inc/jetman.h"

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

#define FUEL_WIDTH 16
#define FUEL_HEIGHT 11

const SpaceshipTypeDefinition u1Definition = { //
		.sprite_def = &u1_sprite, //
				.base_sprite_def = &u1_base_sprite, //
				.middle_sprite_def = &u1_middle_sprite, //
				.top_sprite_def = &u1_top_sprite };

const SpaceshipTypeDefinition u2Definition = { //
		.sprite_def = &u2_sprite, //
				.base_sprite_def = &u2_base_sprite, //
				.middle_sprite_def = &u2_middle_sprite, //
				.top_sprite_def = &u2_top_sprite };

const SpaceshipTypeDefinition u3Definition = { //
		.sprite_def = &u3_sprite, //
				.base_sprite_def = &u3_base_sprite, //
				.middle_sprite_def = &u3_middle_sprite, //
				.top_sprite_def = &u3_top_sprite };

const SpaceshipTypeDefinition u4Definition = { //
		.sprite_def = &u4_sprite, //
				.base_sprite_def = &u4_base_sprite, //
				.middle_sprite_def = &u4_middle_sprite, //
				.top_sprite_def = &u4_top_sprite };

bool spaceship_ready;

static Object_f16* createModule(u8 module, V2s16 pos);

static void handleAssembly(Planet planet[static 1]);
static void handleFuelling(Planet planet[static 1]);
static void handlePart(Object_f16* part, Sprite* sprite, u16 goal, fix16 height, Planet planet[static 1]);
static void mergeParts(Spaceship* spaceship);
static void handleFlight(Spaceship* spaceship, Planet planet[static 1]);
static void handleLanding(Spaceship* spaceship, Planet planet[static 1]);

static bool leftPlanet(Spaceship* spaceship);

u16 default_sprite_attrs = TILE_ATTR(PAL0, TRUE, FALSE, FALSE);

void startSpaceship(Planet planet[static 1]) {

	Spaceship* spaceship = MEM_calloc(sizeof *spaceship);
	planet->spaceship = spaceship;
	spaceship_ready = FALSE;

	SpaceshipDefinition spaceship_definition = planet->def.spaceship_def;
	spaceship->definition = spaceship_definition;

	if (UNASSEMBLED == spaceship_definition.init_step) {

		spaceship->step = UNASSEMBLED;
		spaceship->substep = WAITING;
		spaceship->grabbedBy = 0;

		spaceship->top_object = createModule(TOP, spaceship_definition.top_pos);
		spaceship->top_sprite = SPR_addSprite(spaceship_definition.type_definition.top_sprite_def,
				fix16ToInt(spaceship->top_object->pos.x), fix16ToInt(spaceship->top_object->pos.y),
				default_sprite_attrs);

		spaceship->mid_object = createModule(MID, spaceship_definition.middle_pos);
		spaceship->mid_sprite = SPR_addSprite(spaceship_definition.type_definition.middle_sprite_def,
				fix16ToInt(spaceship->mid_object->pos.x), fix16ToInt(spaceship->mid_object->pos.y),
				default_sprite_attrs);

		spaceship->base_object = createModule(BASE, spaceship_definition.base_pos);
		spaceship->base_sprite = SPR_addSprite(spaceship_definition.type_definition.base_sprite_def,
				fix16ToInt(spaceship->base_object->pos.x), fix16ToInt(spaceship->base_object->pos.y),
				default_sprite_attrs);

		spaceship->fire_sprite = 0;

	} else {
		// LANDING
		spaceship->step = LANDING;
		spaceship->substep = NONE;
		spaceship->grabbedBy = 0;

		// override whatever comes as vertical starting position
		V2s16* base_pos = &planet->def.spaceship_def.base_pos;
		base_pos->y = TOP_POS_V_PX_S16 - 48;

		spaceship->base_object = createModule(WHOLE, *base_pos);
		spaceship->base_object->mov.y = SPACESHIP_SPEED_V;

		spaceship->base_sprite = SPR_addSprite(spaceship_definition.type_definition.sprite_def, base_pos->x,
				base_pos->y, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));

		// fire
		spaceship->fire_sprite = SPR_addSprite(&fire_sprite, base_pos->x, base_pos->y + 52,
				TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
	}

	spaceship->fuel_sprite = 0;
}

void releaseSpaceship(Planet planet[static 1]) {

	if (!planet->spaceship) {
		return;
	}

	if (planet->spaceship->base_object) {
		MEM_free(planet->spaceship->base_object);
		planet->spaceship->base_object = 0;
	}

	if (planet->spaceship->base_sprite) {
		SPR_releaseSprite(planet->spaceship->base_sprite);
		planet->spaceship->base_sprite = 0;
	}

	if (planet->spaceship->mid_object) {
		MEM_free(planet->spaceship->mid_object);
		planet->spaceship->mid_object = 0;
	}

	if (planet->spaceship->mid_sprite) {
		SPR_releaseSprite(planet->spaceship->mid_sprite);
		planet->spaceship->mid_sprite = 0;
	}

	if (planet->spaceship->top_object) {
		MEM_free(planet->spaceship->top_object);
		planet->spaceship->top_object = 0;
	}

	if (planet->spaceship->top_sprite) {
		SPR_releaseSprite(planet->spaceship->top_sprite);
		planet->spaceship->top_sprite = 0;
	}

	if (planet->spaceship->fuel_object) {
		MEM_free(planet->spaceship->fuel_object);
		planet->spaceship->fuel_object = 0;
	}

	if (planet->spaceship->fuel_sprite) {
		SPR_releaseSprite(planet->spaceship->fuel_sprite);
		planet->spaceship->fuel_sprite = 0;
	}

	if (planet->spaceship->fire_sprite) {
		SPR_releaseSprite(planet->spaceship->fire_sprite);
		planet->spaceship->fire_sprite = 0;
	}

	MEM_free(planet->spaceship);
	planet->spaceship = 0;
}

void handleSpaceship(Planet planet[static 1]) {
	/*
	 * Steps:
	 *
	 * LANDING
	 * UNASSEMBLED
	 * FUSELAGE_SET
	 * ASSEMBLED
	 * FUEL_1
	 * FUEL_2
	 * FUEL_3
	 * FUEL_4
	 * FUEL_5
	 * READY
	 * LIFTING
	 * ORBITING
	 */
	Spaceship* spaceship = planet->spaceship;

	if (planet->spaceship->step == LANDING) {
		handleLanding(planet->spaceship, planet);

	} else if (planet->spaceship->step == LIFTING) {
		handleFlight(planet->spaceship, planet);

	} else if (spaceship->step >= UNASSEMBLED && spaceship->step < ASSEMBLED) {
		handleAssembly(planet);

	} else if (spaceship->step >= ASSEMBLED && spaceship->step < READY) {
		handleFuelling(planet);
	}
}

void dropIfGrabbed(Jetman* jetman, Spaceship* spaceship) {

	if (!jetman || spaceship->grabbedBy != jetman) {
		return;
	}

	if (spaceship->substep & GRABBED) {

		if (spaceship->step >= ASSEMBLED) {
			// it's fuel
			spaceship->substep = FALLING;
			spaceship->grabbedBy = 0;
			spaceship->fuel_object->mov.y = SPEED_V_DOWN;

		} else if (spaceship->step == UNASSEMBLED) {
			// jetman had grabbed the fuselage, move it to its initial position
			spaceship->mid_object->pos.x = FIX16(spaceship->definition.middle_pos.x);
			spaceship->mid_object->pos.y = FIX16(spaceship->definition.middle_pos.y);
			updateBox(spaceship->mid_object);
			SPR_setPosition(spaceship->mid_sprite, spaceship->definition.middle_pos.x,
					spaceship->definition.middle_pos.y);

			spaceship->substep = WAITING;

		} else if (spaceship->step == FUSELAGE_SET) {
			// jetman had grabbed the top, move it its initial position
			spaceship->top_object->pos.x = FIX16(spaceship->definition.top_pos.x);
			spaceship->top_object->pos.y = FIX16(spaceship->definition.top_pos.y);
			updateBox(spaceship->top_object);
			SPR_setPosition(spaceship->top_sprite, spaceship->definition.top_pos.x, spaceship->definition.top_pos.y);

			spaceship->substep = WAITING;
		}
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

	Object_f16* object = MEM_calloc(sizeof *object);

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

static void handleAssembly(Planet planet[static 1]) {
	/*
	 * Steps:
	 *
	 * UNASSEMBLED
	 * FUSELAGE_SET
	 * ASSEMBLED
	 *
	 * Substeps:
	 *
	 * NONE
	 * WAITING
	 * GRABBED
	 * FALLING
	 * ASSEMBLING
	 * DONE
	 */
	Spaceship* spaceship = planet->spaceship;

	if (spaceship->step == UNASSEMBLED) {
		handlePart(spaceship->mid_object, spaceship->mid_sprite, FUSELAGE_SET, FIX16_16, planet);

	} else if (spaceship->step == FUSELAGE_SET) {
		handlePart(spaceship->top_object, spaceship->top_sprite, ASSEMBLED, FIX16_32, planet);

		if (spaceship->step == ASSEMBLED) {
			// Change to the complete sprite when assembled
			mergeParts(spaceship);
			spaceship->substep = NONE;
		}
	}
}

static void handleFuelling(Planet planet[static 1]) {
	/*
	 * Steps:
	 *
	 * FUEL_1
	 * FUEL_2
	 * FUEL_3
	 * FUEL_4
	 * FUEL_5
	 * READY
	 *
	 * Substeps:
	 *
	 * NONE
	 * WAITING
	 * GRABBED
	 * FALLING
	 * ASSEMBLING
	 * DONE
	 */
	Spaceship* spaceship = planet->spaceship;

	if (spaceship->step < ASSEMBLED || spaceship->step == READY) {
		return;
	}

	if (spaceship->substep & NONE) {

		if (!spaceship->fuel_object) {
			// first time, initialize a new fuel load
			spaceship->fuel_object = MEM_calloc(sizeof *spaceship->fuel_object);
		}

		dropFromSky(spaceship->fuel_object, &spaceship->base_object->box, FUEL_WIDTH, FUEL_HEIGHT,
		ITEM_DEFAULT_MIN_POS_H_PX_F16, ITEM_DEFAULT_MAX_POS_H_PX_F16);
		spaceship->substep = FALLING;

		if (!spaceship->fuel_sprite) {
			spaceship->fuel_sprite = SPR_addSprite(&fuel_sprite, fix16ToInt(spaceship->fuel_object->pos.x),
					fix16ToInt(spaceship->fuel_object->pos.y), default_sprite_attrs);
		}

	} else if (spaceship->substep & WAITING) {

		if (j1 && grab(&j1->object, spaceship->fuel_object)) {
			// fuel grabbed by j1 while waiting
			spaceship->substep = GRABBED;
			spaceship->grabbedBy = j1;
			onEvent(GRABBED_FUEL, P1);

		} else if (j2 && grab(&j2->object, spaceship->fuel_object)) {
			// fuel grabbed by j2 while waiting
			spaceship->substep = GRABBED;
			spaceship->grabbedBy = j2;
			onEvent(GRABBED_FUEL, P2);
		}

	} else if (spaceship->substep & GRABBED) {

		// fuel in jetman possession
		if (isAboveBaseUpwardProjection(&spaceship->fuel_object->box, &spaceship->base_object->box)) {
			spaceship->fuel_object->pos.x = spaceship->base_object->pos.x;
			spaceship->substep = ASSEMBLING;
			spaceship->grabbedBy = 0;
			spaceship->fuel_object->mov.y = SPEED_V_DOWN;

		} else {
			V2f16 jetman_pos = spaceship->grabbedBy->object.pos;
			spaceship->fuel_object->pos.x = jetman_pos.x;
			spaceship->fuel_object->pos.y = fix16Add(jetman_pos.y, FIX16_12);
		}

	} else if (spaceship->substep & FALLING) {

		if (j1 && grab(&j1->object, spaceship->fuel_object)) {
			// fuel grabbed by j1 while falling
			spaceship->substep = GRABBED;
			spaceship->grabbedBy = j1;
			onEvent(GRABBED_FUEL, P1);

		} else if (j2 && grab(&j2->object, spaceship->fuel_object)) {
			// fuel grabbed by j2 while falling
			spaceship->substep = GRABBED;
			spaceship->grabbedBy = j2;
			onEvent(GRABBED_FUEL, P2);

		} else {
			Box_s16 target_v = targetVBox(spaceship->fuel_object);
			if (landed(target_v, planet)) {
				spaceship->substep = WAITING;
				spaceship->fuel_object->mov.y = SPEED_0;

			} else if (planet->def.mind_bottom && target_v.min.y > BOTTOM_POS_V_PX_S16) {
				// fuel lost
				spaceship->substep = NONE;
				onEvent(LOST_FUEL, 0);

			} else {
				spaceship->fuel_object->pos.y += spaceship->fuel_object->mov.y;
			}
		}

	} else if (spaceship->substep & ASSEMBLING) {

		Box_s16 target_v = targetVBox(spaceship->fuel_object);
		if (overlap(&target_v, &spaceship->base_object->box)) {

			spaceship->step++;
			spaceship->substep = NONE;
			SPR_nextFrame(spaceship->base_sprite);

			if (spaceship->step == READY) { // fuelling just completed
				spaceship_ready = TRUE;
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

static void handlePart(Object_f16* part, Sprite* sprite, u16 goal, fix16 v_offset_px, Planet planet[static 1]) {

	Spaceship* spaceship = planet->spaceship;

	V2f16 jetman_pos;

	if (spaceship->substep & GRABBED) {

		if (isAboveBaseUpwardProjection(&part->box, &spaceship->base_object->box)) {
			// release right over the base
			spaceship->substep = ASSEMBLING;
			spaceship->grabbedBy = 0;
			part->pos.x = spaceship->base_object->pos.x;

		} else {
			jetman_pos = spaceship->grabbedBy->object.pos;
			part->pos.x = jetman_pos.x;
			part->pos.y = fix16Add(jetman_pos.y, FIX16_8); // the rocket part is 8px shorter than the jetman
		}

	} else if (spaceship->substep & WAITING) {
		// has anyone just grabbed it?
		Jetman* jetman = 0;
		if (j1 && overlap(&j1->object.box, &part->box)) {
			jetman = j1;

		} else if (j2 && overlap(&j2->object.box, &part->box)) {
			jetman = j2;
		}

		if (jetman) { // someone just grabbed it

			spaceship->substep = GRABBED;
			spaceship->grabbedBy = jetman;
			onEvent(GRABBED_SPACESHIP_PART, jetman->id);
		}

	} else if (spaceship->substep & ASSEMBLING) {
		// falling
		part->pos.y += SPEED_V_DOWN;
		fix16 v_limit = spaceship->base_object->pos.y - v_offset_px;
		if (part->pos.y >= v_limit) {
			part->pos.y = v_limit;
			spaceship->step = goal;
			spaceship->substep = WAITING;
		}
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
	spaceship->base_sprite = SPR_addSprite(spaceship->definition.type_definition.sprite_def,
			fix16ToInt(spaceship->base_object->pos.x), fix16ToInt(spaceship->base_object->pos.y),
			TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
}

static void handleLanding(Spaceship* spaceship, Planet planet[static 1]) {

	if (spaceship->step != LANDING) {
		return;
	}

	// spaceship
	Box_s16 target_v = targetVBox(spaceship->base_object);

	f16 landed_y = landed(target_v, planet);
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
		if (v_fire_u16 + 12 >= planet->floor->object.box.min.y) {
			SPR_releaseSprite(spaceship->fire_sprite);
			spaceship->fire_sprite = 0;

		} else {
			SPR_setPosition(spaceship->fire_sprite, h_pos_u16, v_fire_u16);
		}
	}
}

static void handleFlight(Spaceship* spaceship, Planet planet[static 1]) {

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
				if (v_fire_u16 + 12 < planet->floor->object.box.min.y) { // leave room for the fire
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
