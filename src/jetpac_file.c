/*
 * jetpac_file.c
 *
 *  Created on: Apr 06, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../inc/fwk/printer.h"

#define WAIT_MS_BEFORE 	1100
#define WAIT_MS_BETWEEN	750

void printDisclaimer() {

	VDP_setTextPalette(PAL2);

	V2u16 pos = { .x = 0, .y = 3 };

	printerOn();
	waitMs(WAIT_MS_BEFORE);

	print("Video Game Program: JetPac MD", &pos);
	waitMs(WAIT_MS_BETWEEN);

	println("", &pos);
	println("", &pos);
	print("Port by Diego Martin, 2019", &pos);
	waitMs(WAIT_MS_BETWEEN);

	println("", &pos);
	println("", &pos);
	print("Original program written by Tim and Chris Stamper, 1983", &pos);
	waitMs(WAIT_MS_BETWEEN);

	println("", &pos);
	println("", &pos);
	print("JetPac is an Ultimate Play the Game / RARE property", &pos);
	waitMs(WAIT_MS_BETWEEN);

	println("", &pos);
	println("", &pos);
	print("This information priority one", &pos);
	waitMs(WAIT_MS_BETWEEN);

	println("", &pos);
	println("", &pos);
	print("End of Line", &pos);
}

void clearDisclaimer() {

	printerOff();
}
