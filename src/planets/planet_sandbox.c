/*
 * planet_sandbox.c
 *
 *  Created on: Dec 20, 2025
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../../inc/elements.h"
#include "../../inc/enemies.h"
#include "../../inc/fwk/commons.h"
#include "../../inc/planet.h"
#include "../../inc/planetary_systems.h"
#include "../../inc/spaceship.h"
#include "../../res/zx.h"

#include "../../inc/config/sandbox_config.h"

static EnemyDefinition *defineEnemy();

static void initPlanetSandbox(Planet planet[static 1]);
static void releasePlanetSandbox(Planet planet[static 1]);

Planet *createPlanetSandbox() {

    Planet *planet = LOC_allocPlanet();

    LOC_createDefaultPlatforms(planet);

    EnemyDefinition *enemyDefinition = defineEnemy();

    LOC_defineEnemiesPopulation(planet, *enemyDefinition, sandbox_config.max_enemies);
    LOC_defineSpaceshipInDefaultPlanet(planet, u1Definition, UNASSEMBLED);

    planet->def->p1_init_pos = 0;
    planet->def->ammo = sandbox_config.ammo;
    planet->def->gravity = sandbox_config.gravity;
    planet->def->terminal_velocity = sandbox_config.terminal_velocity;

    planet->def->mind_bottom = FALSE;

    planet->def->planet_init_func = &initPlanetSandbox;
    planet->def->planet_release_func = &releasePlanetSandbox;

    return planet;
}

static EnemyDefinition *defineEnemy() {

    switch (sandbox_config.enemy) {
    case SANDBOX_ENEMY_ALIEN:
        return &alienDefinition;
    case SANDBOX_ENEMY_BUBBLE:
        return &bubbleDefinition;
    case SANDBOX_ENEMY_CROSS:
        return &crossDefinition;
    case SANDBOX_ENEMY_FALCON:
        return &falconDefinition;
    case SANDBOX_ENEMY_FIGHTER:
        return &fighterDefinition;
    case SANDBOX_ENEMY_METEORITE:
        return &meteoriteDefinition;
    case SANDBOX_ENEMY_OWL:
        return &owlDefinition;
    case SANDBOX_ENEMY_SAUCER:
        return &saucerDefinition;
    default:
        return NULL;
    }
}

static void initPlanetSandbox(Planet planet[static 1]) {
    switch (sandbox_config.system) {
    case SANDBOX_SYSTEM_P_CENTAURI:
        initPlanetarySystemPCentauri();
        break;
    case SANDBOX_SYSTEM_RAN:
        initPlanetarySystemRan();
        break;
    case SANDBOX_SYSTEM_LALANDE:
        initPlanetarySystemLalande();
        break;
    default:
        break;
    }
}

static void releasePlanetSandbox(Planet planet[static 1]) {
    switch (sandbox_config.system) {
    case SANDBOX_SYSTEM_P_CENTAURI:
        releasePlanetarySystemPCentauri();
        break;
    case SANDBOX_SYSTEM_RAN:
        releasePlanetarySystemRan();
        break;
    case SANDBOX_SYSTEM_LALANDE:
        releasePlanetarySystemLalande();
        break;
    default:
        break;
    }
}
