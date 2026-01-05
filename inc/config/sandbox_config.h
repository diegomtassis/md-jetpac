/*
 * sandbox_config.h
 *
 * Sandbox configuration API.
 */

#ifndef INC_CONFIG_SANDBOX_CONFIG_H_
#define INC_CONFIG_SANDBOX_CONFIG_H_

#include <genesis.h>

#include "enemy_hostility.h"

typedef enum {
    SANDBOX_SYSTEM_P_CENTAURI,
    SANDBOX_SYSTEM_RAN,
    SANDBOX_SYSTEM_LALANDE,
    SANDBOX_SYSTEM_KEPLER186,
} SandboxSystemOption;

typedef enum {
    SANDBOX_ENEMY_ALIEN,
    SANDBOX_ENEMY_BUBBLE,
    SANDBOX_ENEMY_CROSS,
    SANDBOX_ENEMY_FALCON,
    SANDBOX_ENEMY_FIGHTER,
    SANDBOX_ENEMY_METEORITE,
    SANDBOX_ENEMY_OWL,
    SANDBOX_ENEMY_SAUCER,
} SandboxEnemyOption;

typedef struct SandboxConfig {
    // Game
    u8 lives;
    bool immunity;
    bool limited_ammo;
    bool allow_nuke;
    // Planet
    u8 system;
    f16 gravity;
    u8 enemy;
    EnemyHostility enemy_hostility;
    u8 max_enemies;
    u16 ammo;
} SandboxConfig;

extern SandboxConfig sandbox_config;

void CONFIG_SANDBOX_init(void);
void CONFIG_SANDBOX_setUp(void);

#endif /* INC_CONFIG_SANDBOX_CONFIG_H_ */
