/*
 * sandbox_config.h
 *
 * Minimal sandbox configuration API.
 */

#ifndef INC_CONFIG_SANDBOX_CONFIG_H_
#define INC_CONFIG_SANDBOX_CONFIG_H_

#include <genesis.h>

typedef enum {
    SANDBOX_SYSTEM_P_CENTAURI,
    SANDBOX_SYSTEM_RAN,
    SANDBOX_SYSTEM_LALANDE,
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
    u8 system;
    u8 enemy;        
} SandboxConfig;

extern SandboxConfig sandbox_config;

void CONFIG_SANDBOX_init(void);
void CONFIG_SANDBOX_setUp(void);

#endif /* INC_CONFIG_SANDBOX_CONFIG_H_ */
