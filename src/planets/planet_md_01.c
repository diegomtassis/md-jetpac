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
#include "../../res/leynos.h"
#include "../../inc/fwk/vdp_utils.h"

static void createPlatforms(Planet planet[static 1]);
static void defineSpaceship(Planet planet[static 1]);
static void defineJetman(Planet planet[static 1]);

static void initMD01(Planet planet[static 1]);
static void releaseMD01(Planet planet[static 1]);

u16 idx_tile_bg_leynos;

Map *bg;

Planet* createPlanetMD01() {
    Planet* planet = allocPlanet();

    createPlatforms(planet);
    defineEnemiesPopulation(planet, meteoriteDefinition, 7);
    defineSpaceship(planet);
    defineJetman(planet);

    planet->def->mind_bottom = TRUE;
    planet->def->ammo = 25;

    planet->def->planet_init_func = &initMD01;
    planet->def->planet_release_func = &releaseMD01;

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
    planet->def->p1_init_pos = MEM_calloc(sizeof * planet->def->p1_init_pos);
    setV2s16(planet->def->p1_init_pos, 140, 72);
}

static void initMD01(Planet planet[static 1]) {

    PAL_setPalette(PAL2, palette_leynos.data, DMA);
    idx_tile_bg_leynos = loadTile(&tileset_leynos_stars, &idx_tile_malloc);
    bg = MAP_create(&map_leynos_stars, BG_B, TILE_ATTR_FULL(PAL2, FALSE, FALSE, FALSE, idx_tile_bg_leynos));
    MAP_scrollTo(bg, 0, 0);
}

static void releaseMD01(Planet planet[static 1]) {
    PAL_setPalette(PAL2, &palette_black, DMA);
    MEM_free(bg);
    idx_tile_malloc = idx_tile_bg_leynos;
}
