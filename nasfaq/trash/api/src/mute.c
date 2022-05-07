// @file mute.c
#include "mute.h"

void mute_init_(mute_t *op) {

	op->muted = 0;
	op->until = 0;
	strcpy(op->message, "");
}

mute_t *mute_init() {

	mute_t *op = malloc(sizeof(mute_t));
	mute_init_(op);
	return op;
}

void mute_set(mute_t *op, bool muted, ulong until, char *message) {

	op->muted = 0;
	op->until = until;
	strcpy(op->message, message);
}

mute_t *mute_init_set(bool muted, ulong until, char *message) {

	mute_t *op = mute_init();
	mute_set(op, muted, until, message);
	return op;
}

void mute_clear(mute_t *op) {

	free(op);
}

void mute_print(mute_t *op) {

	printf("MUTE\n  Muted: %d", op->muted);
	printf("\n  Until: %lu", op->until);
	printf("\n  Message: %s\n", op->message);
}

