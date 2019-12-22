/*
 * types.c
 *
 *  Created on: Dec 22, 2019
 *      Author: diegomtassis
 */

#include "../inc/fwk/types.h"

#include <genesis.h>

s16 list_add(List* list, void* e) {

	if (!list) {
		return -1;
	}

	s16 empty_pos = list_find_empty(list);
	if (empty_pos >= 0) {
		list->e[empty_pos] = e;
		list->count++;
	}

	return empty_pos;
}

s16 list_find_empty(List* list) {

	if (!list) {
		return -1;
	}

	for (int idx = 0; idx < list->size; idx++) {
		if (!list->e[idx]) {
			return idx;
		}
	}

	return -1;
}

s16 list_find(List* list, void* e) {

	if (!list) {
		return -1;
	}

	for (int idx = 0; idx < list->size; idx++) {
		if (list->e[idx] == e) {
			return idx;
		}
	}

	return -1;
}

void list_remove_at(List* list, int pos) {

	if (!list || list->count <= 0 || pos >= list->size || !list->e[pos]) {
		return;
	}

	list->e[pos] = 0;
	list->count--;
}
