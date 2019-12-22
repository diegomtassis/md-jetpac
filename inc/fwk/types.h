/*
 * types.h
 *
 *  Created on: Nov 27, 2019
 *      Author: diegomtassis
 */

#ifndef INC_FWK_TYPES_H_
#define INC_FWK_TYPES_H_

#include <types.h>

typedef struct List {
	u8 count;
	u8 size;
	void** e;
} List;

s16 list_add(List* list, void* e);
s16 list_find_empty(List* list);
s16 list_find(List* list, void* e);
void list_remove_at(List* list, int idx);

#endif /* INC_FWK_TYPES_H_ */
