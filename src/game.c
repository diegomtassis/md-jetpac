/*
 * game.c
 *
 *  Created on: Apr 20, 2019
 *      Author: diegomtassis
 */

#include "../inc/game.h"

#include <genesis.h>

#include "../inc/collectables.h"
#include "../inc/config.h"
#include "../inc/elements.h"
#include "../inc/enemies.h"
#include "../inc/spaceship.h"
#include "../inc/explosions.h"
#include "../inc/shooting.h"
#include "../inc/hud.h"
#include "../inc/events.h"
#include "../inc/jetman.h"
#include "../inc/level.h"
#include "../inc/planets.h"
#include "../inc/fwk/physics.h"

#define DEFAULT_FLASH_WAIT	2000

/**
 * @brief run a level.
 *
 * @param level
 * @return goal accomplished
 */
static bool runLevel(Level level[static 1]);

static bool isJetmanAlive(Level level[static 1]);
static bool isMissionAccomplished(Level level[static 1]);

static void waitForLanding(Level level[static 1]);
static void leavePlanet(Level level[static 1]);

static void handleCollisionsBetweenMovingObjects(Level level[static 1]);
static void handleElementsLeavingScreenUnder(Level level[static 1]);

static void scoreBonus(Level level[static 1]);
static void scorePoints(u16 points);

static void printMessage(const char* message);
static void flashMessage(const char *message, long wait);

static void joyEvent(u16 joy, u16 changed, u16 state);

volatile bool paused = FALSE;

static const V2u16 message_pos = { .x = 16, .y = 7 };

Game * current_game;

/**
 *
 * @param game
 */
void runGame(Game* game) {

	current_game = game;

	SPR_init();

	bool game_over = FALSE;

	/* when all the levels are passed start again */

	u8 level_number = 0;

	displayAmmo(game->mode & MODE_MD);

	while (!game_over) {

		//	log_memory();

		Level* current_level = game->createLevel[level_number]();
		current_level->game = game;

		startLevel(current_level);

		startSpaceship(current_level);
		waitForLanding(current_level);

		startJetman(current_level, game->mode & MODE_MD);
		startEnemies(current_level);

		startCollectables(current_level);
		initShots(current_level);
		initExplosions(current_level);

		SPR_update();
		VDP_waitVSync();

		JOY_setEventHandler(joyEvent);

		game_over = !runLevel(current_level);

		if (game_over) {
			flashMessage("Game Over", DEFAULT_FLASH_WAIT);

		} else {
			SPR_setVisibility(current_level->jetman->sprite, HIDDEN);
			leavePlanet(current_level);
			scoreBonus(current_level);
			updateHud(current_game, current_level->jetman);

			if (++level_number == game->num_levels) {
				level_number = 0;
			}

			waitMs(500);
		}

		releaseExplosions(current_level);
		releaseShots(current_level);
		releaseCollectables(current_level);
		releaseEnemies(current_level);
		releaseJetman(current_level);
		releaseSpaceship(current_level);
		releaseLevel(current_level);
		current_level = 0;

		SPR_update();

		VDP_clearPlan(PLAN_B, TRUE);
	}

	SPR_end();

	current_game = 0;
	return;
}

void releaseGame(Game* game) {

	if (!game) {
		return;
	}

	for (int idx = 0; idx < game->num_levels; ++idx) {
		if (game->createLevel[idx]) {
			MEM_free(game->createLevel[idx]);
			game->createLevel[idx] = 0;
		}
	}
	MEM_free(game->createLevel);
	game->createLevel = 0;

	MEM_free(game);
}

void scoreByEvent(GameEvent event) {

	if (!current_game) {
		return;
	}

	switch (event) {

	case KILLED_ENEMY_01:
		current_game->score += 25;
		break;

	case KILLED_ENEMY_02:
		current_game->score += 80;
		break;

	case GRABBED_SPACESHIP_PART:
		current_game->score += 100;
		break;

	case GRABBED_FUEL:
		current_game->score += 100;
		break;

	case GRABBED_COLLECTABLE:
		current_game->score += 250;
		break;

	case LOST_FUEL:
		current_game->score -= 50;
		break;

	case LOST_COLLECTABLE:
		current_game->score -= 25;
		break;

	default:
		break;
	}
}

static bool runLevel(Level current_level[static 1]) {

	bool jetman_alive = TRUE;
	bool game_over = FALSE;
	bool mission_accomplished = FALSE;

	while (!game_over && !mission_accomplished) {

		if (!paused) {

			if (jetman_alive) {

				jetmanActs(current_level);
				enemiesAct(current_level);
				handleCollisionsBetweenMovingObjects(current_level);
				if (current_level->def.mind_bottom) {
					handleElementsLeavingScreenUnder(current_level);
				}

				handleSpaceship(current_level);

				jetman_alive = isJetmanAlive(current_level);
				if (!jetman_alive) {
					dropIfGrabbed(current_level->spaceship);
					current_game->lives--;
				}

			} else {

				// Smart dying, wait for explosions to finish
				if (!current_level->booms.count) {

					waitMs(100);

					releaseEnemies(current_level);
					if (current_game->lives > 0) {
						resetJetman(current_level);
						startEnemies(current_level);
						jetman_alive = TRUE;
					}
				}
			}

			updateCollectables(current_level);
			updateShots(current_level);
			updateExplosions(current_level);

			mission_accomplished = jetman_alive && isMissionAccomplished(current_level);
			game_over = !current_game->lives && !current_level->booms.count;
			SPR_update();
		}

		updateHud(current_game, current_level->jetman);
		VDP_waitVSync();
	}

	return mission_accomplished;
}

static void handleCollisionsBetweenMovingObjects(Level level[static 1]) {

	for (u8 enemy_idx = 0; enemy_idx < level->enemies.size; enemy_idx++) {

		Enemy* enemy = level->enemies.e[enemy_idx];
		if (enemy && (ALIVE & enemy->health)) {

			// enemy & shot
			if (checkHit(enemy->object.box, level)) {

				killEnemy(enemy, level, TRUE);
				switch (enemy->type) {
				case ENEMY_01:
					onEvent(KILLED_ENEMY_01);
					break;

				default:
					onEvent(KILLED_ENEMY_02);
					break;
				}

				continue;
			}

			// enemy & jetman
			if (overlap(level->jetman->object.box, enemy->object.box)) {
				killJetman(level, TRUE);
				killEnemy(enemy, level, TRUE);
				break;
			}
		}
	}
}

static void handleElementsLeavingScreenUnder(Level level[static 1]) {

	if ((ALIVE & level->jetman->health) && level->jetman->object.box.pos.y > BOTTOM_POS_V_PX_S16) {
		killJetman(level, FALSE);
	}

	for (u8 enemy_idx = 0; enemy_idx < level->enemies.size; enemy_idx++) {

		Enemy* enemy = level->enemies.e[enemy_idx];
		if (enemy && (ALIVE & enemy->health) && enemy->object.box.pos.y > BOTTOM_POS_V_PX_S16) {
			killEnemy(enemy, level, FALSE);
		}
	}
}

static bool isJetmanAlive(Level level[static 1]) {

	return ALIVE & level->jetman->health;
}

static bool isMissionAccomplished(Level level[static 1]) {

	return (level->spaceship->step == READY) && shareBase(level->jetman->object.box, level->spaceship->base_object->box);
}

static void waitForLanding(Level level[static 1]) {

	while (level->spaceship->step == LANDING) {

		handleSpaceship(level);

		SPR_update();
		VDP_waitVSync();
	}
}

static void leavePlanet(Level level[static 1]) {

	launch(level->spaceship);
	do {
		// keep life going while the orbiter lifts
		handleSpaceship(level);
		enemiesAct(level);
		updateExplosions(level);
		updateShots(level);
		updateCollectables(level);

		SPR_update();
		VDP_waitVSync();

	} while (level->spaceship->step == LIFTING);
}

void static scoreBonus(Level level[static 1]) {

	if (current_game->mode & MODE_MD) {

		u16 ammo_bonus = 0;
		char bonus_message[19];

		sprintf(bonus_message, "Bonus %03d", ammo_bonus);
		printMessage(bonus_message);
		waitMs(500);

		while (level->jetman->ammo--) {

			ammo_bonus += 10;
			sprintf(bonus_message, "Bonus %03d", ammo_bonus);
			printMessage(bonus_message);
			waitMs(25);
			updateAmmo(level->jetman);
			VDP_waitVSync();
		}

		flashMessage(bonus_message, 1000);
		level->jetman->ammo = 0;
		scorePoints(ammo_bonus);
	}
}

static void scorePoints(u16 points) {

	if (!current_game) {
		return;
	}

	current_game->score += points;
}

static void printMessage(const char *message) {

	// center the message
	u8 x_pos = message_pos.x - strlen(message) / 2;
	VDP_drawText(message, x_pos, message_pos.y);
}

static void flashMessage(const char *message, long wait) {

	printMessage(message);
	waitMs(wait);
	VDP_clearTextLine(message_pos.y);
}

static void joyEvent(u16 joy, u16 changed, u16 state) {

	if (BUTTON_START & changed & ~state) {
		paused ^= 1;
	}
}
