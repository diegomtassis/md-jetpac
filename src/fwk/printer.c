/*
 * printer.c
 *
 *  Created on: Apr 18, 2019
 *      Author: diegomtassis
 */

#include "../../inc/fwk/printer.h"
#include "../../res/sprite.h"

#include <genesis.h>

#define WAIT_MS	50

static const V2u16 MIN_PRT_AREA = { .x = 0, .y = 3 };
static const V2u16 MAX_PRT_AREA = { .x = 31, .y = 27 };

static void printChar(const char* text, u16 pos, int is_last, V2u16* offset);

static void moveForward(V2u16* offset);
static void moveToNextLine(V2u16* offset);
static void normalizeOffset(V2u16* offset);

static void cursorOn();
static void cursorOnAt(V2u16* offset);
static void cursorOff();

static u16 tilesToPx(u8);

static Sprite* cursor;

void printerOn() {

	SPR_init(5, 16, 64);
	cursor = SPR_addSprite(&cursor_sprite, tilesToPx(MIN_PRT_AREA.x), tilesToPx(MIN_PRT_AREA.y),
			TILE_ATTR(VDP_getTextPalette(), TRUE, FALSE, FALSE));
	cursorOn();
}

void printerOff() {

	SPR_reset();
	SPR_update();
	VDP_clearPlan(VDP_getTextPlan(), TRUE);
}

void println(const char* text, V2u16* offset) {

	print(text, offset);
	if (offset->x != MIN_PRT_AREA.x) {
		moveToNextLine(offset);
	}
	moveToNextLine(offset);

	cursorOnAt(offset);
}

void print(const char* text, V2u16* offset) {

	normalizeOffset(offset);

	u16 rest = strlen(text);
	u16 current = 0;

	cursorOff();

	while (rest--) {

		printChar(text, current++, rest == 1, offset);
		waitMs(WAIT_MS);
	}

	cursorOnAt(offset);
}

static void printChar(const char* text, u16 pos, int is_last, V2u16* offset) {

	char current = text[pos];
	char str[1];
	str[0] = current;

	char prev = ' ';
	if (pos > 0) {
		prev = text[pos - 1];
	}

	char next = ' ';
	if (!is_last) {
		next = text[pos + 1];
	}

	if (current != ' ') {
		if (offset->x == MAX_PRT_AREA.x && next != ' ') {
			if (prev != ' ') {
				VDP_drawText("-", offset->x, offset->y);
				moveForward(offset);
			} else {
				// first letter of a word in the last position, start in the next line
				moveForward(offset);
			}
		}

		VDP_drawText(str, offset->x, offset->y);
		moveForward(offset);

	} else {
		// ignore spaces in the first pos
		if (offset->x) {
			moveForward(offset);
		}
	}
}

static void normalizeOffset(V2u16* offset) {

	if (offset->x < MIN_PRT_AREA.x) {
		offset->x = MIN_PRT_AREA.x;
	}

	if (offset->y < MIN_PRT_AREA.y) {
		offset->y = MIN_PRT_AREA.y;
	}
}

static void moveForward(V2u16* offset) {

	if (offset->x == MAX_PRT_AREA.x) {
		moveToNextLine(offset);
		moveToNextLine(offset);
	} else {
		offset->x++;
	}

	SPR_setPosition(cursor, offset->x, offset->y);
}

static void moveToNextLine(V2u16* offset) {

	offset->x = MIN_PRT_AREA.x;
	offset->y++;

	SPR_setPosition(cursor, offset->x, offset->y);
}

static void cursorOn() {

	SPR_setVisibility(cursor, VISIBLE);
	SPR_update();
}

static void cursorOnAt(V2u16* offset) {

	SPR_setPosition(cursor, tilesToPx(offset->x), tilesToPx(offset->y));
	cursorOn();
}

static void cursorOff() {

	SPR_setVisibility(cursor, HIDDEN);
	SPR_update();
}

static u16 tilesToPx(u8 tiles) {
	return tiles * 8;
}
