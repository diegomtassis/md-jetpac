/*
 * printer.h
 *
 *  Created on: Apr 18, 2019
 *      Author: diegomtassis
 */

#ifndef INC_PRINTER_H_
#define INC_PRINTER_H_

#include <genesis.h>

void printerOn();
void printerOff();

void print(const char *text, Vect2D_u16 * offset);
void println(const char *text, Vect2D_u16 * offset);

#endif /* INC_PRINTER_H_ */
