/*
 * array_fixed_list.h
 *
 *  Created on: Dec 23, 2019
 *      Author: diegomtassis
 */

#ifndef INC_FWK_ARRAY_FIXED_LIST_H_
#define INC_FWK_ARRAY_FIXED_LIST_H_

#include <types.h>

typedef struct {
	u8 count;
	u8 size;
	void** e;
} ArrayFixedList;

/**
 * @brief initializes the size and reserves memory for the elements
 *
 * @param list
 * @param size
 */
void arrayFixedListInit(ArrayFixedList* list, u8 size);

/**
 * @brief releases the list, setting count to 0 and releasing the memory for the elements
 *
 * @param list
 */
void arrayFixedListRelease(ArrayFixedList* list);

/**
 * @brief adds an element in the first empty position
 *
 * @param list
 * @param e
 * @return pos where the element has been added, -1 otherwise
 */
s16 arrayFixedListAdd(ArrayFixedList* list, const void* e);

/**
 * @brief removes the element at a given position
 *
 * @param list
 * @param idx
 */
void arrayFixedListRemoveAt(ArrayFixedList* list, u8 idx);

/**
 * @brief looks for the first empty position
 *
 * @param list
 * @return first empty position in case it exists, -1 otherwise
 */
s16 arrayFixedListFindEmpty(ArrayFixedList* list);

/**
 * @brief finds whether a given element exists in the list
 *
 * @param list
 * @param e
 * @return position of the given element, -1 if it does not exist in the list
 */
s16 arrayFixedListFind(ArrayFixedList* list, const void* e);

#endif /* INC_FWK_ARRAY_FIXED_LIST_H_ */
