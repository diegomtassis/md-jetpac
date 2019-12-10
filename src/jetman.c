/*
 * jetman.c
 *
 *  Created on: Apr 16, 2019
 *      Author: diegomtassis
 */

#include "../inc/jetman.h"

#include <genesis.h>

#include "../inc/players.h"
#include "../inc/constants.h"
#include "../inc/explosions.h"
#include "../inc/fwk/commons.h"
#include "../inc/fwk/physics.h"
#include "../inc/planet.h"
#include "../inc/shooting.h"
#include "../res/sprite.h"

#define ANIM_WALK		0
#define ANIM_FLY		1
#define STEPPING_SPEED	6 // 0 Maximum

#define SPEED_ZERO			FIX16_0
#define SPEED_H_WALK		FIX16(1)
#define SPEED_H_FLY			FIX16(1.5)
#define SPEED_V_UP			FIX16(-1.5)
#define SPEED_V_DOWN_MAX	FIX16(1.5)
#define GRAVITY         	FIX16(0.2)
#define SPEED_V_DOWN_MAX_MINUS_GRAVITY	SPEED_V_DOWN_MAX - GRAVITY

#define UP			0x01
#define DOWN		0x02
#define RIGHT		0x04
#define LEFT		0x10
#define BOOST		0x20

#define JETMAN_HEIGHT 24
#define JETMAN_WIDTH 16

static Jetman* createJetman(u8 id, PlayerStatus* status);
static void releaseJetman(Jetman jetman[static 1]);
static void handleInputJetman(Jetman*, u16 joy);

static void moveToStart(Jetman* jetman, V2f16 init_pos);
static void shapePlayer(Jetman player[static 1], const SpriteDefinition* sprite, u16 ammo);
static V2f16 figureOutInitPosition(const Planet planet[static 1], u8 player);
static void moveJetman(Jetman*, Planet*);
static u8 calculateNextMovement(Jetman*);
static void updatePosition(Jetman*, Planet*);
static f16 reachedTop(Box_s16, const Planet*);
static f16 blockedByLeft(Box_s16, const Planet*);
static f16 blockedByRight(Box_s16, const Planet*);

static void drawJetman(Jetman*);

bool p1_shoot_pushed;
bool p1_shoot_order;

bool p2_shoot_pushed;
bool p2_shoot_order;

void startPlayers(Planet planet[static 1], PlayerStatus* p1_status, PlayerStatus* p2_status) {

	Jetman* player = createJetman(P1, p1_status);
	moveToStart(player, figureOutInitPosition(planet, P1));
	shapePlayer(player, &carl_sprite, planet->def.ammo);
	planet->p1 = player;

	if (p2_status && p2_status->lives > 0) {

		player = createJetman(P2, p2_status);
		moveToStart(player, figureOutInitPosition(planet, P2));
		shapePlayer(player, &ann_sprite, planet->def.ammo);
		player->invincible = TRUE; // Only for the moment
		planet->p2 = player;
	}
}

void releasePlayers(Planet planet[static 1]) {

	if (planet->p1) {
		releaseJetman(planet->p1);
		planet->p1 = 0;
	}

	if (planet->p2) {
		releaseJetman(planet->p2);
		planet->p2 = 0;
	}
}

void resetPlayer(Jetman* player, Planet planet[static 1]) {

	if (!player) {
		return;
	}

	moveToStart(player, figureOutInitPosition(planet, player->id));
	player->health = ALIVE;
	return;
}

void killPlayer(Jetman* player, Planet planet[static 1], bool exploding) {

	if (player->invincible) {
		return;
	}

	if (exploding) {
		explode(player->object.box, planet);
	}

	player->health = DEAD;
}

void playersAct(Planet planet[static 1]) {

	Jetman* player = planet->p1;
	if (player->health & ALIVE) {
		handleInputJetman(player, JOY_1);
		moveJetman(player, planet);
		drawJetman(player);

		if (p1_shoot_order && (player->ammo || !planet->game->config->limited_ammo)) {

			V2s16 where = { 0 };
			where.x = player->object.box.pos.x + (player->head_back ? 0 : 16);
			where.y = player->object.box.pos.y + 11;

			shoot(P1, where, planet->p1->head_back, planet);
			player->ammo--;
			p1_shoot_order = FALSE;
		}
	}

	player = planet->p2;
	if (player && (player->health & ALIVE)) {

		handleInputJetman(player, JOY_2);
		moveJetman(player, planet);
		drawJetman(player);

		if (p1_shoot_order && (player->ammo || !planet->game->config->limited_ammo)) {

			V2s16 where = { 0 };
			where.x = player->object.box.pos.x + (player->head_back ? 0 : 16);
			where.y = player->object.box.pos.y + 11;

			shoot(P2, where, planet->p1->head_back, planet);
			player->ammo--;
			p1_shoot_order = FALSE;
		}
	}
}

static Jetman* createJetman(u8 id, PlayerStatus* status) {

	Jetman* jetman = MEM_calloc(sizeof *jetman);

	jetman->id = id;

	jetman->object.size.x = JETMAN_WIDTH;
	jetman->object.size.y = JETMAN_HEIGHT;

	jetman->object.box.w = JETMAN_WIDTH;
	jetman->object.box.h = JETMAN_HEIGHT;

	jetman->health = ALIVE;

	jetman->status = status;

	return jetman;
}

static void releaseJetman(Jetman jetman[static 1]) {

	if (!jetman) {
		return;
	}

	SPR_setVisibility(jetman->sprite, FALSE);
	SPR_releaseSprite(jetman->sprite);
	jetman->sprite = 0;
	jetman->status = 0;
	MEM_free(jetman);
}

static void moveToStart(Jetman* jetman, V2f16 init_pos) {

	jetman->object.pos.x = init_pos.x;
	jetman->object.pos.y = init_pos.y;

	jetman->object.mov.x = SPEED_ZERO;
	jetman->object.mov.y = SPEED_ZERO;

	updateBox(&jetman->object);
}

static void shapePlayer(Jetman player[static 1], const SpriteDefinition* sprite, u16 ammo) {

	player->sprite = SPR_addSprite(sprite, fix16ToInt(player->object.pos.x), fix16ToInt(player->object.pos.y),
			TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
	p1_shoot_pushed = FALSE;
	p1_shoot_order = FALSE;
	player->ammo = ammo;
}

static V2f16 figureOutInitPosition(const Planet planet[static 1], u8 player) {

	V2f16 init_pos;

	if (player == P1) {

		if (planet->def.p1_init_pos) {

			init_pos.x = FIX16(planet->def.p1_init_pos->x);
			init_pos.y = FIX16(planet->def.p1_init_pos->y);

		} else {
			init_pos.x = FIX16(124);
			init_pos.y = fix16Sub(planet->floor->object.pos.y, FIX16(8 * 3));
		}

	} else {

		init_pos.x = FIX16(80);
		init_pos.y = fix16Sub(planet->floor->object.pos.y, FIX16(8 * 3));
	}

	return init_pos;
}

static void moveJetman(Jetman* jetman, Planet planet[static 1]) {

	if (BOOST & calculateNextMovement(jetman)) {
		boost(jetman->object.box, planet);
	}
	updatePosition(jetman, planet);
	SPR_setPosition(jetman->sprite, fix16ToInt(jetman->object.pos.x), fix16ToInt(jetman->object.pos.y));
}

static u8 calculateNextMovement(Jetman* jetman) {

	u8 movement = 0;

// horizontal movement
	if (jetman->order.x > 0) {
		jetman->object.mov.x = jetman->airborne ? SPEED_H_FLY : SPEED_H_WALK;
		jetman->head_back = FALSE;
		movement |= RIGHT;

	} else if (jetman->order.x < 0) {
		jetman->object.mov.x = jetman->airborne ? -SPEED_H_FLY : -SPEED_H_WALK;
		jetman->head_back = TRUE;
		movement |= LEFT;

	} else {
		jetman->object.mov.x = SPEED_ZERO;
	}

// vertical movement
	if (jetman->order.y < 0) {
		if (!jetman->airborne) {
			movement |= BOOST;
		}

		jetman->object.mov.y = SPEED_V_UP;
		movement |= UP;

	} else {
		/*
		 * either falling or walking. But at this point it's not known yet whether he's walking,
		 * so by default he's falling.
		 */
		if (jetman->object.mov.y <= SPEED_V_DOWN_MAX_MINUS_GRAVITY) {
			jetman->object.mov.y += GRAVITY;
		}
		movement |= DOWN;
	}

	return movement;
}

static void updatePosition(Jetman* jetman, Planet planet[static 1]) {

// horizontal position
	Box_s16 target_h = targetHBox(jetman->object);
	if (target_h.pos.x > MAX_POS_H_PX_S16) {
		jetman->object.pos.x = MIN_POS_H_PX_F16;

	} else if (target_h.pos.x < MIN_POS_H_PX_S16) {
		jetman->object.pos.x = MAX_POS_H_PX_F16;

	} else {

		f16 blockedHorizontally = blockedByLeft(target_h, planet);
		if (!blockedHorizontally) {
			blockedHorizontally = blockedByRight(target_h, planet);
		}

		if (blockedHorizontally) {
			jetman->object.pos.x = blockedHorizontally;

		} else {
			jetman->object.pos.x += jetman->object.mov.x;
		}
	}

// vertical position
	Box_s16 target_v = targetVBox(jetman->object);
	f16 landed_pos_y = landed(target_v, planet);
	jetman->airborne = !landed_pos_y;

	if (landed_pos_y) {
		jetman->object.pos.y = landed_pos_y;
		jetman->object.mov.y = SPEED_ZERO;

	} else {
		f16 top_pos_y = reachedTop(target_v, planet);
		if (top_pos_y) {
			jetman->object.pos.y = top_pos_y;
			if (jetman->object.mov.y < 0) {
				jetman->object.mov.y = SPEED_ZERO;
			}

		} else {
			jetman->object.pos.y += jetman->object.mov.y;
		}
	}

// update box
	updateBox(&jetman->object);
}

static f16 reachedTop(Box_s16 subject_box, const Planet planet[static 1]) {

	if (subject_box.pos.y <= MIN_POS_V_PX_S16) {
		return MIN_POS_V_PX_F16;
	}

	for (u8 i = 0; i < planet->num_platforms; i++) {
		Box_s16 object_box = planet->platforms[i]->object.box;
		if (hitUnder(subject_box, object_box)) {
			return FIX16(adjacentYUnder(subject_box, object_box));
		}
	}

	if (hitUnder(subject_box, planet->floor->object.box)) {
		return FIX16(adjacentYUnder(subject_box, planet->floor->object.box));
	}

	return FIX16_0;
}

static f16 blockedByLeft(Box_s16 target_box, const Planet planet[static 1]) {

	if (hitLeft(target_box, planet->floor->object.box)) {
		return FIX16(adjacentXOnTheLeft(target_box, planet->floor->object.box));
	}

	for (u8 i = 0; i < planet->num_platforms; i++) {
		Box_s16 object_box = planet->platforms[i]->object.box;
		if (hitLeft(target_box, object_box)) {
			return FIX16(adjacentXOnTheLeft(target_box, object_box));
		}
	}

	return FIX16_0;
}

static f16 blockedByRight(Box_s16 target_box, const Planet planet[static 1]) {

	if (hitRight(target_box, planet->floor->object.box)) {
		return FIX16(adjacentXOnTheRight(target_box, planet->floor->object.box));
	}

	for (u8 i = 0; i < planet->num_platforms; i++) {
		Box_s16 object_box = planet->platforms[i]->object.box;
		if (hitRight(target_box, object_box)) {
			return FIX16(adjacentXOnTheRight(target_box, object_box));
		}
	}

	return FIX16_0;
}

static void drawJetman(Jetman* jetman) {

	Sprite* sprite = jetman->sprite;

	if (jetman->airborne) {
		// somewhere in the air
		SPR_setAnim(sprite, ANIM_FLY);

	} else {
		SPR_setAnim(sprite, ANIM_WALK);
		if (jetman->object.mov.x) {
			// walking
			jetman->walk_step_counter++;
			jetman->walk_step_counter %= STEPPING_SPEED;
			if (!jetman->walk_step_counter) {
				// controlling the animation speed
				SPR_nextFrame(sprite);
			}
		}
	}

	SPR_setHFlip(sprite, jetman->head_back);
}

static void handleInputJetman(Jetman* jetman, u16 joy) {

	u16 value = JOY_readJoypad(joy);

	if (value & BUTTON_B) {
		jetman->order.y = -1;
	} else {
		jetman->order.y = 0;
	}

	if (value & BUTTON_LEFT) {
		jetman->order.x = -1;
	} else if (value & BUTTON_RIGHT) {
		jetman->order.x = +1;
	} else {
		jetman->order.x = 0;
	}

	bool order;
	bool pushed;

	if (value & BUTTON_C) {
		if (!pushed) {
			// detect flank
			order = TRUE;
		}
		pushed = TRUE;
	} else {
		pushed = FALSE;
	}

	if (joy & JOY_1) {
		p1_shoot_order = order;
		p1_shoot_pushed = pushed;
	} else {
		p2_shoot_order = order;
		p2_shoot_pushed = pushed;
	}
}
