#ifndef _KEYGEN_H_
#define _KEYGEN_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "setup.h"

#include <gmp.h>
#include <flint/fmpz.h>
#include <flint/fq.h>

typedef struct key__t {

	uint nb_primes;
	lprime_t *lprimes;
	fmpz_t *steps;
} key__t;

void key_init(key__t *, cfg_t *);
key__t *key_init_(cfg_t *);
void keygen(key__t *, cfg_t *, uint, flint_rand_t);
key__t *keygen_(cfg_t *, uint, flint_rand_t);
void key_clear(key__t *);

void key_print(key__t *);
#endif

