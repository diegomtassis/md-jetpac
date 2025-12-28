/*
 * planet.c
 *
 *  Created on: Dec 7, 2019
 *      Author: diegomtassis
 */

#include "../../inc/planet.h"

#include <genesis.h>

#include "../../inc/collectables.h"
#include "../../inc/enemies.h"
#include "../../inc/explosions.h"
#include "../../inc/fwk/commons.h"
#include "../../inc/physical_constants.h"
#include "../../inc/fwk/vdp_utils.h"
#include "../../inc/jetman.h"
#include "../../inc/shooting.h"
#include "../../inc/spaceship.h"
#include "../../res/zx.h"

static u16 palette[64];

static u16 idx_tile_platform;
static u16 idx_tile_floor;

static void drawPlatforms(VDPPlane plane, const Planet planet[static 1]);
static void drawPlatform(VDPPlane plane, Platform platform[static 1], u16 idx_tile);

static void clearPlatform(VDPPlane plane, Platform platform[static 1]);

void LOC_loadPlanetsBaseResources() {
    // load floor & platform
    idx_tile_floor = loadTile(&floor, &idx_tile_malloc);
    idx_tile_platform = loadTile(&platform, &idx_tile_malloc);
}

void LOC_releasePlanetsBaseResources() {
    // release floor & platform
}

Planet* LOC_allocPlanet() {
    Planet *planet = (Planet *)MEM_calloc(sizeof(Planet));
    planet->def = (PlanetDefinition *)MEM_calloc(sizeof(PlanetDefinition));

    return planet;
}

void LOC_startPlanet(Planet planet[static 1]) {
    SYS_disableInts();

    // initialization
    VDP_clearPlane(BG_B, TRUE);

    PAL_setPalette(PAL0, palette_black, DMA);

    if (planet->def->planet_init_func) {
        planet->def->planet_init_func(planet);
    }

    drawPlatforms(BG_A, planet);

    SYS_enableInts();

    // prepare palette
    memcpy(&palette[0], palette_zx.data, 16 * 2);

    // fade in
    PAL_fadeIn(0, (1 * 16) - 1, palette, 60, FALSE);
}

void LOC_releasePlanet(Planet *planet) {
    if (!planet) {
        return;
    }

    releaseExplosions(planet);
    releaseShots(planet);
    releaseCollectables(planet);
    releaseEnemies(planet);
    releaseJetmen(planet);
    releaseSpaceship(planet);

    // floor
    if (planet->floor) {
        clearPlatform(BG_A, planet->floor);
        LOC_releasePlatform(planet->floor);
        planet->floor = 0;
    }

    // platforms
    if (planet->platforms) {
        for (u8 i = 0; i < planet->num_platforms; i++) {
            clearPlatform(BG_A, planet->platforms[i]);
            LOC_releasePlatform(planet->platforms[i]);
            planet->platforms[i] = 0;
        }
        MEM_free(planet->platforms);
        planet->platforms = 0;
    }

    // jetman definition
    if (planet->def->p1_init_pos) {
        MEM_free(planet->def->p1_init_pos);
        planet->def->p1_init_pos = 0;
    }

    if (planet->def->p2_init_pos) {
        MEM_free(planet->def->p2_init_pos);
        planet->def->p2_init_pos = 0;
    }

    // planet release custom
    if (planet->def->planet_release_func) {
        planet->def->planet_release_func(planet);
    }

    // release definition
    MEM_free(planet->def);

    // release planet
    MEM_free(planet);
}

f16 LOC_landed(Box_s16 subject_box, const Planet* planet) {
    if (hitAbove(&subject_box, &planet->floor->object.box)) {
        return FIX16(adjacentYAbove(&subject_box, &planet->floor->object.box));
    }

    for (u8 i = 0; i < planet->num_platforms; i++) {
        Box_s16 object_box = planet->platforms[i]->object.box;
        if (hitAbove(&subject_box, &object_box)) {
            return FIX16(adjacentYAbove(&subject_box, &object_box));
        }
    }

    return FIX16_0;
}

void LOC_createDefaultPlatforms(Planet planet[static 1]) {
    planet->floor = LOC_createPlatform(0, 25, 32);

    planet->num_platforms = 3;
    planet->platforms = MEM_calloc(planet->num_platforms * sizeof(Platform *));

    planet->platforms[0] = LOC_createPlatform(4, 11, 6);
    planet->platforms[1] = LOC_createPlatform(15, 14, 4);
    planet->platforms[2] = LOC_createPlatform(24, 8, 6);
}

void LOC_defineSpaceshipInDefaultPlanet(Planet planet[static 1], SpaceshipTypeDefinition type_definition, u16 init_step) {
    planet->def->spaceship_def.type_definition = type_definition;
    planet->def->spaceship_def.init_step = init_step;

    if (init_step == UNASSEMBLED) {
        setV2s16(&planet->def->spaceship_def.base_pos, 168, 184);
        setV2s16(&planet->def->spaceship_def.middle_pos, 128, 96);
        setV2s16(&planet->def->spaceship_def.top_pos, 48, 72);
    } else {
        setV2s16(&planet->def->spaceship_def.base_pos, 168, 152);
    }
}

void LOC_defineEnemiesPopulation(Planet planet[static 1], const EnemyDefinition enemyDefinition, u16 size) {
    planet->def->enemies_def.enemy_def = enemyDefinition;
    planet->def->enemies_def.num_enemies = size;
}

void LOC_useEarthGravity(Planet planet[static 1]) {
    if (!planet || !planet->def) {
        return;
    }

    planet->def->gravity = EARTH_GRAVITY;
}

Platform* LOC_createPlatform(u16 pos_x_t, u16 pos_y_t, u16 length_t) {
    Platform *platform = MEM_calloc(sizeof * platform);

    V2u16 pos_t = { .x = pos_x_t, .y = pos_y_t };
    platform->pos_t = pos_t;

    V2u16 size = { .x = length_t, .y = 1 };
    platform->size_t = size;

    V2f16 pos_px = { .x = FIX16(pos_x_t * 8), .y = FIX16(pos_y_t * 8) };
    platform->object.pos = pos_px;

    V2u16 size_px = { .x = length_t * 8, .y = 8 };
    platform->object.size = size_px;

    platform->object.box.w = size_px.x;
    platform->object.box.h = size_px.y;
    updateBox(&platform->object);

    return platform;
}

void LOC_releasePlatform(Platform *platform) { MEM_free(platform); }

static void drawPlatforms(VDPPlane plane, const Planet planet[static 1]) {
    // draw floor
    drawPlatform(plane, planet->floor, idx_tile_floor);

    // draw platforms
    for (u8 i = 0; i < planet->num_platforms; i++) {
        drawPlatform(plane, planet->platforms[i], idx_tile_platform);
    }
}

static void drawPlatform(VDPPlane plane, Platform platform[static 1], u16 idx_tile) {
    // left edge
    VDP_setTileMapXY(plane, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, idx_tile), platform->pos_t.x, platform->pos_t.y);

    // middle section
    VDP_fillTileMapRect(plane, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, idx_tile + 1), platform->pos_t.x + 1,
        platform->pos_t.y, platform->size_t.x - 2, 1);

    // right edge
    VDP_setTileMapXY(plane, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, idx_tile + 2),
        platform->pos_t.x + platform->size_t.x - 1, platform->pos_t.y);
}

static void clearPlatform(VDPPlane plane, Platform platform[static 1]) {

    VDP_clearTileMapRect(plane, platform->pos_t.x, platform->pos_t.y, platform->size_t.x, 1);
}
