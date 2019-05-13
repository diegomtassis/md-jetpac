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

static Object_f16* createModule(u8 module, Vect2D_u16 pos);

void startSpaceship(Level* level) {

	Spaceship* spaceship = MEM_alloc(sizeof(Spaceship));
	level->spaceship = spaceship;

	if (UNASSEMBLED & level->def.spaceshipDef.type) {

		spaceship->objectTop = *createModule(TOP, level->def.spaceshipDef.topPos);
		spaceship->spriteTop = SPR_addSprite(&u1_top_sprite, fix16ToInt(spaceship->objectTop.pos.x),
				fix16ToInt(spaceship->objectTop.pos.y), TILE_ATTR(PAL0, TRUE, FALSE, FALSE));

		spaceship->objectMid = *createModule(MID, level->def.spaceshipDef.middlePos);
		spaceship->spriteMid = SPR_addSprite(&u1_middle_sprite, fix16ToInt(spaceship->objectMid.pos.x),
				fix16ToInt(spaceship->objectMid.pos.y), TILE_ATTR(PAL0, TRUE, FALSE, FALSE));

		spaceship->objectBase = *createModule(BASE, level->def.spaceshipDef.basePos);
		spaceship->spriteBase = SPR_addSprite(&u1_base_sprite, fix16ToInt(spaceship->objectBase.pos.x),
				fix16ToInt(spaceship->objectBase.pos.y), TILE_ATTR(PAL0, TRUE, FALSE, FALSE));

	} else {
		// ASSEMBLED
		spaceship->objectBase = *createModule(WHOLE, level->def.spaceshipDef.basePos);
	}
}

void releaseSpaceship(Level* level) {

	if (level->spaceship->spriteBase) {
		SPR_releaseSprite(level->spaceship->spriteBase);
		level->spaceship->spriteBase = 0;
	}

	if (level->spaceship->spriteMid) {
		SPR_releaseSprite(level->spaceship->spriteMid);
		level->spaceship->spriteMid = 0;
	}

	if (level->spaceship->spriteTop) {
		SPR_releaseSprite(level->spaceship->spriteTop);
		level->spaceship->spriteTop = 0;
	}

	MEM_free(level->spaceship);
	level->spaceship = 0;
}

static Object_f16* createModule(u8 module, Vect2D_u16 pos) {

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

