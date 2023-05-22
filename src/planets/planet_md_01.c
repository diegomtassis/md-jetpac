/*
 * planet_md_01.c
 *
 *  Created on: May 12, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../../inc/elements.h"
#include "../../inc/enemies.h"
#include "../../inc/fwk/commons.h"
#include "../../inc/planet.h"
#include "../../inc/spaceship.h"
#include "../../res/sprite.h"

static void createPlatforms(Planet planet[static 1]);
static void defineSpaceship(Planet planet[static 1]);
static void defineJetman(Planet planet[static 1]);

static void initMD01(Planet planet[static 1]);
static void releaseMD01(Planet planet[static 1]);

Planet* createPlanetMD01() {
    Planet* planet = allocPlanet();

    createPlatforms(planet);
    defineEnemiesPopulation(planet, meteoriteDefinition, 7);
    defineSpaceship(planet);
    defineJetman(planet);

    planet->def->mind_bottom = TRUE;
    planet->def->ammo = 25;

    return planet;
}

static void createPlatforms(Planet planet[static 1]) {
    planet->floor = createPlatform(3, 16, 9);

    planet->num_platforms = 3;
    planet->platforms = MEM_calloc(planet->num_platforms * sizeof(Platform*));

    planet->platforms[0] = createPlatform(22, 10, 5);
    planet->platforms[1] = createPlatform(22, 16, 5);
    planet->platforms[2] = createPlatform(22, 22, 5);
}

static void defineSpaceship(Planet planet[static 1]) {
    planet->def->spaceship_def.type_definition = u1Definition;
    planet->def->spaceship_def.init_step = UNASSEMBLED;
    setV2s16(&planet->def->spaceship_def.base_pos, 52, 112);
    setV2s16(&planet->def->spaceship_def.middle_pos, 188, 64);
    setV2s16(&planet->def->spaceship_def.top_pos, 188, 160);
}

static void defineJetman(Planet planet[static 1]) {
    planet->def->p1_init_pos = MEM_calloc(sizeof *planet->def->p1_init_pos);
    setV2s16(planet->def->p1_init_pos, 140, 72);
}

static void initMD01(Planet planet[static 1]) {}

static void releaseMD01(Planet planet[static 1]) {}
