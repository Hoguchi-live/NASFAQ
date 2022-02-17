#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "common.h"

typedef struct settings_t {

	bool walletIsPublic;
} settings_t;

settings_t *settings_init();
void settings_set(settings_t *, bool);
settings_t *settings_init_set(bool);
void settings_clear(settings_t *);

void settings_print(settings_t *);
#endif

