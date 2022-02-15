// @file arithmetic.c
#include "keygen.h"

/**
  Initialize key for use with keygen and cfg.
*/
void key_init(key__t *key, cfg_t *cfg) {

	key->nb_primes = cfg->nb_primes;

	key->lprimes = malloc(sizeof(lprime_t) * key->nb_primes);
	memcpy(key->lprimes, cfg->lprimes, sizeof(lprime_t) * key->nb_primes);

	key->steps = malloc(sizeof(fmpz_t) * key->nb_primes);
	for(int i = 0; i < key->nb_primes; i++) fmpz_init((key->steps)[i]);
}

/**
  Returns an initialized for use with keygen and cfg.
*/
key__t *key_init_(cfg_t *cfg) {

	key__t *key = malloc(sizeof(key__t));
	key_init(key, cfg);
	return key;
}


/**
  Sets key to a curve generated via the config cfg and the given seeds.
*/
void keygen(key__t *key, cfg_t *cfg, uint seed, flint_rand_t state) {

	lprime_t *lp;
	fmpz_t mod;
	fmpz_t steps;

	fmpz_init(mod);
	fmpz_init(steps);
	srand(cfg->seed);
	srand(seed);

	for(int i = 0; i < key->nb_primes; i++) {

		lp = key->lprimes + i;

		// random direction: 0 backward, 1 forward (if available)
		if( (lp->bkw == 0) || (rand() % 2) ) {
			fmpz_set_ui(mod, lp->hbound + 1);
			fmpz_randtest_mod(steps, state, mod);

			// Overwrite the key when timing
			#ifdef TIMING
			fmpz_set_ui(steps, 10);
			#endif

			// These two primes do not work, the reported values might be off in the article
			if(fmpz_equal_ui(lp->l, 947) || fmpz_equal_ui(lp->l, 1723))  fmpz_set_ui(steps, 0);

			fmpz_set((key->steps)[i], steps);

		}
		else {
			fmpz_set_ui(mod, (key->lprimes)[i].hbound + 1);
			fmpz_randtest_mod(steps, state, mod);
			fmpz_neg(steps, steps);

			// Overwrite the key when timing
			#ifdef TIMING
			fmpz_set_ui(steps, 10);
			#endif

			// These two primes do not work, the reported values might be off in the article
			if(fmpz_equal_ui(lp->l, 947) || fmpz_equal_ui(lp->l, 1723))  fmpz_set_ui(steps, 0);

			fmpz_set((key->steps)[i], steps);
		}
	}

	fmpz_clear(mod);
	fmpz_clear(steps);
}

/**
  Wrapper returning an initialized and randomly set key.
*/
key__t *keygen_(cfg_t *cfg, uint seed, flint_rand_t state) {
	key__t *key = key_init_(cfg);
	keygen(key, cfg, seed, state);
	return key;
}


/**
  Clears the given key, releasing any memory used. It must be reinitialised in order to be used again.
*/
void key_clear(key__t *key) {

	//for(int i = 0; i < key->nb_primes; i++) lprime_clear( &(key->lprimes)[i] );
	free(key->lprimes);

	//for(int i = 0; i < key->nb_primes; i++) fmpz_clear( &(key->steps)[i] );
	free(key->steps);
	free(key);
}

/**
  Prints a compact representation of the key to stdout.
*/
void key_print(key__t *key){

	printf("*** Key structure ***\n");
	for(int i = 0; i < key->nb_primes; i++) {
		fmpz_print((key->lprimes)[i].l);
		printf(" ---> ");
		fmpz_print((key->steps)[i]);
		printf("\n");
	}
	printf("*********************\n");
}

