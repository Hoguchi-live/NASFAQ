#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../../src/EllipticCurves/models.h"
#include "../../src/EllipticCurves/memory.h"
#include "../../src/EllipticCurves/arithmetic.h"
#include "../../src/EllipticCurves/pretty_print.h"

#include "../../src/Polynomials/binary_trees.h"
#include "../../src/Polynomials/roots.h"

#include "../../src/Isogeny/radical.h"
#include "../../src/Isogeny/walk.h"

#include "../../src/Exchange/setup.h"
#include "../../src/Exchange/keygen.h"
#include "../../src/Exchange/dh.h"
#include "../../src/Exchange/info.h"

#include <gmp.h>
#include <flint/fmpz.h>
#include <flint/fq.h>
#include <flint/fq_poly.h>
#include <flint/fq_poly_factor.h>

int main() {

	flint_rand_t state;
	cfg_t *cfg = cfg_init_set();

	const fq_ctx_t *F = (cfg->fields);

	fq_t j_inv_A, j_inv_B;
	MG_curve_t E_A, E_B, E_secret_A, E_secret_B;

	fq_init(j_inv_A, *F);
	fq_init(j_inv_B, *F);
	MG_curve_init(&E_A, F);
	MG_curve_init(&E_B, F);
	MG_curve_init(&E_secret_A, F);
	MG_curve_init(&E_secret_B, F);
	flint_randinit(state);

	//// Config
	#ifndef TIMING
	cfg_print(cfg);
	#endif

	//// Secret keys
	key__t *key_A = keygen_(cfg, 0, state);
	key__t *key_B = keygen_(cfg, 1, state);

	#ifndef TIMING
	printf("Alice's secret key: ");
	key_print(key_A);
	printf("Bob's secret key: ");
	key_print(key_B);
	#endif

	//// Apply secret keys to base curve to get public keys
	#ifndef TIMING
	printf("Computing Alice's public key\n");
	#endif
	int ret_A = apply_key(&E_A, cfg->E, key_A, cfg);
	#ifndef TIMING
	printf("Success: %d\n", ret_A);
	printf("Computing Bob's public key\n");
	int ret_B = apply_key(&E_B, cfg->E, key_B, cfg);
	printf("Success: %d\n", ret_B);


	//// Apply secret key to public keys to get the secret
	printf("Computing Alice's shared secret\n");
	int ret_secret_A = apply_key(&E_secret_A, &E_B, key_A, cfg);
	printf("Success: %d\n", ret_secret_A);
	printf("Computing Bob's shared secret\n");
	int ret_secret_B = apply_key(&E_secret_B, &E_A, key_B, cfg);
	printf("Success: %d\n", ret_secret_B);

	//// Compute secrets' j-invariant to check for validity
	MG_j_invariant(&j_inv_A, &E_secret_A);
	printf("Alice's shared secret's j-invariant: \n");
	fq_print_pretty(j_inv_A, *F);

	MG_j_invariant(&j_inv_B, &E_secret_B);
	printf("\nBob's shared secret's j-invariant: \n");
	fq_print_pretty(j_inv_B, *F);
	#endif


	key_clear(key_A);
	key_clear(key_B);
	cfg_clear(cfg);

	fq_clear(j_inv_A, *F);
	fq_clear(j_inv_B, *F);
	MG_curve_clear(&E_A);
	MG_curve_clear(&E_B);
	MG_curve_clear(&E_secret_A);
	MG_curve_clear(&E_secret_B);
	flint_randclear(state);
}

