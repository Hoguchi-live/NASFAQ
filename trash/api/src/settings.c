// @file settings.c
#include "settings.h"

void settings_init_(settings_t *op) {

	op->walletIsPublic = 0;
}

settings_t *settings_init() {

	settings_t *op = malloc(sizeof(settings_t));
	settings_init_(op);
	return op;
}

void settings_set(settings_t *op, bool walletIsPublic) {

	op->walletIsPublic = walletIsPublic;
}

settings_t *settings_init_set(bool walletIsPublic) {

	settings_t *op = settings_init();
	settings_set(op, walletIsPublic);
	return op;
}

void settings_clear(settings_t *op) {

	free(op);
}

void settings_print(settings_t *op) {

	printf("SETTINGS\n  Wallet is public: %d\n", op->walletIsPublic);
}

