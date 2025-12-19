/*
 * sandbox_config.h
 *
 * Minimal sandbox configuration API.
 */

#ifndef INC_CONFIG_SANDBOX_CONFIG_H_
#define INC_CONFIG_SANDBOX_CONFIG_H_

#include <genesis.h>

typedef enum {
	LEYNOS, //
	ANSIMUZ //,
} PlanetCode;

typedef enum {
	ALIEN, //
    BUBBLE, //
    CROSS, //
    FALCON, //
    FIGHTER, //
    METEORITE, //
    OWL, //
    SAUCER, //
} EnemyCode;

typedef struct SandboxConfig {
    u8 planet;
    u8 enemy;        
} SandboxConfig;

extern SandboxConfig sandbox_config;

void CONFIG_SANDBOX_init(void);
void CONFIG_SANDBOX_setUp(void);

#endif /* INC_CONFIG_SANDBOX_CONFIG_H_ */
