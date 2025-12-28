/*
 * jetman.c
 *
 *  Created on: Apr 16, 2019
 *      Author: diegomtassis
 */

#include "../inc/jetman.h"

#include <genesis.h>

#include "../inc/config/game_config.h"
#include "../inc/config/sandbox_config.h"
#include "../inc/constants.h"
#include "../inc/explosions.h"
#include "../inc/fwk/commons.h"
#include "../inc/fwk/physics.h"
#include "../inc/game.h"
#include "../inc/hud.h"
#include "../inc/physical_constants.h"
#include "../inc/planet.h"
#include "../inc/players.h"
#include "../inc/shooting.h"
#include "../inc/spaceship.h"
#include "../res/zx.h"

#define ANIM_WALK 0
#define ANIM_FLY 1
#define STEPPING_SPEED 6 // 0 Maximum

#define SPEED_ZERO FIX16_0
#define SPEED_H_WALK FIX16(1)
#define SPEED_H_FLY FIX16(1.5)
#define SPEED_V_UP_MAX FIX16(-1.5)
#define ACCELERATION_H FIX16(0.2)
#define UP_ACCELERATION FIX16(-0.2)
#define SPEED_LOST_IN_CRASH FIX16(0.15)

#define UP 0x01
#define DOWN 0x02
#define RIGHT 0x04
#define LEFT 0x10
#define BOOST 0x20

#define JETMAN_HEIGHT 24
#define JETMAN_WIDTH 16

Jetman *j1;
Jetman *j2;

static Jetman *startJetman(Player *player, Planet planet[static 1]);
static Jetman *createJetman(Player *player);
static void releaseJetman(Jetman jetman[static 1]);
static void handleInputJetman(Jetman *jetman);

static void moveToStart(Jetman *jetman, V2f16 init_pos);
static void shapeJetman(Jetman jetman[static 1], const SpriteDefinition *sprite, u16 ammo);
static V2f16 figureOutInitPosition(const Planet planet[static 1], u8 player_id);
static void moveJetman(Jetman *, Planet *);
static u8 calculateNextMovement(Jetman *);
static void updatePosition(Jetman *, Planet *);
static f16 reachedTop(Box_s16, const Planet *);
static f16 hitPlatformUnder(Box_s16, const Planet *);
static f16 blockedByRight(Box_s16, const Planet *);
static f16 blockedByLeft(Box_s16, const Planet *);

static void drawJetman(Jetman *);

bool joy_pushed[256];
bool joy_flank[256];

void startJetmen(Planet planet[static 1]) {

    if (game.p1 && game.p1->lives > 0) {
        planet->j1 = startJetman(game.p1, planet);
        planet->j1->immunity = game_config.immunity;
        j1 = planet->j1;
    }

    if (game.p2 && game.p2->lives > 0) {
        planet->j2 = startJetman(game.p2, planet);
        planet->j2->immunity = game_config.immunity;
        j2 = planet->j2;
    }
}

void releaseJetmen(Planet planet[static 1]) {

    if (planet->j1) {
        releaseJetman(planet->j1);
        planet->j1 = 0;
        j1 = 0;
    }

    if (planet->j2) {
        releaseJetman(planet->j2);
        planet->j2 = 0;
        j2 = 0;
    }
}

void resetJetman(Jetman *jetman, Planet planet[static 1]) {

    if (!jetman) {
        return;
    }

    moveToStart(jetman, figureOutInitPosition(planet, jetman->id));
    jetman->health = ALIVE;
    return;
}

void killJetman(Jetman *jetman, Planet planet[static 1], bool exploding) {

    if (jetman->immunity) {
        return;
    }

    if (exploding) {
        explode(jetman->object.box, planet);
    }

    jetman->health = DEAD;
}

void jetmanActs(Jetman *jetman, Planet planet[static 1]) {

    if (!jetman || !(jetman->health & ALIVE) || jetman->finished) {
        return;
    }

    handleInputJetman(jetman);
    moveJetman(jetman, planet);

    const bool limited_ammo = game_config.limited_ammo;

    if (spaceship_ready && shareBase(&jetman->object.box, &planet->spaceship->base_object->box)) {
        jetman->finished = TRUE;

    } else if (joy_flank[jetman->joystick] && (jetman->ammo || !limited_ammo)) {

        shoot(jetman, planet);
        if (limited_ammo) {
            jetman->ammo--;
        }
        joy_flank[jetman->joystick] = FALSE;
    }
    drawJetman(jetman);
}

bool resurrectOrRelease(Jetman *jetman, Planet planet[static 1]) {

    if (!jetman) {
        return FALSE;
    }

    if (jetman->player->lives) {
        resetJetman(jetman, planet);
        return TRUE;
    }

    if (jetman->id == P1) {
        planet->j1 = 0;
        j1 = 0;
    } else {
        planet->j2 = 0;
        j2 = 0;
    }

    releaseJetman(jetman);

    return FALSE;
}

bool isJetmanAlive(Jetman *jetman) { return jetman && (ALIVE & jetman->health); }

void updateJetmanStatus(Jetman *jetman, bool *alive, Planet planet[static 1]) {

    if (!(*alive)) {
        return;
    }

    if (!(*alive = isJetmanAlive(jetman))) {

        dropIfGrabbed(jetman, planet->spaceship);
        jetman->player->lives--;
    }

    HUD_updatePlayerHud(jetman->player, jetman->ammo);
}

static Jetman *startJetman(Player *player, Planet planet[static 1]) {

    Jetman *jetman = createJetman(player);

	jetman->gravity = planet->def->gravity;

    moveToStart(jetman, figureOutInitPosition(planet, player->id));
    shapeJetman(jetman, player->id == P1 ? &carl_sprite : &ann_sprite, planet->def->ammo);

    return jetman;
}

static Jetman *createJetman(Player *player) {

    Jetman *jetman = MEM_calloc(sizeof *jetman);

    jetman->id = player->id;
    jetman->joystick = player->id == P1 ? JOY_1 : JOY_2;
    ;

    jetman->object.size.x = JETMAN_WIDTH;
    jetman->object.size.y = JETMAN_HEIGHT;

    jetman->object.box.w = JETMAN_WIDTH;
    jetman->object.box.h = JETMAN_HEIGHT;

    jetman->health = ALIVE;

    jetman->player = player;

    return jetman;
}

static void releaseJetman(Jetman jetman[static 1]) {

    if (!jetman) {
        return;
    }

    SPR_setVisibility(jetman->sprite, FALSE);
    SPR_releaseSprite(jetman->sprite);
    jetman->sprite = 0;
    jetman->player = 0;
    MEM_free(jetman);
}

static void moveToStart(Jetman *jetman, V2f16 init_pos) {

    jetman->object.pos.x = init_pos.x;
    jetman->object.pos.y = init_pos.y;

    jetman->object.mov.x = SPEED_ZERO;
    jetman->object.mov.y = SPEED_ZERO;

    updateBox(&jetman->object);
}

static void shapeJetman(Jetman jetman[static 1], const SpriteDefinition *sprite, u16 ammo) {

    jetman->sprite = SPR_addSprite(sprite, F16_toInt(jetman->object.pos.x), F16_toInt(jetman->object.pos.y),
                                   TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
    jetman->ammo = ammo;
}

static V2f16 figureOutInitPosition(const Planet planet[static 1], u8 player_id) {

    V2f16 init_pos;

    if (player_id == P1) {

        if (planet->def->p1_init_pos) {

            init_pos.x = FIX16(planet->def->p1_init_pos->x);
            init_pos.y = FIX16(planet->def->p1_init_pos->y);

        } else {
            init_pos.x = FIX16(124);
            init_pos.y = planet->floor->object.pos.y - FIX16(8 * 3);
        }

    } else {

        init_pos.x = FIX16(80);
        init_pos.y = planet->floor->object.pos.y - FIX16(8 * 3);
    }

    return init_pos;
}

static void moveJetman(Jetman *jetman, Planet *planet) {

    if (BOOST & calculateNextMovement(jetman)) {
        boost(jetman->object.box, planet);
    }
    updatePosition(jetman, planet);
    SPR_setPosition(jetman->sprite, F16_toInt(jetman->object.pos.x), F16_toInt(jetman->object.pos.y));
}

static u8 calculateNextMovement(Jetman *jetman) {

    u8 movement = 0;

    // horizontal movement
    if (jetman->order.x > 0) {

        jetman->object.mov.x += ACCELERATION_H;
        f16 max_speed = jetman->airborne ? SPEED_H_FLY : SPEED_H_WALK;

        if (jetman->object.mov.x > 0) {
            jetman->head_back = FALSE;
            movement |= RIGHT;
            if (jetman->object.mov.x > max_speed) {
                jetman->object.mov.x = max_speed;
            }
        }

    } else if (jetman->order.x < 0) {

        jetman->object.mov.x -= ACCELERATION_H;
        f16 max_speed = jetman->airborne ? -SPEED_H_FLY : -SPEED_H_WALK;

        if (jetman->object.mov.x < 0) {
            jetman->head_back = TRUE;
            movement |= LEFT;
            if (jetman->object.mov.x < max_speed) {
                jetman->object.mov.x = max_speed;
            }
        }

    } else {
        // do not stop suddenly
        if (jetman->object.mov.x > 0) {
            if (jetman->object.mov.x >= ACCELERATION_H) {
                jetman->object.mov.x -= ACCELERATION_H;
            } else {
                jetman->object.mov.x = SPEED_ZERO;
            }

        } else if (jetman->object.mov.x < 0) {
            if (jetman->object.mov.x <= -ACCELERATION_H) {
                jetman->object.mov.x += ACCELERATION_H;
            } else {
                jetman->object.mov.x = SPEED_ZERO;
            }
        }
    }

    // vertical movement
    if (jetman->order.y < 0) {
        if (!jetman->airborne) {
            movement |= BOOST;
        }

        jetman->object.mov.y += UP_ACCELERATION;
        if (jetman->object.mov.y < SPEED_V_UP_MAX) {
            jetman->object.mov.y = SPEED_V_UP_MAX;
        }

        movement |= UP;

    } else {
        /*
         * either falling or walking. But at this point it's not known yet whether he's walking,
         * so by default he's falling.
         */
        jetman->object.mov.y += jetman->gravity;
        if (jetman->object.mov.y > DEFAULT_SPEED_V_DOWN_MAX) {
            jetman->object.mov.y = DEFAULT_SPEED_V_DOWN_MAX;
        }
        movement |= DOWN;
    }

    return movement;
}

static void updatePosition(Jetman *jetman, Planet *planet) {

    // horizontal position
    Box_s16 target_h = targetHBox(&jetman->object);
    if (target_h.min.x > MAX_POS_H_PX_S16) {
        jetman->object.pos.x = MIN_POS_H_PX_F16;

    } else if (target_h.min.x < MIN_POS_H_PX_S16) {
        jetman->object.pos.x = MAX_POS_H_PX_F16;

    } else {

        f16 blockedInAxisX = blockedByLeft(target_h, planet);
        if (blockedInAxisX && jetman->object.mov.x < 0) {

            // bounce by reversing its speed, losing some momentum in the bounce
            jetman->object.mov.x = -jetman->object.mov.x - SPEED_LOST_IN_CRASH;
            if (jetman->object.mov.x < 0) {
                jetman->object.mov.x = SPEED_ZERO;
                jetman->object.pos.x = blockedInAxisX;
            } else {
                jetman->object.pos.x = blockedInAxisX + jetman->object.mov.x;
            }

        } else {
            blockedInAxisX = blockedByRight(target_h, planet);
            if (blockedInAxisX && jetman->object.mov.x > 0) {

                // bounce by reversing its speed, losing some momentum in the bounce
                jetman->object.mov.x = -jetman->object.mov.x + SPEED_LOST_IN_CRASH;
                if (jetman->object.mov.x > 0) {
                    jetman->object.mov.x = SPEED_ZERO;
                    jetman->object.pos.x = blockedInAxisX;
                } else {
                    jetman->object.pos.x = blockedInAxisX + jetman->object.mov.x;
                }

            } else {
                jetman->object.pos.x += jetman->object.mov.x;
            }
        }
    }

    // vertical position
    Box_s16 target_v = targetVBox(&jetman->object);
    f16 landed_pos_y = LOC_landed(target_v, planet);
    jetman->airborne = !landed_pos_y;

    if (landed_pos_y) {
        jetman->object.pos.y = landed_pos_y;
        jetman->object.mov.y = SPEED_ZERO;

    } else {
        f16 limit_pos_y = reachedTop(target_v, planet);
        if (limit_pos_y) {
            jetman->object.pos.y = limit_pos_y;
            if (jetman->object.mov.y < 0) {
                jetman->object.mov.y = SPEED_ZERO;
            }

        } else {
            // hitting a platform underneath?
            limit_pos_y = hitPlatformUnder(target_v, planet);
            if (limit_pos_y && jetman->object.mov.y < 0) {
                // bounce by reversing its speed, losing some momentum in the bounce
                jetman->object.mov.y = -jetman->object.mov.y - SPEED_LOST_IN_CRASH;
                if (jetman->object.mov.y < 0) {
                    jetman->object.mov.y = SPEED_ZERO;
                    jetman->object.pos.y = limit_pos_y;
                } else {
                    jetman->object.pos.y = limit_pos_y + jetman->object.mov.y;
                }

            } else {
                jetman->object.pos.y += jetman->object.mov.y;
            }
        }
    }

    // update box
    updateBox(&jetman->object);
}

static f16 reachedTop(Box_s16 subject_box, const Planet *planet) {

    if (subject_box.min.y <= MIN_POS_V_PX_S16) {
        return MIN_POS_V_PX_F16;
    }

    return FIX16_0;
}

static f16 hitPlatformUnder(Box_s16 subject_box, const Planet *planet) {

    for (u8 idx = planet->num_platforms; idx;) {
        Box_s16 object_box = planet->platforms[--idx]->object.box;
        if (hitUnder(&subject_box, &object_box)) {
            return FIX16(adjacentYUnder(&subject_box, &object_box));
        }
    }

    if (hitUnder(&subject_box, &planet->floor->object.box)) {
        return FIX16(adjacentYUnder(&subject_box, &planet->floor->object.box));
    }

    return FIX16_0;
}

static f16 blockedByRight(Box_s16 target_box, const Planet *planet) {

    if (planet->floor && hitLeft(&target_box, &planet->floor->object.box)) {
        return FIX16(adjacentXOnTheLeft(&target_box, &planet->floor->object.box));
    }

    for (u8 idx = planet->num_platforms; idx;) {
        Box_s16 object_box = planet->platforms[--idx]->object.box;
        if (hitLeft(&target_box, &object_box)) {
            return FIX16(adjacentXOnTheLeft(&target_box, &object_box));
        }
    }

    return FIX16_0;
}

static f16 blockedByLeft(Box_s16 target_box, const Planet *planet) {

    if (planet->floor && hitRight(&target_box, &planet->floor->object.box)) {
        return FIX16(adjacentXOnTheRight(&target_box, &planet->floor->object.box));
    }

    for (u8 idx = planet->num_platforms; idx;) {
        Box_s16 object_box = planet->platforms[--idx]->object.box;
        if (hitRight(&target_box, &object_box)) {
            return FIX16(adjacentXOnTheRight(&target_box, &object_box));
        }
    }

    return FIX16_0;
}

static void drawJetman(Jetman *jetman) {

    Sprite *sprite = jetman->sprite;

    if (jetman->finished) {
        SPR_setVisibility(sprite, HIDDEN);

    } else if (jetman->airborne) {
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

static void handleInputJetman(Jetman *jetman) {

    u8 joy = jetman->joystick;
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

    if (value & BUTTON_C) {
        if (!joy_pushed[joy]) {
            // detect flank
            joy_flank[joy] = TRUE;
        }
        joy_pushed[joy] = TRUE;
    } else {
        joy_pushed[joy] = FALSE;
    }
}
