/*
 * jetpac_file.c
 *
 *  Created on: Apr 06, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../inc/printer.h"

void showDisclaimer() {

	VDP_setTextPalette(PAL2);

	Vect2D_u16 pos = { .x = 0, .y = 3 };

	printerOn();

	println("Video Game Program: JetPac MD", &pos);
	println("", &pos);

	println("Port by Diego Martin, 2019", &pos);
	println("", &pos);

	println("Original program written by Tim and Chris Stamper, 1983", &pos);
	println("", &pos);

	println("JetPac is a Ultimate Play the Game / RARE property", &pos);
	println("", &pos);

	println("This information priority one", &pos);
	println("", &pos);

	println("End of Line", &pos);


}
