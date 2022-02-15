// @file setup.c
#include "setup.h"

/*********************************************
   l-primes memory management
*********************************************/
/**
  Initializes op for use.
  A corresponding call to lprime_clear() must be made after finishing with the lprime_t to free the memory.
*/
void lprime_init(lprime_t *op){

	fmpz_init(op->l);
}

/**
  Returns a pointer to an initialized lprime_t.
*/
lprime_t *lprime_init_(){

	lprime_t *rop = malloc(sizeof(lprime_t));
	lprime_init(rop);
	return rop;
}

/**
 Sets op to the lprime l with given type (0:unused, 1:radical, 2:velu), bounds, degree and possibility of backward walking.
*/
void lprime_set(lprime_t *op, fmpz_t l, uint type, uint lbound, uint hbound, uint r, uint bkw){

	fmpz_set(op->l, l);

	op->type = type;
	op->lbound = lbound;
	op->hbound = hbound;
	op->r = r;
	op->bkw = bkw;
}

/**
  Clears the given lprime, releasing any memory used. It must be reinitialised in order to be used again.
*/
void lprime_clear(lprime_t *op) {

	fmpz_clear(op->l);
}

/*********************************************
   Config memory management
*********************************************/
/**
  Returns a pointer to a cfg_t config context.
  Every global parameters are hardcoded here.
*/
cfg_t* cfg_init_set() {

	//// Alloc config struct
	cfg_t *cfg = malloc(sizeof(cfg_t));

	//// Extensions of the base field
	//// Alloc fields array
	cfg->fields = (fq_ctx_t *)malloc(sizeof(fq_ctx_t) * MAX_EXTENSION_DEGREE);
	char gen[] = "x";

	//// Initialize extensions
	fmpz_t base_p;
	char base_p_str[] = BASE_p;

	fmpz_init(base_p);
	fmpz_set_str(base_p, base_p_str, 0);

	for(int i=1; i < MAX_EXTENSION_DEGREE + 1; i++) {

		fq_ctx_init( (cfg->fields)[i-1], base_p, i , gen);
	}

	//// Base field shortcut
	fq_ctx_t *F;
	F = &((cfg->fields)[0]);

	//// Base curve parameters
	MG_curve_t *E;

	E = malloc(sizeof(MG_curve_t));

	MG_curve_init(E, F);
	MG_curve_set_str(E, F, BASE_A, BASE_B, 10);

	cfg->E = E;

	//// GLOBAL PROTOCOL PARAMETERS
	//// These bounds are much smaller than what would be used in practice
	//// They are used to produce a working example that does not take too long to run
	//// The actual optimized bounds can be found in file optimization/files/optimized.json
	uint l_PRIMES_int[NB_PRIMES] = {3, 5, 7,     11, 13, 17, 103,     523, 821, 947, 1723,    //degree 1
					19, 661,    // degree 3
					1013, 1181,     // degree 4
					31, 61, 1321, // degree 5
					29, 71, 547, // degree 7
					881, // degree 8
					37, 1693}; // degree 9
	uint l_PRIMES_LBOUNDS[NB_PRIMES] = {1000, 1000, 1000,     100, 100, 100, 100,     100, 100, 100, 100,
					10, 10,
					10, 10,
					10, 10, 10,
					5, 5, 5,
					5,
					5, 5};
	uint l_PRIMES_HBOUNDS[NB_PRIMES] = {1000, 1000, 1000,     100, 100, 100, 100,     100, 100, 100, 100,
					10, 10,
					10, 10,
					10, 10, 10,
					5, 5, 5,
					5,
					5, 5};
	uint l_PRIMES_R[NB_PRIMES] =   {1, 1, 1,     1, 1, 1, 1,     1, 1, 1, 1,
					3, 3,
					4, 4,
					5, 5, 5,
					7, 7, 7,
					8,
					9, 9};
	uint l_PRIMES_BKW[NB_PRIMES] = {1, 1, 1,     1, 1, 1, 1,     0, 0, 0, 0,
					1, 0,
					0, 0,
					1, 1, 0,
					1, 1, 0,
					0,
					1, 0};

	//// Alloc lprimes array
	cfg->lprimes = (lprime_t *)malloc(sizeof(lprime_t) * NB_PRIMES);
	cfg->nb_primes = NB_PRIMES;

	//// Create and set l-primes accordingly
	fmpz_t l_fmpz;
	uint type, lbound, hbound, r, bkw;

	fmpz_init(l_fmpz);

	for(int i=0; i < NB_PRIMES; i++) {

		uint l = l_PRIMES_int[i];
		fmpz_set_ui(l_fmpz, l);

		switch(l) {
			case 3: case 5: case 7:
				type = 1;	//radical isogeny
				lbound = l_PRIMES_LBOUNDS[i];
				hbound = l_PRIMES_HBOUNDS[i];
				r = l_PRIMES_R[i];
				bkw = l_PRIMES_BKW[i];

				break;
			default:
				type = 2;	// Sqrt-Velu
				lbound = l_PRIMES_LBOUNDS[i];
				hbound = l_PRIMES_HBOUNDS[i];
				r = l_PRIMES_R[i];
				bkw = l_PRIMES_BKW[i];
				break;
		}
		lprime_init(&(cfg->lprimes)[i]);
		lprime_set(&(cfg->lprimes)[i], l_fmpz, type, lbound, hbound, r, bkw);
	}


	//// Random seed for key generation
	cfg->seed = 0;

	fmpz_clear(l_fmpz);
	fmpz_clear(base_p);
	return cfg;
}

/**
  Prints a compact representation of the global configuration to stdout.
*/
void cfg_print(cfg_t *cfg) {

	fq_ctx_t *F = &((cfg->fields)[0]);

	printf("*** Config structure ***\n Finite field Fp^d \np = ");
	fmpz_print(fq_ctx_prime(*F));
	printf("\nd = %ld", fq_ctx_degree(*F));
	printf("\n Base elliptic curve BY^2 = X^3 + AX^2 + X\nA = ");
	fq_print_pretty(cfg->E->A, *F);
	printf("\nB = ");
	fq_print_pretty(cfg->E->B, *F);
	printf("\n l-primes \nnb_primes = %d\n", cfg->nb_primes);
	printf("************************\n");
}

/**
  Clears the given configuration, releasing any memory used. It must be reinitialised in order to be used again.
*/
void cfg_clear(cfg_t *op) {

	//// Clear the base curve
	MG_curve_clear(op->E);
	free(op->E);

	//// Clear l-primes and free the array
	for(int i = 0; i < NB_PRIMES; i++) lprime_clear( &(op->lprimes)[i] );
	free(op->lprimes);

	//// Clear the fields and free the array
	for(int i = 0; i < MAX_EXTENSION_DEGREE; i++) fq_ctx_clear( (op->fields)[i] );
	free(op->fields);

	free(op);
}

