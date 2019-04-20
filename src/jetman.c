/*
 * jetman.c
 *
 *  Created on: Apr 16, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../inc/levels.h"
#include "../res/sprite.h"

#define ANIM_WALK		0
#define ANIM_FLY		1
#define STEPPING_SPEED    	6	// 0 Maximum

#define SPEED_ZERO    	FIX16(0)
#define SPEED_H_NORMAL  FIX16(1.5)
#define SPEED_V_UP    	FIX16(-1.5)
#define SPEED_V_DOWN    FIX16(1.5)

#define MIN_POS_H_PX       -8
#define MAX_POS_H_PX       248
#define MAX_POS_V_PX 		32

#define MIN_POS_H_PX_F16    FIX16(MIN_POS_H_PX)
#define MAX_POS_H_PX_F16    FIX16(MAX_POS_H_PX)
#define MAX_POS_V_PX_F16 	FIX16(MAX_POS_V_PX)

static void handleInputJetman();

typedef struct {
	u8 walk_step_counter;
} JetmanAnimation;

static Jetman* createPlayer1(const Level*);
static void moveJetman(Jetman*, const Level*);
static void calculateNextMovement(Jetman*);
static void updatePosition(Jetman*, const Level*);
static Vect2D_f16 targetPosition(const Jetman*);
static Box_f16 targetBox(Vect2D_f16);
static fix16 landed(Box_f16, const Level*);
static fix16 reachedTop(Box_f16, const Level*);
static fix16 blockedByLeft(Box_f16, const Level*);
static fix16 blockedByRight(Box_f16, const Level*);

static void animateJetman(const Jetman*, JetmanAnimation*, Sprite*);

Sprite* sprites[1];

Jetman* player1;
JetmanAnimation p1_anim;

fix16 floor_px_f16;

void startJetman(const Level* level) {

	floor_px_f16 = FIX16(level->floor->pos_t.y * 8);

	player1 = createPlayer1(level);
	sprites[0] = SPR_addSprite(&jetman_sprite, fix16ToInt(player1->object.pos.x), fix16ToInt(player1->object.pos.y),
			TILE_ATTR(PAL0, TRUE, FALSE, FALSE));

	while (TRUE) {

		handleInputJetman();

		moveJetman(player1, level);
		animateJetman(player1, &p1_anim, sprites[0]);

		SPR_update();
		VDP_waitVSync();
	}
}

static Jetman* createPlayer1(const Level* level) {

	Jetman* p1 = MEM_alloc(sizeof(Jetman));

	p1->object.pos.x = FIX16(124);
	p1->object.pos.y = fix16Sub(floor_px_f16, FIX16(8*3));

	p1->object.mov.x = SPEED_ZERO;
	p1->object.mov.y = SPEED_ZERO;
	p1->object.size.x = 16;
	p1->object.size.y = 24;

	Box_f16 box = { .x = p1->object.pos.x, //
			.y = p1->object.pos.y, //
			.w = p1->object.size.x, //
			.h = p1->object.size.y };
	p1->object.box = &box;

	return p1;
}

static void moveJetman(Jetman* player, const Level* level) {

	calculateNextMovement(player);
	updatePosition(player, level);

	SPR_setPosition(sprites[0], fix16ToInt(player->object.pos.x), fix16ToInt(player->object.pos.y));
}

static void calculateNextMovement(Jetman* player) {

	// horizontal movement
	if (player->order.x > 0) {
		player->object.mov.x = SPEED_H_NORMAL;

	} else if (player->order.x < 0) {
		player->object.mov.x = -SPEED_H_NORMAL;

	} else {
		player->object.mov.x = SPEED_ZERO;
	}

	// vertical movement
	if (player->order.y < 0) {
		player->object.mov.y = SPEED_V_UP;

	} else {
		player->object.mov.y = SPEED_V_DOWN;
	}
}

static void updatePosition(Jetman* player, const Level* level) {

	Vect2D_f16 target_pos = targetPosition(player);
	Box_f16 target_box = targetBox(target_pos);

	// horizontal position
	if (target_pos.x > MAX_POS_H_PX_F16) {
		player->object.pos.x = MIN_POS_H_PX_F16;

	} else if (target_pos.x < MIN_POS_H_PX_F16) {
		player->object.pos.x = MAX_POS_H_PX_F16;

	} else if (!blockedByLeft(target_box, level) && !blockedByRight(target_box, level)) {
		player->object.pos.x = target_pos.x;
	}

	// vertical position
	fix16 landed_pos_y = landed(target_box, level);
	if (landed_pos_y) {
		player->object.pos.y = landed_pos_y;
		player->object.mov.y = SPEED_ZERO;

	} else {
		fix16 reachedTop_pos_y = reachedTop(target_box, level);
		if (reachedTop_pos_y) {
			player->object.pos.y = MAX_POS_V_PX_F16;

		} else {
			player->object.pos.y = target_pos.y;
		}
	}
}

static Vect2D_f16 targetPosition(const Jetman* player) {

	fix16 target_x = fix16Add(player->object.pos.x, player->object.mov.x);
	fix16 target_y = fix16Add(player->object.pos.y, player->object.mov.y);

	Vect2D_f16 newPos = { .x = target_x, .y = target_y };

	return newPos;
}

static Box_f16 targetBox(Vect2D_f16 pos) {

	Box_f16 box;

	box.x = pos.x;
	box.y = pos.y;
	box.w = 16;
	box.h = 24;

	return box;
}

static fix16 landed(Box_f16 box, const Level* level) {

	fix16 max_y = fix16Sub(floor_px_f16, FIX16(box.h));
	if (box.y >= max_y) {
		return max_y;
	}

	return 0;
}

static fix16 reachedTop(Box_f16 box, const Level* level) {

	if (box.y <= MAX_POS_V_PX_F16) {
		return MAX_POS_V_PX_F16;
	}

	return 0;
}

static fix16 blockedByLeft(Box_f16 box, const Level* level) {

	return 0;
}

static fix16 blockedByRight(Box_f16 box, const Level* level) {

	return 0;
}

static void animateJetman(const Jetman* player, JetmanAnimation* p_anim, Sprite* sprite) {

	if (player->object.mov.y) {
		// somewhere in the air
		SPR_setAnim(sprite, ANIM_FLY);

	} else {
		SPR_setAnim(sprites[0], ANIM_WALK);
		if (player->object.mov.x) {
			// walking
			p_anim->walk_step_counter++;
			p_anim->walk_step_counter %= STEPPING_SPEED;
			if (!p_anim->walk_step_counter) {
				// controlling the animation speed
				SPR_nextFrame(sprite);
			}
		}
	}

	SPR_setHFlip(sprite, player->object.mov.x < 0);
}

static void handleInputJetman() {

	u16 value = JOY_readJoypad(JOY_1);

	if (value & BUTTON_B) {
		player1->order.y = -1;
	} else {
		player1->order.y = 0;
	}

	if (value & BUTTON_LEFT) {
		player1->order.x = -1;
	} else if (value & BUTTON_RIGHT) {
		player1->order.x = +1;
	} else {
		player1->order.x = 0;
	}
}
