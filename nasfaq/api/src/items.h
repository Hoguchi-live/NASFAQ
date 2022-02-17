#ifndef _ITEMS_H_
#define _ITEMS_H_

#include "common.h"

#define ITEMS_TYPE_SIZE 32

typedef enum {LICENSE, COSMETIC, UNDEFINED} item_category;

typedef struct item_t {

	item_category category;
	char type[ITEMS_TYPE_SIZE];
	ulong acquiredTimestamp;
	float lastPurchasePrice;
	uint quantity;
} item_t;

item_t *item_init();
void item_set(item_t *, item_category, char *, ulong, float, uint);
item_t *item_init_set(item_category, char *, ulong, float, uint);
void item_clear(item_t *);

void item_print(item_t *);

#endif

