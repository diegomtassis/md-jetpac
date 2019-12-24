/*
 * lists.h
 *
 *  Created on: Dec 23, 2019
 *      Author: diegomtassis
 */

#ifndef INC_FWK_LISTS_H_
#define INC_FWK_LISTS_H_

#include <types.h>

typedef struct List {
	u8 count;
	u8 size;
	void** e;
} FixedList;

void fixedlist_init(FixedList* list, u8 size);
void fixedlist_release(FixedList* list);
s16 fixedlist_add(FixedList* list, void* e);
void fixedlist_remove_at(FixedList* list, int idx);
s16 fixedlist_find_empty(FixedList* list);
s16 fixedlist_find(FixedList* list, void* e);

#endif /* INC_FWK_LISTS_H_ */
