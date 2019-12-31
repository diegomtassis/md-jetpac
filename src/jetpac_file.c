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

	turnPrinterOn();
	waitMs(WAIT_MS_BEFORE);

	print("Video Game Program: JetPac MD");
	waitMs(WAIT_MS_BETWEEN);

	println("");
	println("");
	println("Port by Diego Martin, 2019-2020");
	print("Work in progress");
	waitMs(WAIT_MS_BETWEEN);

	println("");
	println("");
	print("Original program written by Tim and Chris Stamper, 1983");
	waitMs(WAIT_MS_BETWEEN);

	println("");
	println("");
	print("JetPac is an Ultimate Play the Game / RARE property");
	waitMs(WAIT_MS_BETWEEN);

	println("");
	println("");
	print("This information priority one");
	waitMs(WAIT_MS_BETWEEN);

	println("");
	println("");
	print("End of Line");
}

void clearDisclaimer() {

	turnPrinterOff();
}
