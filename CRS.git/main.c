#include <stdio.h>
#include <stdlib.h>

#include "EllipticCurves/models.h"
#include "EllipticCurves/memory.h"

#include <gmp.h>
#include <flint/fmpz.h>
#include <flint/fq.h>

int main() {

	// Elliptic curve parameters
	char p_str[] = "0x13";
	slong d = 2;
	char a_str[] = "0x1";
	char b_str[] = "0x7";

	fmpz_t p_fmpz, a_fmpz, b_fmpz;
	fmpz_init(p_fmpz);
	fmpz_init(a_fmpz);
	fmpz_init(b_fmpz);

	fmpz_set_str(p_fmpz, p_str, 0);
	fmpz_set_str(a_fmpz, a_str, 0);
	fmpz_set_str(b_fmpz, b_str, 0);

	// Finite field context
	fq_ctx_t F;
	char *Fgen = "g";
	fq_ctx_init(F, p_fmpz, d, Fgen);

	// Base curve
	SW_curve E;
	SW_init(&E, &F);
	SW_set(&E, &F, a_fmpz, b_fmpz);

	// Print base curve
	SW_print(&E);
	//print_test(a_fmpz, b_fmpz, a_fmpz, a_fmpz, b_fmpz);

	return 0;
}
