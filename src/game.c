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

static bool isMissionAccomplished(Planet planet[static 1]);

static void waitForLanding(Planet planet[static 1]);
static void leavePlanet(Planet planet[static 1]);

static void handleCollisionsBetweenMovingObjects(Planet planet[static 1]);
static void handleElementsLeavingScreenUnder(Planet planet[static 1]);
static void handleCollisionBetweenJetmanAndEnemy(Jetman* jetman, Enemy* enemy, Planet planet[static 1]);

static void scoreBonus(Planet planet[static 1]);
static void scorePoints(u16 points, u8 player_id);

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

		startJetmen(current_planet);
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
			SPR_setVisibility(current_planet->j1->sprite, HIDDEN);
			SPR_setVisibility(current_planet->j2->sprite, HIDDEN);

			leavePlanet(current_planet);

			scoreBonus(current_planet);

			if (++planet_number == config->num_planets) {
				planet_number = 0;
			}

			waitMs(500);
		}

		releaseExplosions(current_planet);
		releaseShots(current_planet);
		releaseCollectables(current_planet);
		releaseEnemies(current_planet);
		releaseJetmen(current_planet);
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

void scoreByEvent(GameEvent event, u8 player_id) {

	if (!current_game) {
		return;
	}

	Player* status = player_id == P1 ? current_game->p1 : current_game->p2;
	if (!status) {
		return;
	}

	switch (event) {

	case KILLED_ENEMY:
		status->score += 25;
		break;

	case GRABBED_SPACESHIP_PART:
		status->score += 100;
		break;

	case GRABBED_FUEL:
		status->score += 100;
		break;

	case GRABBED_COLLECTABLE:
		status->score += 250;
		break;

	case LOST_FUEL:
		status->score -= 50;
		break;

	case LOST_COLLECTABLE:
		status->score -= 25;
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
	game->p1->id = P1;
	game->p1->lives = config->lives;
	game->p1->score = 0;

	if (config->players == TWO_PLAYERS) {
		game->p2 = MEM_alloc(sizeof(*game->p2));
		game->p2->id = P2;
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
	bool p2_alive = current_planet->j2 != 0; // comparison to avoid a warning
	bool game_over = FALSE;
	bool mission_accomplished = FALSE;

	while (!game_over && !mission_accomplished) {

		if (!paused) {

			u8 num_alive_before = p1_alive + p2_alive;

			if (num_alive_before) {

				jetmanActs(current_planet->j1, current_planet);
				jetmanActs(current_planet->j2, current_planet);

				enemiesAct(current_planet);
				handleCollisionsBetweenMovingObjects(current_planet);
				if (current_planet->def.mind_bottom) {
					handleElementsLeavingScreenUnder(current_planet);
				}

				handleSpaceship(current_planet);

				if (p1_alive) {
					updateJetmanStatus(current_planet->j1, &p1_alive, current_planet);
				}

				if (p2_alive) {
					updateJetmanStatus(current_planet->j2, &p2_alive, current_planet);
				}
			}

			/*
			 * Handling of dead is a complex task, that's why the following code is not straight forward.
			 * If in player mode or only one player is alive, dying implies restarting the enemies, but if one player dies
			 * while another player is alive then the enemies stay as they are but the resurrected player has immunity
			 * for a short period of time.
			 */
			if (!num_alive_before) {

				// Scenario 1: No players alive, though one may still have lives. Wait for the explosions to extinguish and try to resurrect the player.
				if (!current_planet->booms.count) {

					waitMs(100);

					// Explosions extinguished, resurrect the player with lives if there is any
					if (current_planet->j1 && !p1_alive) {
						p1_alive = resurrectOrRelease(current_planet->j1, current_planet);
					}
					if (current_planet->j2 && !p2_alive) {
						p2_alive = resurrectOrRelease(current_planet->j2, current_planet);
					}

					releaseEnemies(current_planet);
					if (p1_alive || p2_alive) {
						startEnemies(current_planet);
					}
				}

			} else if (num_alive_before == 2 && p1_alive + p2_alive == 1) {

				// Scenario 2: There was 2 alive before the ACT and one has just passed. Try to resurrect it.
				if (current_planet->j1 && !p1_alive) {
					p1_alive = resurrectOrRelease(current_planet->j1, current_planet);
					// TODO add immunity
				}
				if (current_planet->j2 && !p2_alive) {
					p2_alive = resurrectOrRelease(current_planet->j2, current_planet);
					// TODO add immunity
				}
			}

			updateCollectables(current_planet);
			updateShots(current_planet);
			updateExplosions(current_planet);

			mission_accomplished = (p1_alive || p2_alive) && isMissionAccomplished(current_planet);
			game_over = (!current_game->p1->lives && (!current_game->p2 || !current_game->p2->lives))
					&& !current_planet->booms.count;
			SPR_update();
		}

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

			handleCollisionBetweenJetmanAndEnemy(planet->j1, enemy, planet);
			handleCollisionBetweenJetmanAndEnemy(planet->j2, enemy, planet);
		}
	}
}

static void handleElementsLeavingScreenUnder(Planet planet[static 1]) {

	Jetman* jetman = planet->j1;
	if (jetman && (ALIVE & jetman->health) && jetman->object.box.pos.y > BOTTOM_POS_V_PX_S16) {
		killJetman(jetman, planet, FALSE);
	}

	jetman = planet->j2;
	if (jetman && (ALIVE & jetman->health) && jetman->object.box.pos.y > BOTTOM_POS_V_PX_S16) {
		killJetman(jetman, planet, FALSE);
	}

	// enemies
	for (u8 enemy_idx = 0; enemy_idx < planet->enemies.size; enemy_idx++) {

		Enemy* enemy = planet->enemies.e[enemy_idx];
		if (enemy && (ALIVE & enemy->health) && enemy->object.box.pos.y > BOTTOM_POS_V_PX_S16) {
			killEnemy(enemy, planet, FALSE);
		}
	}
}

static void handleCollisionBetweenJetmanAndEnemy(Jetman* jetman, Enemy* enemy, Planet planet[static 1]) {

	if (!jetman) {
		return;
	}

	if (!jetman->immunity && overlap(jetman->object.box, enemy->object.box)) {
		killJetman(jetman, planet, TRUE);
		killEnemy(enemy, planet, TRUE);
	}
}

static bool isMissionAccomplished(Planet planet[static 1]) {

	return (planet->spaceship->step == READY) && shareBase(planet->j1->object.box, planet->spaceship->base_object->box);
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
		char bonus_message[22];

		sprintf(bonus_message, "Bonus P1 %03d", ammo_bonus);
		printMessage(bonus_message);
		waitMs(500);

		while (planet->j1->ammo--) {

			ammo_bonus += 10;
			sprintf(bonus_message, "Bonus P1 %03d", ammo_bonus);
			printMessage(bonus_message);
			waitMs(25);
			updateAmmo(P1, planet->j1->ammo);
			VDP_waitVSync();
		}

		flashMessage(bonus_message, 1000);
		planet->j1->ammo = 0;
		scorePoints(ammo_bonus, P1);

		// REFACTOR
		if (planet->j2) {

			VDP_clearTextLine(message_pos.y);

			waitMs(120);

			ammo_bonus = 0;
			sprintf(bonus_message, "Bonus P2 %03d", ammo_bonus);

			waitMs(25);

			while (planet->j2->ammo--) {

				ammo_bonus += 10;
				sprintf(bonus_message, "Bonus P2 %03d", ammo_bonus);
				printMessage(bonus_message);
				waitMs(25);
				updateAmmo(P2, planet->j2->ammo);
				VDP_waitVSync();
			}

			flashMessage(bonus_message, 1000);
			planet->j2->ammo = 0;
			scorePoints(ammo_bonus, P2);
		}
	}
}

static void scorePoints(u16 points, u8 player_id) {

	if (!current_game) {
		return;
	}

	if (player_id == P1) {
		current_game->p1->score += points;

	} else if (player_id == P2) {
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
