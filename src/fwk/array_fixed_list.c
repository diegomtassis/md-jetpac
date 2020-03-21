/*
 * array_fixed_list.c
 *
 *  Created on: Dec 23, 2019
 *      Author: diegomtassis
 */

#include <genesis.h>

#include "../../inc/fwk/array_fixed_list.h"
#include "../../inc/fwk/commons.h"

void arrayFixedListInit(ArrayFixedList* list, u8 size) {

	if (!list) {
		return;
	}

	list->e = MEM_calloc(sizeof(void*) * size);
	list->size = size;
	list->count = 0;
}

void arrayFixedListRelease(ArrayFixedList* list) {

	if (!list) {
		return;
	}

	memset(list->e, 0, list->size);
	MEM_free(list->e);
	list->e = 0;
	list->count = 0;
	list->size = 0;
}

s16 arrayFixedListAdd(ArrayFixedList* list, const void* e) {

	if (!list) {
		return -1;
	}

	s16 empty_pos = arrayFixedListFindEmpty(list);
	if (empty_pos >= 0) {
		list->e[empty_pos] = e;
		list->count++;
	}

	return empty_pos;
}

s16 arrayFixedListFindEmpty(ArrayFixedList* list) {

	if (!list) {
		return -1;
	}

	for (u8 idx = 0; idx < list->size; idx++) {
		if (!list->e[idx]) {
			return idx;
		}
	}

	return -1;
}

s16 arrayFixedListFind(ArrayFixedList* list, const void* e) {

	if (!list) {
		return -1;
	}

	for (u8 idx = list->size; idx;) {

		if (list->e[--idx] == e) {
			return idx;
		}
	}

	return -1;
}

void arrayFixedListRemoveAt(ArrayFixedList* list, u8 pos) {

	if (!list || list->count <= 0 || pos >= list->size || !list->e[pos]) {
		return;
	}

	list->e[pos] = 0;
	list->count--;
}
