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
#include "../inc/planet.h"
#include "../inc/planets.h"
#include "../inc/players.h"
#include "../inc/fwk/physics.h"

#define DEFAULT_FLASH_WAIT	2000

/**
 * @brief run a planet.
 *
 * @param planet
 * @return goal accomplished
 */
static Game* createGame(Config config[static 1]);
static void releaseGame(Game* game);

static bool runPlanet(Planet planet[static 1]);

static bool isPlayerAlive(Jetman* player);
static bool isMissionAccomplished(Planet planet[static 1]);

static void waitForLanding(Planet planet[static 1]);
static void leavePlanet(Planet planet[static 1]);

static void handleCollisionsBetweenMovingObjects(Planet planet[static 1]);
static void handleElementsLeavingScreenUnder(Planet planet[static 1]);

static void scoreBonus(Planet planet[static 1]);
static void scorePoints(u16 points, Player player);

static void printMessage(const char* message);
static void flashMessage(const char *message, long wait);

static void joyEvent(u16 joy, u16 changed, u16 state);

volatile bool paused = FALSE;

static const V2u16 message_pos = { .x = 16, .y = 7 };

Game * current_game;

GameResult runGame(Config config[static 1]) {

	current_game = createGame(config);

	SPR_init();

	bool game_over = FALSE;

	/* when all the planets are passed start again */

	u8 planet_number = 0;

	displayAmmo(config->mode == MD);

	while (!game_over) {

		//	log_memory();

		Planet* current_planet = config->createPlanet[planet_number]();
		current_planet->game = current_game;

		startPlanet(current_planet);

		startSpaceship(current_planet);
		waitForLanding(current_planet);

		startPlayers(current_planet, current_game->p1, current_game->p2);
		current_planet->p1->immune = config->difficulty == EASY;
		if (current_planet->p2) {
			current_planet->p2->immune = current_planet->p1->immune;
		}

		startEnemies(current_planet);

		startCollectables(current_planet);
		initShots(current_planet);
		initExplosions(current_planet);

		SPR_update();
		VDP_waitVSync();

		JOY_setEventHandler(joyEvent);

		game_over = !runPlanet(current_planet);

		if (game_over) {
			flashMessage("Game Over", DEFAULT_FLASH_WAIT);

		} else {
			SPR_setVisibility(current_planet->p1->sprite, HIDDEN);
			leavePlanet(current_planet);
			scoreBonus(current_planet);
			updateHud(current_game, current_planet->p1);

			if (++planet_number == config->num_planets) {
				planet_number = 0;
			}

			waitMs(500);
		}

		releaseExplosions(current_planet);
		releaseShots(current_planet);
		releaseCollectables(current_planet);
		releaseEnemies(current_planet);
		releasePlayers(current_planet);
		releaseSpaceship(current_planet);
		releasePlanet(current_planet);
		current_planet = 0;

		SPR_update();

		VDP_clearPlan(PLAN_B, TRUE);
	}

	SPR_end();

	GameResult result = { //
			.p1_score = current_game->p1->score, //
					.p2_score = current_game->p2->score //
			};

	releaseGame(current_game);
	current_game = 0;

	return result;
}

void scoreByEvent(GameEvent event, u8 player) {

	if (!current_game) {
		return;
	}

	switch (event) {

	case KILLED_ENEMY:
		current_game->p1->score += 25;
		break;

	case GRABBED_SPACESHIP_PART:
		current_game->p1->score += 100;
		break;

	case GRABBED_FUEL:
		current_game->p1->score += 100;
		break;

	case GRABBED_COLLECTABLE:
		current_game->p1->score += 250;
		break;

	case LOST_FUEL:
		current_game->p1->score -= 50;
		break;

	case LOST_COLLECTABLE:
		current_game->p1->score -= 25;
		break;

	default:
		break;
	}
}

static Game* createGame(Config config[static 1]) {

	Game* game = MEM_alloc(sizeof *game);

	game->config = config;
	game->planet = 0;

	game->p1 = MEM_alloc(sizeof(*game->p1));
	game->p1->lives = config->lives;
	game->p1->score = 0;

	if (config->players == TWO_PLAYERS) {
		game->p2 = MEM_alloc(sizeof(*game->p2));
		game->p2->lives = config->lives;
		game->p2->score = 0;
	}

	return game;
}

static void releaseGame(Game* game) {

	if (!game) {
		return;
	}

	if (game->p1) {
		MEM_free(game->p1);
	}

	if (game->p2) {
		MEM_free(game->p2);
	}

	MEM_free(game);
}

static bool runPlanet(Planet current_planet[static 1]) {

	bool p1_alive = TRUE;
	bool p2_alive = current_planet->p2 != 0; // comparison to avoid a warning
	bool game_over = FALSE;
	bool mission_accomplished = FALSE;

	Jetman* player;

	while (!game_over && !mission_accomplished) {

		if (!paused) {

			if (p1_alive) {

				playersAct(current_planet);
				enemiesAct(current_planet);
				handleCollisionsBetweenMovingObjects(current_planet);
				if (current_planet->def.mind_bottom) {
					handleElementsLeavingScreenUnder(current_planet);
				}

				handleSpaceship(current_planet);

				player = current_planet->p1;
				p1_alive = isPlayerAlive(player);
				if (!p1_alive) {
					dropIfGrabbed(current_planet->spaceship);
					current_game->p1->lives--;
				}

				player = current_planet->p2;
				if (player) {
					p2_alive = isPlayerAlive(player);
					if (!p2_alive) {
						dropIfGrabbed(current_planet->spaceship);
						current_game->p2->lives--;
					}
				}

			} else {

				// Smart dying, wait for explosions to finish
				if (!current_planet->booms.count) {

					waitMs(100);

					releaseEnemies(current_planet);

					if (current_game->p1->lives > 0) {
						resetPlayer(current_planet->p1, current_planet);
						startEnemies(current_planet);
						p1_alive = TRUE;
					}
				}
			}

			updateCollectables(current_planet);
			updateShots(current_planet);
			updateExplosions(current_planet);

			mission_accomplished = p1_alive && isMissionAccomplished(current_planet);
			game_over = !current_game->p1->lives && !current_planet->booms.count;
			SPR_update();
		}

		updateHud(current_game, current_planet->p1);
		VDP_waitVSync();
	}

	return mission_accomplished;
}

static void handleCollisionsBetweenMovingObjects(Planet planet[static 1]) {

	for (u8 enemy_idx = 0; enemy_idx < planet->enemies.size; enemy_idx++) {

		Enemy* enemy = planet->enemies.e[enemy_idx];
		if (enemy && (ALIVE & enemy->health)) {

			// enemy & shot
			u8 shooter = checkHit(enemy->object.box, planet);
			if (shooter) {
				killEnemy(enemy, planet, TRUE);
				onEvent(KILLED_ENEMY, shooter);
				continue;
			}

			// enemy & p1
			Jetman* player = planet->p1;
			if (!player->immune && overlap(player->object.box, enemy->object.box)) {
				killPlayer(player, planet, TRUE);
				killEnemy(enemy, planet, TRUE);
				break;
			}

			// enemy & p2
			player = planet->p2;
			if (player && !player->immune && overlap(player->object.box, enemy->object.box)) {
				killPlayer(player, planet, TRUE);
				killEnemy(enemy, planet, TRUE);
				break;
			}
		}
	}
}

static void handleElementsLeavingScreenUnder(Planet planet[static 1]) {

	// p1
	Jetman* player = planet->p1;
	if ((ALIVE & player->health) && player->object.box.pos.y > BOTTOM_POS_V_PX_S16) {
		killPlayer(player, planet, FALSE);
	}

	// p2
	player = planet->p2;
	if (player && (ALIVE & player->health) && player->object.box.pos.y > BOTTOM_POS_V_PX_S16) {
		killPlayer(player, planet, FALSE);
	}

	// enemies
	for (u8 enemy_idx = 0; enemy_idx < planet->enemies.size; enemy_idx++) {

		Enemy* enemy = planet->enemies.e[enemy_idx];
		if (enemy && (ALIVE & enemy->health) && enemy->object.box.pos.y > BOTTOM_POS_V_PX_S16) {
			killEnemy(enemy, planet, FALSE);
		}
	}
}

static bool isPlayerAlive(Jetman* player) {

	return player && (ALIVE & player->health);
}

static bool isMissionAccomplished(Planet planet[static 1]) {

	return (planet->spaceship->step == READY) && shareBase(planet->p1->object.box, planet->spaceship->base_object->box);
}

static void waitForLanding(Planet planet[static 1]) {

	while (planet->spaceship->step == LANDING) {

		handleSpaceship(planet);

		SPR_update();
		VDP_waitVSync();
	}
}

static void leavePlanet(Planet planet[static 1]) {

	launch(planet->spaceship);
	do {
		// keep life going while the orbiter lifts
		handleSpaceship(planet);
		enemiesAct(planet);
		updateExplosions(planet);
		updateShots(planet);
		updateCollectables(planet);

		SPR_update();
		VDP_waitVSync();

	} while (planet->spaceship->step == LIFTING);
}

void static scoreBonus(Planet planet[static 1]) {

	if (current_game->config->mode == MD) {

		u16 ammo_bonus = 0;
		char bonus_message[19];

		sprintf(bonus_message, "Bonus %03d", ammo_bonus);
		printMessage(bonus_message);
		waitMs(500);

		while (planet->p1->ammo--) {

			ammo_bonus += 10;
			sprintf(bonus_message, "Bonus %03d", ammo_bonus);
			printMessage(bonus_message);
			waitMs(25);
			updateAmmo(planet->p1);
			VDP_waitVSync();
		}

		flashMessage(bonus_message, 1000);
		planet->p1->ammo = 0;
		scorePoints(ammo_bonus, P1);
	}
}

static void scorePoints(u16 points, Player player) {

	if (!current_game) {
		return;
	}

	if (player == P1) {
		current_game->p1->score += points;

	} else if (player == P2) {
		current_game->p2->score += points;
	}

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
