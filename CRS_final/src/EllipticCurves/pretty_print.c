#include "pretty_print.h"

/**
  Prints a compact representation of E to stdout.
*/
void SW_curve_print(SW_curve_t *E) {

	printf("Y^3 = X^3 + ");
	fq_print_pretty(E->a, *(E->F));
	printf("X + ");
	fq_print_pretty(E->b, *(E->F));
}


/**
  Prints a representation of E to stdout.
*/
void SW_curve_print_full(SW_curve_t *E) {

	fmpz_t p;
	long d;

	fmpz_init_set(p, fq_ctx_prime(*(E->F)));
	d = fq_ctx_degree(*(E->F));

	printf("Elliptic curve Y^3 = X^3 + ");
	fq_print_pretty(E->a, *(E->F));
	printf("X + ");
	fq_print_pretty(E->b, *(E->F));
	printf(" in short Weierstrass form over F_p^d with p = ");
	fmpz_print(p);
	printf(" and d = %ld\n", d);

	fmpz_clear(p);
}

/**
  Prints a compact representation of P to stdout.
*/
void SW_point_print(SW_point_t *P) {

	printf("[");
	fq_print_pretty(P->x, *(P->E->F));
	printf(", ");
	fq_print_pretty(P->y, *(P->E->F));
	printf(", ");
	fq_print_pretty(P->z, *(P->E->F));
	printf("]");
}

/**
  Prints a representation of P to stdout.
*/
void SW_point_print_full(SW_point_t *P) {

	printf("Point [");
	fq_print_pretty(P->x, *(P->E->F));
	printf(", ");
	fq_print_pretty(P->y, *(P->E->F));
	printf(", ");
	fq_print_pretty(P->z, *(P->E->F));
	printf("] on ");
	SW_curve_print(P->E);
}

/**
  Prints a compact representation of E to stdout.
*/
void MG_curve_print(MG_curve_t *E) {

	fq_print_pretty(E->B, *(E->F));
	printf("Y^3 = X^3 + ");
	fq_print_pretty(E->A, *(E->F));
	printf("X^2 + X");
}

/**
  Prints a representation of E to stdout.
*/
void MG_curve_print_full(MG_curve_t *E) {

	fmpz_t p;
	long d;

	fmpz_init_set(p, fq_ctx_prime(*(E->F)));
	d = fq_ctx_degree(*(E->F));

	printf("Elliptic curve ");
	fq_print_pretty(E->B, *(E->F));
	printf("Y^3 = X^3 + ");
	fq_print_pretty(E->A, *(E->F));
	printf("X^2 + X");
	printf(" in Montgomery form over F_p^d with p = ");
	fmpz_print(p);
	printf(" and d = %ld\n", d);

	fmpz_clear(p);
}

/**
  Prints a compact representation of P to stdout.
*/
void MG_point_print(MG_point_t *P) {

	printf("(");
	fq_print_pretty(P->X, *(P->E->F));
	printf(", ");
	fq_print_pretty(P->Z, *(P->E->F));
	printf(")");
}

/**
  Prints a representation of P to stdout.
*/
void MG_point_print_full(MG_point_t *P) {

	printf("Point (");
	fq_print_pretty(P->X, *(P->E->F));
	printf(", ");
	fq_print_pretty(P->Z, *(P->E->F));
	printf(") on ");
	MG_curve_print(P->E);
}

/**
  Prints a compact representation of E to stdout.
*/
void TN_curve_print(TN_curve_t *E) {

	printf("Y^2 + (1-c)XY -bY = X^3 -bX^2 with \n");
	printf("b = ");
	fq_print_pretty(E->b, *(E->F));
	printf("\nc = ");
	fq_print_pretty(E->c, *(E->F));
}

