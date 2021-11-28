#include <stdio.h>
#include <stdlib.h>

#include "memory.h"
#include "models.h"

#include <gmp.h>
#include <flint/fmpz.h>
#include <flint/fq.h>

void SW_init(SW_curve *E, const fq_ctx_t *F) {

	//fmpz_init(E->p);
	fq_init(E->a, *F);
	fq_init(E->b, *F);
}

void SW_set(SW_curve *E, const fq_ctx_t *F, const fmpz_t a, const fmpz_t b) {

	fmpz_t p;
	long d;

	fmpz_init_set(p, fq_ctx_prime(*F));
	d = fq_ctx_degree(*F);

	E->F = F;
	fq_set_fmpz(E->a, a, *F);
	fq_set_fmpz(E->b, b, *F);

	fmpz_clear(p);
}

void SW_print(SW_curve *E) {

	printf("Elliptic curve Y^3 = X^3 + ");
	fq_print_pretty(E->a, *(E->F));
	printf("X + ");
	fq_print_pretty(E->b, *(E->F));
	printf(" in short Weierstrass form over F_p^d with p = ");
	//fmpz_print(E->p);
	//printf(" and d = %ld", E->d);
}

//#define template_weierstrass(a1, a2, a3, a4, a6) const fmpz_t a1, const fmpz_t a2, const fmpz_t a3, const fmpz_t a4, const fmpz_t a6
//void print_test(template_weierstrass(a1, a2, a3, a4, a6)) {
//	printf("\n");
//	fmpz_print(a1);
//	printf("\n");
//	fmpz_print(a2);
//	printf("\n");
//	fmpz_print(a3);
//	printf("\n");
//	fmpz_print(a4);
//	printf("\n");
//	fmpz_print(a6);
//	printf("\n");
//}
