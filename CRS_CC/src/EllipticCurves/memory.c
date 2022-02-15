/// @file memory.c
#include "memory.h"

/*****************************************
   Short Weierstrass memory management
*****************************************/

/**
  Initializes E for use, with context F, and setting its coefficients to zero.
  A corresponding call to SW_curve_clear() must be made after finishing with the SW_curve_t to free the memory used by the curve.
*/
void SW_curve_init(SW_curve_t *E, const fq_ctx_t *F) {

	fq_init(E->a, *F);
	fq_init(E->b, *F);
}

/**
 Sets E to elliptic curve over F in Weierstrass form with coefficients a and b.
 Curve parameters are given as elements of F.
*/
void SW_curve_set(SW_curve_t *E, const fq_ctx_t *F, const fq_t a, const fq_t b) {

	E->F = F;
	fq_set(E->a, a, *F);
	fq_set(E->b, b, *F);
}

/**
 See SW_curve_set().
 Curve coefficients are given as signed integers.
*/
void SW_curve_set_si(SW_curve_t *E, const fq_ctx_t *F, const slong a, const slong b) {

	fq_t aa, bb;

	fq_init(aa, *F);
	fq_init(bb, *F);

	fq_set_si(aa, a, *F);
	fq_set_si(bb, b, *F);

	SW_curve_set(E, F, aa, bb);
}

/**
 See SW_curve_set().
 Curve coefficients are given as unsigned integers.
*/
void SW_curve_set_su(SW_curve_t *E, const fq_ctx_t *F, const ulong a, const ulong b) {

	fq_t aa, bb;

	fq_init(aa, *F);
	fq_init(bb, *F);

	fq_set_ui(aa, a, *F);
	fq_set_ui(bb, b, *F);

	SW_curve_set(E, F, aa, bb);

	fq_clear(aa, *F);
	fq_clear(bb, *F);
}


/**
 See SW_curve_set().
 Parameters are given as null-terminated strings, in base b. The base b can vary between 2 and 62, inclusive. Returns 0 if the string contain valid inputs and −1 otherwise.
*/
int SW_curve_set_str(SW_curve_t *E, const fq_ctx_t *F, const char *str_a, const char *str_b, int b) {

	fmpz_t fmpz_a, fmpz_b;
	fq_t fq_a, fq_b;
	int ret;

	fmpz_init(fmpz_a);
	fmpz_init(fmpz_b);
	fq_init(fq_a, *F);
	fq_init(fq_b, *F);

	ret = fmpz_set_str(fmpz_a, str_a, b);
	if(ret) return -1;
	ret = fmpz_set_str(fmpz_b, str_b, b);
	if(ret) return -1;

	fq_set_fmpz(fq_a, fmpz_a, *F);
	fq_set_fmpz(fq_b, fmpz_b, *F);

	SW_curve_set(E, F, fq_a, fq_b);

	fmpz_clear(fmpz_a);
	fmpz_clear(fmpz_b);
	fq_clear(fq_a, *F);
	fq_clear(fq_b, *F);

	return 0;
}

/**
  Clears the given curve, releasing any memory used. It must be reinitialised in order to be used again.
*/
void SW_curve_clear(SW_curve_t *E) {

	fq_clear(E->a, *(E->F));
	fq_clear(E->b, *(E->F));
}

/*********************************************
   Short Weierstrass points memory management
**********************************************/

/**
  Initializes P for use, with context F, and setting its coefficients to zero.
  A corresponding call to SW_point_clear() must be made after finishing with the SW_point_t to free the memory used by the curve.
TODO: swap F for E in parameters. A point is member of E not of F.
*/
void SW_point_init(SW_point_t *P, SW_curve_t *E) {

	fq_init(P->x, *(E->F));
	fq_init(P->y, *(E->F));
	fq_init(P->z, *(E->F));

	P->E = E;
}

/**
 Sets P to point of elliptic curve E with coordinates x, y, z.
 Point parameters are given as elements of F.
TODO: Check if P and E's fields are correct
*/
void SW_point_set(SW_point_t *P, const fq_t x, const fq_t y, const fq_t z, SW_curve_t *E) {

	fq_set(P->x, x, *(E->F));
	fq_set(P->y, y, *(E->F));
	fq_set(P->z, z, *(E->F));

	P->E = E;
}

/**
 See SW_point_set().
 Point coordinates are given as signed integers.
*/
void SW_point_set_si(SW_point_t *P, const slong x, const slong y, const slong z, SW_curve_t *E) {

	fq_t xx, yy, zz;

	fq_init(xx, *(E->F));
	fq_init(yy, *(E->F));
	fq_init(zz, *(E->F));

	fq_set_si(xx, x, *(E->F));
	fq_set_si(yy, y, *(E->F));
	fq_set_si(zz, z, *(E->F));

	SW_point_set(P, xx, yy, zz, E);

	fq_clear(xx, *(E->F));
	fq_clear(yy, *(E->F));
	fq_clear(zz, *(E->F));
}

/**
 See SW_point_set().
 Point coordinates are given as unsigned integers.
*/
void SW_point_set_ui(SW_point_t *P, const ulong x, const ulong y, const ulong z, SW_curve_t *E) {

	fq_t xx, yy, zz;

	fq_init(xx, *(E->F));
	fq_init(yy, *(E->F));
	fq_init(zz, *(E->F));

	fq_set_ui(xx, x, *(E->F));
	fq_set_ui(yy, y, *(E->F));
	fq_set_ui(zz, z, *(E->F));

	SW_point_set(P, xx, yy, zz, E);

	fq_clear(xx, *(E->F));
	fq_clear(yy, *(E->F));
	fq_clear(zz, *(E->F));
}

/**
  Clears the given point, releasing any memory used. It must be reinitialised in order to be used again.
*/
void SW_point_clear(SW_point_t *P) {

	fq_clear(P->x, *(P->E->F));
	fq_clear(P->y, *(P->E->F));
	fq_clear(P->z, *(P->E->F));
}


/**************************************
   Montgomery curves memory management
**************************************/

/**
  Initializes E for use, with context F, and setting its coefficients to zero.
  A corresponding call to MG_curve_clear() must be made after finishing with the MG_curve_t to free the memory used by the curve.
*/
void MG_curve_init(MG_curve_t *E, const fq_ctx_t *F) {

	E->F = F;
	fq_init(E->A, *F);
	fq_init(E->B, *F);
}

/**
 Sets elliptic curve rop in Montgomery form to curve op.
 This is aking to a deep copy.
 rop must be initialized.
*/
void MG_curve_set_(MG_curve_t *rop, MG_curve_t *op) {

	rop->F = op->F;
	fq_set(rop->A, op->A, *(op->F));
	fq_set(rop->B, op->B, *(op->F));
}

/**
 Sets E to elliptic curve over F in Montgomery form with coefficients A and B.
 Curve parameters are given as elements of F.
*/
void MG_curve_set(MG_curve_t *E, const fq_ctx_t *F, const fq_t A, const fq_t B) {

	E->F = F;
	fq_set(E->A, A, *F);
	fq_set(E->B, B, *F);
}

/**
 See MG_curve_set().
 Curve coefficients are given as signed integers.
*/
void MG_curve_set_si(MG_curve_t *E, const fq_ctx_t *F, const slong A, const slong B) {

	fq_t AA, BB;

	fq_init(AA, *F);
	fq_init(BB, *F);

	fq_set_si(AA, A, *F);
	fq_set_si(BB, B, *F);

	MG_curve_set(E, F, AA, BB);

	fq_clear(AA, *F);
	fq_clear(BB, *F);
}

/**
 See MG_curve_set().
 Curve coefficients are given as unsigned integers.
*/
void MG_curve_set_ui(MG_curve_t *E, const fq_ctx_t *F, const ulong A, const ulong B) {

	fq_t AA, BB;

	fq_init(AA, *F);
	fq_init(BB, *F);

	fq_set_ui(AA, A, *F);
	fq_set_ui(BB, B, *F);

	MG_curve_set(E, F, AA, BB);

	fq_clear(AA, *F);
	fq_clear(BB, *F);
}

/**
 See MG_curve_set().
 Parameters are given as null-terminated strings, in base b. The base b can vary between 2 and 62, inclusive. Returns 0 if the string contain valid inputs and −1 otherwise.
*/
int MG_curve_set_str(MG_curve_t *E, const fq_ctx_t *F, const char *str_A, const char *str_B, int b) {

	fmpz_t fmpz_A, fmpz_B;
	fq_t fq_A, fq_B;
	int ret;

	fmpz_init(fmpz_A);
	fmpz_init(fmpz_B);
	fq_init(fq_A, *F);
	fq_init(fq_B, *F);

	ret = fmpz_set_str(fmpz_A, str_A, b);
	if(ret) return -1;
	ret = fmpz_set_str(fmpz_B, str_B, b);
	if(ret) return -1;

	fq_set_fmpz(fq_A, fmpz_A, *F);
	fq_set_fmpz(fq_B, fmpz_B, *F);

	MG_curve_set(E, F, fq_A, fq_B);

	fmpz_clear(fmpz_A);
	fmpz_clear(fmpz_B);
	fq_clear(fq_A, *F);
	fq_clear(fq_B, *F);

	return 0;
}

/**
  Clears the given curve, releasing any memory used. It must be reinitialised in order to be used again.
*/
void MG_curve_clear(MG_curve_t *E) {

	fq_clear(E->A, *(E->F));
	fq_clear(E->B, *(E->F));
}

/*********************************************
   Montgomery curve points memory management
**********************************************/

/**
  Initializes P for use, with context F, and setting its coefficients to zero.
  A corresponding call to SW_point_clear() must be made after finishing with the SW_point_t to free the memorZ used bZ the curve.
TODO: swap F for E in parameters. A point is member of E not of F.
*/
void MG_point_init(MG_point_t *P, MG_curve_t *E) {

	fq_init(P->X, *(E->F));
	fq_init(P->Z, *(E->F));

	P->E = E;
}

/**
 Sets P to point of elliptic curve E with coordinates X, Z, z.
 Point parameters are given as elements of F.
TODO: Check if P and E's fields are correct
*/
void MG_point_set(MG_point_t *P, const fq_t X, const fq_t Z, MG_curve_t *E) {

	fq_set(P->X, X, *(E->F));
	fq_set(P->Z, Z, *(E->F));

	P->E = E;
}

/**
   Sets P to the same point as Q.
   */
void MG_point_set_(MG_point_t *P, MG_point_t *Q) {

	fq_set(P->X, Q->X, *(P->E->F));
	fq_set(P->Z, Q->Z, *(P->E->F));
}

/**
 See MG_point_set().
 Point coordinates are given as signed integers.
*/
void MG_point_set_si(MG_point_t *P, const slong X, const slong Z, MG_curve_t *E) {

	fq_t XX, ZZ;

	fq_init(XX, *(E->F));
	fq_init(ZZ, *(E->F));

	fq_set_si(XX, X, *(E->F));
	fq_set_si(ZZ, Z, *(E->F));

	MG_point_set(P, XX, ZZ, E);

	fq_clear(XX, *(E->F));
	fq_clear(ZZ, *(E->F));
}

/**
 See MG_point_set().
 Point coordinates are given as unsigned integers.
*/
void MG_point_set_ui(MG_point_t *P, const ulong X, const ulong Z, MG_curve_t *E) {

	fq_t XX, ZZ;

	fq_init(XX, *(E->F));
	fq_init(ZZ, *(E->F));

	fq_set_ui(XX, X, *(E->F));
	fq_set_ui(ZZ, Z, *(E->F));

	MG_point_set(P, XX, ZZ, E);

	fq_clear(XX, *(E->F));
	fq_clear(ZZ, *(E->F));
}

/**
 See MG_point_set().
 Point coordinates are given as strings of integers.
   */
void MG_point_set_fmpz(MG_point_t *P, const fmpz_t X, const fmpz_t Z, MG_curve_t *E) {

	fq_t XX, ZZ;

	fq_init(XX, *(E->F));
	fq_init(ZZ, *(E->F));

	fq_set_fmpz(XX, X, *(E->F));
	fq_set_fmpz(ZZ, Z, *(E->F));

	MG_point_set(P, XX, ZZ, E);

	fq_clear(XX, *(E->F));
	fq_clear(ZZ, *(E->F));
}

/**
 See MG_point_set().
 Point coordinates are given as strings of integers.
   */
void MG_point_set_str(MG_point_t *P, const char *X, const char *Z, MG_curve_t *E) {

	fmpz_t XX, ZZ;

	fmpz_init(XX);
	fmpz_init(ZZ);

	fmpz_set_str(XX, X, 10);
	fmpz_set_str(ZZ, Z, 10);

	MG_point_set_fmpz(P, XX, ZZ, E);

	fmpz_clear(XX);
	fmpz_clear(ZZ);
}

/**
 Sets P to the point at infinity on the underlying curve.
*/
void MG_point_set_infty(MG_point_t *P) {

	fq_set_ui(P->X, 1, *(P->E->F));
	fq_set_ui(P->Z, 0, *(P->E->F));
}

/**
  Clears the given point, releasing anZ memorZ used. It must be reinitialised in order to be used again.
*/
void MG_point_clear(MG_point_t *P) {

	fq_clear(P->X, *(P->E->F));
	fq_clear(P->Z, *(P->E->F));
}

/**************************************
   Tate normal curves memory management
**************************************/

/**
  Initializes E for use, with context F, and setting its coefficients to zero.
  A corresponding call to TN_curve_clear() must be made after finishing with the TN_curve_t to free the memory used by the curve.
*/
void TN_curve_init(TN_curve_t *E, fmpz_t l, const fq_ctx_t *F) {

	fq_init(E->b, *F);
	fq_init(E->c, *F);
	fmpz_init_set(E->l, l);
}

/**
 Sets E to elliptic curve over F in Tate-normal form with coefficients A and B.
 Curve parameters are given as elements of F.
*/
void TN_curve_set(TN_curve_t *E, const fq_t b, const fq_t c, fmpz_t l, const fq_ctx_t *F) {

	E->F = F;
	fq_set(E->b, b, *F);
	fq_set(E->c, c, *F);
	fmpz_set(E->l, l);
}

/**
  Deep copy
*/
void TN_curve_set_(TN_curve_t *rop, TN_curve_t *op) {

	rop->F = op->F;
	fq_set(rop->b, op->b, *(rop->F));
	fq_set(rop->c, op->c, *(rop->F));
	fmpz_set(rop->l, op->l);
}

/**
 See TN_curve_set().
 Curve coefficients are given as signed integers.
*/
void TN_curve_set_si(TN_curve_t *E, const slong b, const slong c, const slong l, const fq_ctx_t *F) {

	fq_t bb, cc;
	fmpz_t ll;

	fq_init(bb, *F);
	fq_init(cc, *F);
	fmpz_init(ll);

	fq_set_si(bb, b, *F);
	fq_set_si(cc, c, *F);
	fmpz_set_si(ll, l);

	TN_curve_set(E, bb, cc, ll, F);

	fmpz_clear(ll);
	fq_clear(bb, *F);
	fq_clear(cc, *F);
}

/**
 See TN_curve_set().
 Curve coefficients are given as unsigned integers.
*/
void TN_curve_set_ui(TN_curve_t *E, const ulong b, const ulong c, ulong l, const fq_ctx_t *F) {

	fq_t bb, cc;
	fmpz_t ll;

	fq_init(bb, *F);
	fq_init(cc, *F);
	fmpz_init(ll);

	fq_set_ui(bb, b, *F);
	fq_set_ui(cc, c, *F);
	fmpz_set_ui(ll, l);

	TN_curve_set(E, bb, cc, ll, F);

	fmpz_clear(ll);
	fq_clear(bb, *F);
	fq_clear(cc, *F);
}

/**
 See TN_curve_set().
 Parameters are given as null-terminated strings, in base b. The base b can vary between 2 and 62, inclusive. Returns 0 if the string contain valid inputs and −1 otherwise.
*/
int TN_curve_set_str(TN_curve_t *E, const char *str_b, const char *str_c, const char *str_l, int base, const fq_ctx_t *F) {

	fmpz_t fmpz_b, fmpz_c, fmpz_l;
	fq_t fq_b, fq_c;
	int ret;

	fmpz_init(fmpz_b);
	fmpz_init(fmpz_c);
	fmpz_init(fmpz_l);
	fq_init(fq_b, *F);
	fq_init(fq_c, *F);

	ret = fmpz_set_str(fmpz_b, str_c, base);
	if(ret) return -1;
	ret = fmpz_set_str(fmpz_c, str_c, base);
	if(ret) return -1;
	ret = fmpz_set_str(fmpz_l, str_l, base);
	if(ret) return -1;

	fq_set_fmpz(fq_b, fmpz_b, *F);
	fq_set_fmpz(fq_c, fmpz_c, *F);

	TN_curve_set(E, fq_b, fq_c, fmpz_l, F);

	fmpz_clear(fmpz_l);
	fmpz_clear(fmpz_b);
	fmpz_clear(fmpz_c);
	fq_clear(fq_b, *F);
	fq_clear(fq_c, *F);

	return 0;
}

/**
  Clears the given curve, releasing any memory used. It must be reinitialised in order to be used again.
*/
void TN_curve_clear(TN_curve_t *E) {

	fq_clear(E->b, *(E->F));
	fq_clear(E->c, *(E->F));
	fmpz_clear(E->l);
}

