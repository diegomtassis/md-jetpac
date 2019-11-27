/*
 * types.h
 *
 *  Created on: Nov 27, 2019
 *      Author: diegomtassis
 */

#ifndef INC_FWK_TYPES_H_
#define INC_FWK_TYPES_H_

typedef struct List {
	u8 count;
	u8 size;
	void** e;
} List;

#endif /* INC_FWK_TYPES_H_ */
