/*
 * logger.h
 *
 *  Created on: May 25, 2019
 *      Author: diegomtassis
 */

#ifndef INC_LOGGER_H_
#define INC_LOGGER_H_

#include <genesis.h>

void log_info_xy(const char *message, u16 x, u16 y);
void log_info_x(const char *message, u16 x);

void log_memory();

#endif /* INC_LOGGER_H_ */
