// @file items.c
#include "items.h"

void item_init_(item_t *op) {

	op->category = UNDEFINED;
	strcpy(op->type, "");
	op->acquiredTimestamp = 0;
	op->lastPurchasePrice = 0;
	op->quantity = 0;
}

item_t *item_init() {

	item_t *op = malloc(sizeof(item_t));
	item_init_(op);
	return op;
}

void item_set(item_t *op, item_category op1, char *op2, ulong op3, float op4, uint op5) {


	op->category = op1;
	strcpy(op->type, op2);
	op->acquiredTimestamp = op3;
	op->lastPurchasePrice = op4;
	op->quantity = op5;
}

item_t *item_init_set(item_category op1, char *op2, ulong op3, float op4, uint op5) {

	item_t *op = item_init();
	item_set(op, op1, op2, op3, op4, op5);
	return op;
}

void item_clear(item_t *op) {

	free(op);
}

void item_print(item_t *op) {

	printf("ITEM\n  Category: ");
	switch (op->category) {
		case LICENSE: printf("license "); break;
		case COSMETIC: printf("cosmetic "); break;
		default: printf("undefined ");
	}
	printf("\n  Type: %s", op->type);
	printf("\n  Acquired timestamp: %lu", op->acquiredTimestamp);
	printf("\n  Last purchase price: %f", op->lastPurchasePrice);
	printf("\n  Quantity: %u\n", op->quantity);
}

