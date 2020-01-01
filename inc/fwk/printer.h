/*
 * printer.h
 *
 *  Created on: Apr 18, 2019
 *      Author: diegomtassis
 */

#ifndef INC_PRINTER_H_
#define INC_PRINTER_H_

#include <genesis.h>

#define WAIT_50			50
#define WAIT_750		750
#define WAIT_1100	 	1100

#define COMMA_WAIT 			500
#define MIDDLE_DOT_WAIT 	900
#define PARAGRAPH_DOT_WAIT 	1250
#define FULL_STOP_WAIT 		3000

void initPrinter();
void turnPrinterOn();
void turnPrinterOff();
void clearScreen();

void print(const char *text);
void println(const char *text);

void printerWait(u32 ms);

#endif /* INC_PRINTER_H_ */
