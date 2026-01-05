/*
 * main.c
 *
 *  Created on: Apr 06, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../inc/config/game_config.h"
#include "../inc/elements.h"
#include "../inc/fwk/printer.h"
#include "../inc/fwk/vdp_utils.h"
#include "../inc/game.h"
#include "../inc/hud.h"
#include "../inc/jetpac_file.h"
#include "../inc/splash_screen.h"
#include "../res/fonts.h"

// #include "../inc/fwk/logger.h"

#define LOADING_TIME 3000

int main(bool hard) {

    // default resolution
    VDP_setScreenWidth256();
    VDP_setScreenHeight224();
	
    initPrinter();

    if (hard) {

        // jetpac file
        // VDP_setScreenWidth320();
        // printDisclaimer();
        // JOY_waitPress(JOY_1, BUTTON_BTN);
        // clearDisclaimer();
        // VDP_setScreenWidth256();

        // waitMs(50);

        // splash screen
        showSplashScreen();
        waitMs(LOADING_TIME);
        clearSplashScreen();
    }

	VDP_loadFont(font_custom.tileset, DMA);
	PAL_setPalette(PAL1, palette_font.data, DMA);
	VDP_setTextPalette(PAL1);

    resetTileMemory();
    HUD_init();

    CONFIG_GAME_init();

    while (1) {

        // log_memory();
        CONFIG_GAME_setUp();

        GameResult result = GAME_run();

        HUD_registerScore(result.p1_score);
        HUD_registerScore(result.p2_score);

        SYS_doVBlankProcess();
    }

    return 0;
}
