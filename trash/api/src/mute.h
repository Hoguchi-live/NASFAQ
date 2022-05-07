#ifndef _MUTE_H_
#define _MUTE_H_

#include "common.h"

#define MUTE_MESSAGE_SIZE 128

typedef struct mute_t {

	bool muted;
	ulong until;
	char message[MUTE_MESSAGE_SIZE];
} mute_t;

mute_t *mute_init();
void mute_set(mute_t *, bool, ulong, char *);
mute_t *mute_init_set(bool, ulong, char *);
void mute_clear(mute_t *);

void mute_print(mute_t *);
#endif

