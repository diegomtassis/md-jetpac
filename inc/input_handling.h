/**
 *  \file input-handling.h
 */

#ifndef _INPUT_HANDLING_H_
#define _INPUT_HANDLING_H_

#include <types.h>


void inputHandler(u16 joy, u16 changed, u16 state);

void logInputButton(u16 joy, u16 button, u16 changed, u16 state, const char *str, u16 x, u16 y);

#endif // _INPUT_HANDLING_H_
