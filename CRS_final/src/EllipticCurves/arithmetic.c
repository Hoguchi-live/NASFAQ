// @file arithmetic.c
#include "arithmetic.h"

/**
  Embed elliptic curve E's base field in F_p^r if possible
  rop must be initialized over L.
*/
void MG_curve_update_field(MG_curve_t *rop, MG_curve_t *op, const fq_ctx_t *L) {

	char *str_A, *str_B;

	//// Get string representation (with generator "x") of curve parameters
	str_A = fq_get_str_pretty(op->A, *(op->F));
	str_B = fq_get_str_pretty(op->B, *(op->F));

	//// Try to embed A and B in L
	MG_curve_set_str(rop, L, str_A, str_B, 10);
}

/**
  Inline version of the function MG_curve_update_field.
*/
void MG_curve_update_field_(MG_curve_t *op, const fq_ctx_t *L) {

	//// Copy op curve
	MG_curve_t tmp;
	MG_curve_init(&tmp, op->F);
	MG_curve_set_(&tmp, op);

	fq_clear(op->A, *(op->F));
	fq_clear(op->B, *(op->F));

	fq_init(op->A, *L);
	fq_init(op->B, *L);

	MG_curve_update_field(op, &tmp, L);

	MG_curve_clear(&tmp);
}

/**
  Sets output to the j-invariant of short Weierstrass curve E.
*/
void SW_j_invariant(fq_t *output, SW_curve_t *E) {

	fq_t tmp1, tmp2, tmp3;				// temporary registers
	fq_t delta, j_invariant;	 		// discriminant, j-invariant

	fq_init(tmp1, *(E->F));
	fq_init(tmp2, *(E->F));
	fq_init(tmp3, *(E->F));

	fq_init(delta, *(E->F));
	fq_init(j_invariant, *(E->F));

	// compute delta
	fq_pow_ui(tmp1, E->a, 3, *(E->F));
	fq_mul_si(tmp1, tmp1, 4, *(E->F));

	fq_pow_ui(tmp2, E->b,  2, *(E->F));
	fq_mul_si(tmp2, tmp2, 27, *(E->F));

	fq_add(delta, tmp1, tmp2, *(E->F));
	fq_mul_si(delta, delta, -16, *(E->F));

	// compute j
	fq_mul_si(tmp1, tmp1, 4, *(E->F));
	fq_mul_si(tmp1, tmp1, 4, *(E->F));
	fq_mul_si(j_invariant, tmp1, -1728, *(E->F));
	fq_inv(tmp3, delta, *(E->F));
	fq_mul(j_invariant, j_invariant, tmp3, *(E->F));

	// set output
	fq_set(*output, j_invariant, *(E->F));

	// clear memory
	fq_clear(tmp1, *(E->F));
	fq_clear(tmp2, *(E->F));
	fq_clear(tmp3, *(E->F));
	fq_clear(delta, *(E->F));
	fq_clear(j_invariant, *(E->F));
}

/**
  Sets output to the j-invariant of Montgomery curve E.
*/
void MG_j_invariant(fq_t *output, MG_curve_t *E) {

	if(fq_is_zero(E->A, *(E->F)) && fq_is_zero(E->B, *(E->F))) return;

	fq_t tmp1, tmp2;				// temporary registers
	fq_t j_invariant;	 		// discriminant, j-invariant

	fq_init(tmp1, *(E->F));
	fq_init(tmp2, *(E->F));
	fq_init(j_invariant, *(E->F));

	// numerator
	fq_pow_ui(tmp1, E->A, 2, *(E->F));
	fq_sub_ui(tmp1, tmp1, 3, *(E->F));
	fq_pow_ui(tmp1, tmp1, 3, *(E->F));
	fq_mul_ui(tmp1, tmp1, 256, *(E->F));

	// denominator
	fq_pow_ui(tmp2, E->A, 2, *(E->F));
	fq_sub_ui(tmp2, tmp2, 4, *(E->F));

	fq_div(j_invariant, tmp1, tmp2, *(E->F));

	// set output
	fq_set(*output, j_invariant, *(E->F));

	// clear memory
	fq_clear(tmp1, *(E->F));
	fq_clear(tmp2, *(E->F));
	fq_clear(j_invariant, *(E->F));
}

/**
  Sets output to the j-invariant of Tate normal curve E.
*/
void TN_j_invariant(fq_t *rop, TN_curve_t *E){

	int flag_3 = fmpz_equal_ui(E->l, 3);
	fq_t b2, b4, b6, b8, c4, delta, tmp1, tmp2, j_invariant;

	const fq_ctx_t *F = E->F;
	fq_init(b2, *F);
	fq_init(b4, *F);
	fq_init(b6, *F);
	fq_init(b8, *F);
	fq_init(c4, *F);
	fq_init(tmp1, *F);
	fq_init(tmp2, *F);
	fq_init(delta, *F);
	fq_init(j_invariant, *F);

	// Set tmp1 = (c-1) = -a1
	fq_sub_ui(tmp1, E->c, 1, *F);

	// Set b2 = a1^2 - 4b OR a1^2 when l = 3
	fq_pow_ui(b2, tmp1, 2, *F);
	if(!flag_3) {
		fq_mul_ui(tmp2, E->b, 4, *F);
		fq_sub(b2, b2, tmp2, *F);
	}

	// Set b4 = (c-1)b
	fq_mul(b4, tmp1, E->b, *F);

	// Set b6 = b^2
	fq_pow_ui(b6, E->b, 2, *F);

	// Set b8 = -b^3 or 0 when l = 3
	if(!flag_3) {
		fq_pow_ui(b8, E->b, 3, *F);
		fq_neg(b8, b8, *F);
	}

	// Set c4 = b2^2 - 24b4
	fq_pow_ui(c4, b2, 2, *F);
	fq_mul_ui(tmp1, b4, 24, *F);
	fq_sub(c4, c4, tmp1, *F);

	// Set delta = -b2^2*b8 - 8b^4 - 27 b6^2 + 9b2*b4*b6
	fq_mul_ui(delta, b2, 9, *F);
	fq_mul(delta, delta, b4, *F);
	fq_mul(delta, delta, b6, *F);

	if(!flag_3) {
		fq_pow_ui(tmp2, b2, 2, *F);
		fq_mul(tmp2, tmp2, b8, *F);
		fq_sub(delta, delta, tmp2, *F);
	}

	fq_pow_ui(tmp1, b6, 2, *F);
	fq_mul_ui(tmp1, tmp1, 27, *F);
	fq_sub(delta, delta, tmp1, *F);

	fq_pow_ui(tmp1, b4, 3, *F);
	fq_mul_ui(tmp1, tmp1, 8, *F);
	fq_sub(delta, delta, tmp1, *F);

	// Set j = c4^3 / delta
	fq_pow_ui(j_invariant, c4, 3, *F);
	fq_inv(tmp1, delta, *F);
	fq_mul(j_invariant, j_invariant, tmp1, *F);

	fq_set(*rop, j_invariant, *F);

	fq_clear(tmp1, *(E->F));
	fq_clear(tmp2, *(E->F));
	fq_clear(j_invariant, *(E->F));
	fq_clear(delta, *(E->F));
	fq_clear(c4, *F);
	fq_clear(b2, *F);
	fq_clear(b4, *F);
	fq_clear(b6, *F);
	fq_clear(b8, *F);
}

/*************************************
  Points on elliptic curves
*************************************/
/**
  Sets output to 1 if point P=[0, 1, 0] and 0 otherwise.
  Warning: this does not check whether P is a valid projective point or not.
*/
void SW_point_isinfinity(bool *output, SW_point_t *P) {

	*output = fq_is_zero(P->x, *(P->E->F)) && fq_is_zero(P->z, *(P->E->F));
}

/**
  Sets output to 1 if point P=(1, 0) and 0 otherwise.
  Warning: this does not check whether P is a valid projective point or not.
*/
void MG_point_isinfinity(bool *output, MG_point_t *P) {

	*output = fq_is_zero(P->Z, *(P->E->F));
}

/**
  Returns -1 if P is not a projective point.
  If it is, sets output to 1 if point P belongs to the underlying curve and 0 otherwise.
  Finally returns 0.
*/
int SW_point_isvalid(bool *output, SW_point_t *P) {

	// Check if P is projective
	if(fq_is_zero(P->x, *(P->E->F)) && fq_is_zero(P->y, *(P->E->F)) && fq_is_zero(P->z, *(P->E->F))) return -1;

	const fq_ctx_t *F;
	fq_t res, tmp1, tmp2, tmp3, tmp4;

	F = P->E->F;
	fq_init(res, *F);
	fq_init(tmp1, *F);
	fq_init(tmp2, *F);
	fq_init(tmp3, *F);
	fq_init(tmp4, *F);

	// Y^2 * Z
	fq_pow_ui(tmp1, P->y, 2, *F);
	fq_mul(tmp1, tmp1, P->z, *F);
	// X^3
	fq_pow_ui(tmp2, P->x, 3, *F);
	// aX*Z^2
	fq_pow_ui(tmp3, P->z, 2, *F);
	fq_mul(tmp3, tmp3, P->x, *F);
	fq_mul(tmp3, tmp3, P->E->a, *F);
	// bZ^3
	fq_pow_ui(tmp4, P->z, 3, *F);
	fq_mul(tmp4, tmp4, P->E->b, *F);

	fq_add(res, tmp2, tmp3, *F);
	fq_add(res, res, tmp4, *F);
	fq_sub(res, res, tmp1, *F);

	*output = fq_is_zero(res, *F);

	// clear
	fq_clear(res, *F);
	fq_clear(tmp1, *F);
	fq_clear(tmp2, *F);
	fq_clear(tmp3, *F);
	fq_clear(tmp4, *F);

	return 0;
}

/**
TODO: try by square root to determine if point is in predefined extension.
  Returns -1 if P is not a projective point.
  If it is, sets output to 1 if point P belongs to the underlying curve and 0 otherwise.
  Finally returns 0.
*/
int MG_point_isvalid(bool *output, MG_point_t *P) {

	// Check if P is projective
	if(fq_is_zero(P->X, *(P->E->F)) && fq_is_zero(P->Z, *(P->E->F))) return -1;
	return 0;
}


/**
  Sets output to 1 if P is the point at infinity, 0 otherwise.
*/
void MG_point_isinfty(bool *output, MG_point_t *P) {

	*output = fq_is_zero(P->Z, *(P->E->F));
}

/**
  Returns 1 if P is the point at infinity, 0 otherwise.
*/
bool MG_point_isinfty_(MG_point_t *P) {

	return fq_is_zero(P->Z, *(P->E->F));
}

/**
  Normalizes point coordinate to (X/Z, 1) or (1, 0) if P is at infinity.
*/
void MG_point_normalize(MG_point_t *P) {

	if(!fq_is_zero(P->Z, *(P->E->F))) {
		fq_div(P->X, P->X, P->Z, *(P->E->F));
		fq_one(P->Z, *(P->E->F));
	}
	else {
		fq_set_ui(P->X, 1, *(P->E->F));
	}
}

/**
  Returns a non-infinity random point on the underlying curve.
  P must be initialized.
*/
void SW_point_rand_ninfty(SW_point_t *P) {

	fq_t x, y, tmp1, tmp2;
	flint_rand_t state;

	const fq_ctx_t *F = P->E->F;

	fq_init(x, *F);
	fq_init(y, *F);
	fq_init(tmp1, *F);
	fq_init(tmp2, *F);
	flint_randinit(state);

	// Main loop
	int ret = 0;
	while(ret == 0) {
		// Find random x in base field
		fq_randtest(x, state, *F);

		// Compute T := X^3 + aX + b
		fq_pow_ui(tmp1, x, 3, *F);
		fq_mul(tmp2, P->E->a, x, *F);
		fq_add(tmp2, tmp2, P->E->b, *F);
		fq_add(tmp1, tmp1, tmp2, *F);

		// Extract root if exists, otherwise fail with 0.
		ret = fq_sqr_from_polyfact(y, tmp1, *F);
	}

	// Create corresponding SW_point_t, is not infinity
	fq_set(P->x, x, *F);
	fq_set(P->y, y, *F);
	fq_set_ui(P->z, 1, *F);

	flint_randclear(state);
	fq_clear(tmp2, *F);
	fq_clear(tmp1, *F);
	fq_clear(y, *F);
	fq_clear(x, *F);
}

/**
  Returns a non-infinity random point on the underlying curve.
  P must be initialized.
*/
void MG_point_rand_ninfty(MG_point_t *P, flint_rand_t state) {

	fq_t X, Y, tmp1, tmp2;

	const fq_ctx_t *F = P->E->F;

	fq_init(X, *F);
	fq_init(Y, *F);
	fq_init(tmp1, *F);
	fq_init(tmp2, *F);

	// Main loop looking for a x such that x^3 + Ax^2 + x is a square
	int ret = 0;
	while(ret == 0) {
		// Find random x in base field
		fq_randtest(X, state, *F);

		// Compute T := B^-1 * x * (x^2 + Ax + 1)
		fq_pow_ui(tmp1, X, 2, *F);
		fq_mul(tmp2, P->E->A, X, *F);
		fq_add_ui(tmp2, tmp2, 1, *F);
		fq_add(tmp1, tmp1, tmp2, *F);
		fq_mul(tmp1, tmp1, X, *F);

		fq_inv(tmp2, P->E->B, *F);
		fq_mul(tmp1, tmp1, tmp2, *F);

		// Extract root if exists, otherwise fail with 0.
		ret = fq_sqr_from_polyfact(tmp2, tmp1, *F);
	}
	// Create corresponding MG_point_t, is not infinity
	fq_set(P->X, X, *F);
	fq_set_ui(P->Z, 1, *F);

	fq_clear(tmp2, *F);
	fq_clear(tmp1, *F);
	fq_clear(Y, *F);
	fq_clear(X, *F);
}

/**
  Same as MG_point_rand_ninfty but forcing y to be in Fq^2 \ Fq.
  The only thing that changes in the algorithm is
  	ret == 0 ---> ret == 1
  as we want a non-square this time.
  **/
void MG_point_rand_ninfty_nsquare(MG_point_t *P, flint_rand_t state) {

	fq_t X, Y, tmp1, tmp2;

	const fq_ctx_t *F = P->E->F;

	fq_init(X, *F);
	fq_init(Y, *F);
	fq_init(tmp1, *F);
	fq_init(tmp2, *F);

	// Main loop looking for a x such that x^3 + Ax^2 + x is a non-square
	int ret = 1;
	while(ret == 1) {
		// Find random x in base field
		fq_randtest(X, state, *F);

		// Compute T := B^-1 * x * (x^2 + Ax + 1)
		fq_pow_ui(tmp1, X, 2, *F);
		fq_mul(tmp2, P->E->A, X, *F);
		fq_add_ui(tmp2, tmp2, 1, *F);
		fq_add(tmp1, tmp1, tmp2, *F);
		fq_mul(tmp1, tmp1, X, *F);

		fq_inv(tmp2, P->E->B, *F);
		fq_mul(tmp1, tmp1, tmp2, *F);

		// Extract root if exists, otherwise fail with 0.
		ret = fq_sqr_from_polyfact(tmp2, tmp1, *F);
	}
	// Create corresponding MG_point_t, is not infinity
	fq_set(P->X, X, *F);
	fq_set_ui(P->Z, 1, *F);

	fq_clear(tmp2, *F);
	fq_clear(tmp1, *F);
	fq_clear(Y, *F);
	fq_clear(X, *F);
}

/******************************
  Montgomery Arithmetics
******************************/
/**
  Try to set B to 1, return 1 if successful, 0 otherwise.
  This is possible if and only if B admits a square root in the base field.
**/
int MG_curve_normalize(MG_curve_t *E){
	fq_t tmp;
	fq_init(tmp, *(E->F));

	int ret = fq_sqr_from_polyfact(tmp, E->B, *(E->F));
	if(ret == 1) fq_set_ui(E->B, 1, *(E->F));

	fq_clear(tmp, *(E->F));
	return ret;
}

/**
  Bruteforce recovery of a possible y-coordinate of P into rop.
  Returns 1 if successful, 0 otherwise.
*/
int MG_rec_y(fq_t rop, MG_point_t *P){

	const fq_ctx_t *F = P->E->F;

	// normalize P and check if equals O
	MG_point_normalize(P);
	if(MG_point_isinfty_(P)){
		fq_set_ui(rop, 1, *F);
		return 1;
	}

	fq_t tmp1, tmp2;
	fq_init(tmp1, *F);
	fq_init(tmp2, *F);

	// Get B^-1 * x(x^2 + Ax + 1)
	fq_pow_ui(tmp1, P->X, 2, *F);
	fq_mul(tmp2, P->E->A, P->X, *F);
	fq_add_ui(tmp2, tmp2, 1, *F);
	fq_add(tmp1, tmp1, tmp2, *F);
	fq_mul(tmp1, tmp1, P->X, *F);

	fq_inv(tmp2, P->E->B, *F);
	fq_mul(tmp1, tmp1, tmp2, *F);

	int ret = fq_sqr_from_polyfact(rop, tmp1, *F);

	fq_clear(tmp2, *F);
	fq_clear(tmp1, *F);

	return ret;
}

/**
   Sets output to P+Q if D = P-Q
   output must be initialized.
*/
void MG_xADD(MG_point_t *output, MG_point_t P, MG_point_t Q, MG_point_t D) {

	const fq_ctx_t *F;
	F = (P.E)->F;

	fq_t v0, v1, v2, v3;
	fq_init(v0, *F);
	fq_init(v1, *F);
	fq_init(v2, *F);
	fq_init(v3, *F);

	fq_add(v0, P.X, P.Z, *F);
	fq_sub(v1, Q.X, Q.Z, *F);
	fq_mul(v1, v1, v0, *F);
	fq_sub(v0, P.X, P.Z, *F);
	fq_add(v2, Q.X, Q.Z, *F);
	fq_mul(v2, v2, v0, *F);
	fq_add(v3, v1, v2, *F);
	fq_sqr(v3, v3, *F);
	fq_sub(v1, v1, v2, *F);
	fq_sqr(v1, v1, *F);

	fq_mul(output->X, D.Z, v3, *F);
	fq_mul(output->Z, D.X, v1, *F);

	// clear memory
	fq_clear(v0, *F);
	fq_clear(v1, *F);
	fq_clear(v2, *F);
	fq_clear(v3, *F);
}

/**
   Sets output to 2 times P
   output must be initialized.
*/
void MG_xDBL(MG_point_t *output, MG_point_t P) {

	const fq_ctx_t *F;
	F = (P.E)->F;

	// Buffers
	fq_t v1, v2, v3;
	fq_init(v1, *F);
	fq_init(v2, *F);
	fq_init(v3, *F);

	// set v3 = (A+2)/4
	fq_add_ui(v3, (P.E)->A, 2, *F);
	fq_div_ui(v3, v3, 4, *F);

	fq_add(v1, P.X, P.Z, *F);
	fq_sqr(v1, v1, *F);
	fq_sub(v2, P.X, P.Z, *F);
	fq_sqr(v2, v2, *F);
	fq_mul(output->X, v1, v2, *F);
	fq_sub(v1, v1, v2, *F);
	fq_mul(v3, v3, v1, *F);
	fq_add(v3, v3, v2, *F);
	fq_mul(output->Z, v1, v3, *F);

	// clear memory
	fq_clear(v1, *F);
	fq_clear(v2, *F);
	fq_clear(v3, *F);
}

/**
   Same as MG_xDBL but the curve specific doubling constant (A+2)/4 needs to be precomputed
   output must be initialized.
*/
void MG_xDBL_const(MG_point_t *output, MG_point_t P,const fq_t dbl_const) {

	const fq_ctx_t *F;
	F = (P.E)->F;

	// Buffers
	fq_t v1, v2, v3;
	fq_init(v1, *F);
	fq_init(v2, *F);
	fq_init(v3, *F);

	fq_add(v1, P.X, P.Z, *F);
	fq_sqr(v1, v1, *F);
	fq_sub(v2, P.X, P.Z, *F);
	fq_sqr(v2, v2, *F);
	fq_mul(output->X, v1, v2, *F);
	fq_sub(v1, v1, v2, *F);
	fq_mul(v3, dbl_const, v1, *F);
	fq_add(v3, v3, v2, *F);
	fq_mul(output->Z, v1, v3, *F);

	// clear memory
	fq_clear(v1, *F);
	fq_clear(v2, *F);
	fq_clear(v3, *F);
}

/**
   Sets rop to the k times *op using the montgomery ladder double-and-add type procedure.
   rop must be initialized.
*/
void MG_ladder_iter_(MG_point_t *rop, fmpz_t k, MG_point_t *op) {
	// Check if k <0
	//TODO

	MG_curve_t *E = op->E;
	const fq_ctx_t *F = E->F;

	// Check if k = 0
	if(fmpz_is_zero(k)){
		fq_one(rop->X, *F);
		fq_zero(rop->Z, *F);
		return;
	}

	// Check if P = O
	bool isinfty;
	MG_point_isinfty(&isinfty, op);
	if(isinfty) return;

	// Set the doubling constant
	fq_t dbl_cst;
	fq_init(dbl_cst, *F);
	fq_add_ui(dbl_cst, E->A, 2, *F);
	fq_div_ui(dbl_cst, dbl_cst, 4, *F);

	// Buffers
	MG_point_t X0, X1;

	MG_point_init(&X0, E);
	MG_point_init(&X1, E);

	fq_set(X0.X, op->X, *F);
	fq_set(X0.Z, op->Z, *F);
	MG_xDBL_const(&X1, *op, dbl_cst);

	int l;
	l = fmpz_sizeinbase(k, 2);

	for (int i = l-2; i>=0; i--) {
		if (fmpz_tstbit(k, i)) {
			MG_xADD(&X0, X0, X1, *op);
			MG_xDBL_const(&X1, X1, dbl_cst);
		}
		else {
			MG_xADD(&X1, X0, X1, *op);
			MG_xDBL_const(&X0, X0, dbl_cst);
		}
	}

	fq_set(rop->X, X0.X, *F);
	fq_set(rop->Z, X0.Z, *F);

	MG_point_clear(&X0);
	MG_point_clear(&X1);
	fq_clear(dbl_cst, *F);
}

/**
   Sets rop to the frobenius' trace for the CRS base curve.
   rop must be initialized.
   One should call PARI to turn this into a real function.
*/
void MG_curve_trace(fmpz_t rop) {

	char trace[] = "-147189550172528104900422131912266898599387555512924231762107728432541952979290";
	fmpz_set_str(rop, trace, 10);
}

/**
   Sets rop to the cardinal of E(F_p) where p is the caracteristic of the base field.
   rop must be initialized.
*/
void MG_curve_card_base(fmpz_t rop, MG_curve_t *E) {

	fmpz_t trace;
	fmpz_init(trace);
	MG_curve_trace(trace);

	fmpz_set(rop, fq_ctx_prime(*(E->F)));
	fmpz_add_ui(rop, rop, 1);
	fmpz_sub(rop, rop, trace);

	fmpz_clear(trace);
}

/**
   Sets rop to the cardinal of E(F_p^r) where p is the caracteristic of the base field.
   rop must be initialized.
   r must be positive and greater than 1.
   ref: https://perso.univ-rennes1.fr/christophe.ritzenthaler/cours/elliptic-curve-course.pdf page 11.
	TODO: Work with q instead of p, problem with quad twist? Remove E in parameters, p is constant. r should be an int.
*/
void MG_curve_card_ext(fmpz_t rop, MG_curve_t *E, fmpz_t r) {

	fmpz_t t, p, c, s, s_, tmp;

	fmpz_init(t);	// trace
	fmpz_init(p);	// caracteristic
	fmpz_init(c);	// counter
	fmpz_init(s);	// s_n
	fmpz_init(s_);	// s_{n-1}
	fmpz_init(tmp);

	fmpz_set(p, fq_ctx_prime(*(E->F)));
	MG_curve_trace(t);
	fmpz_set(c, r);

	// Initialization
	fmpz_set_ui(s_, 2);
	fmpz_set(s, t);

	while(!fmpz_is_one(c)) {

		// compute new s_n
		fmpz_set(tmp, s);
		fmpz_mul(s, s, t);
		fmpz_mul(s_, s_, p);
		fmpz_sub(s, s, s_);

		// set s_{n-1} to previous s_n
		fmpz_set(s_, tmp);

		fmpz_sub_ui(c, c, 1);
	}
	// Compute cardinal as q^r + 1 - s_
	fmpz_pow_ui(rop, p, fmpz_get_ui(r));
	fmpz_add_ui(rop, rop, 1);
	fmpz_sub(rop, rop, s);

	fmpz_clear(t);
	fmpz_clear(p);
	fmpz_clear(c);
	fmpz_clear(s);
	fmpz_clear(s_);
	fmpz_clear(tmp);
}

/**
   Sets P to a random l-torsion point on the underlying curve and returns 1.
   The point P will be strictly in E(F_q^r).

   not implemented yet:
   If r % 2 == 0, the x-coordinate of P will be in F_q^r//2 (x-only arithmetics).
   Variable card holds the cardinal of E(F_q) and can be computed using MG_curve_card.
   TODO: card will be hardcoded and held in a struct.
   Returns 0 in case of failure (no such point on E).
*/
int MG_curve_rand_torsion(MG_point_t *P, fmpz_t l, fmpz_t card) {

	flint_rand_t state;
	fmpz_t val, cofactor, e;
	MG_point_t Q, R;
	bool isinfty = 1;

	fmpz_init(val);
	fmpz_init(cofactor);
	fmpz_init(e);
	MG_point_init(&Q, P->E);
	MG_point_init(&R, P->E);
	flint_randinit(state);

	MG_point_set_infty(&Q);

	fmpz_val_q(val, cofactor, card, l); // Possible optimization?
	if(fmpz_is_zero(val)) return 0;

	while(isinfty) {

		MG_point_rand_ninfty(&R, state);
		MG_ladder_iter_(&Q, cofactor, &R);
		MG_point_isinfty(&isinfty, &Q);
	};

	// Extract l-torsion point from possibly l^val-torsion point.
	// Here R acts as a temporary variable for l*Q
	MG_ladder_iter_(&R, l, &Q);
	MG_point_isinfty(&isinfty, &R);
	fmpz_set_ui(e, 1);

	// While l*Q != O do Q := l*Q
	while(!isinfty && 0 >= fmpz_cmp(e, val)) {
		MG_point_set_(&Q, &R);
		MG_ladder_iter_(&R, l, &Q);
		MG_point_isinfty(&isinfty, &R);

		fmpz_add_ui(e, e, 1);
	}

	// Case of failure
	if(!isinfty) return 0;

	MG_point_normalize(&Q);
	MG_point_set_(P, &Q);

	MG_point_clear(&R);
	MG_point_clear(&Q);
	fmpz_clear(e);
	fmpz_clear(cofactor);
	fmpz_clear(val);
	flint_randclear(state);

	return 1;
}
/**
WIP: only for degree two reduction
   Sets P to a random l-torsion point on the underlying curve and returns 1.
   The point P will be strictly in E(F_q^r).

   not implemented yet:
   If r % 2 == 0, the x-coordinate of P will be in F_q^r//2 (x-only arithmetics).
   Variable card holds the cardinal of E(F_q) and can be computed using MG_curve_card.
   TODO: card will be hardcoded and held in a struct.
   Returns 0 in case of failure (no such point on E).
*/
int MG_curve_rand_torsion_(MG_point_t *P, fmpz_t l, fmpz_t card) {

	int ec = 0;
	flint_rand_t state;
	fmpz_t val, cofactor;
	MG_point_t Q, R, Q_test;
	bool isinfty = 1;

	fmpz_init(val);
	fmpz_init(cofactor);
	MG_point_init(&Q, P->E);
	MG_point_init(&R, P->E);
	MG_point_init(&Q_test, P->E);
	flint_randinit(state);

	MG_point_set_infty(&Q);

	fmpz_val_q(val, cofactor, card, l);
	if(fmpz_is_zero(val)) return 0;

	while(isinfty) {

		MG_point_rand_ninfty_nsquare(&R, state);
		MG_ladder_iter_(&Q, cofactor, &R);
		MG_point_isinfty(&isinfty, &Q);
	};

	// Extract l-torsion point from possibly l^val-torsion point.
	// Here R acts as a temporary variable for l*Q
	MG_ladder_iter_(&R, l, &Q);
	MG_point_isinfty(&isinfty, &R);
	int e = 0;

	// While l*Q != O do Q := l*Q
	while(!isinfty && 0 < fmpz_cmp_ui(val, e)) {
		MG_point_set_(&Q, &R);
		MG_ladder_iter_(&R, l, &Q);
		MG_point_isinfty(&isinfty, &R);

		e++;
	}

	// Case of failure
	if(!isinfty) ec = 0;
	else {ec = 1; MG_point_set_(P, &Q);}

	MG_point_clear(&R);
	MG_point_clear(&Q);
	fmpz_clear(cofactor);
	fmpz_clear(val);
	flint_randclear(state);

	return ec;
}

/******************************
  Tate form Arithmetics
******************************/
/**
  Sets rop to the Tate-normal form of op relative to point P
  rop must be initialized.
TODO: Check torsion
*/
void MG_get_TN(TN_curve_t *rop, MG_curve_t *op, MG_point_t *P, fmpz_t l){

	fq_t y2, b2, b3_, b4, c2, b, c, tmp1, tmp2;

	const fq_ctx_t *F = op->F;
	fq_init(y2, *F);
	fq_init(tmp1, *F);
	fq_init(tmp2, *F);
	fq_init(b2, *F);
	fq_init(b3_, *F);
	fq_init(b4, *F);
	fq_init(c2, *F);
	fq_init(b, *F);
	fq_init(c, *F);

	// extract y^2 into y2
	// TODO: make this a function
	MG_point_normalize(P);
	fq_pow_ui(tmp1, P->X, 2, *F);
	fq_mul(tmp2, P->X, op->A, *F);
	fq_add(tmp1, tmp2, tmp1, *F);
	fq_add_ui(tmp1, tmp1, 1, *F);
	fq_mul(tmp1, tmp1, P->X, *F);

	fq_inv(tmp2, op->B, *F);
	fq_mul(y2, tmp1, tmp2, *F);

	// Computing temporary coefficients
	// b2 = 3x + A
	fq_mul_ui(b2, P->X, 3, *F);
	fq_add(b2, b2, P->E->A, *F);
	// b3_ = b3 ^ 2 = 4y^2
	fq_mul_ui(b3_, y2, 4, *F);
	// b4 = 3x^2 + 2Ax + 1
	fq_mul(b4, b2, P->X, *F);
	fq_mul(tmp1, P->X, op->A, *F);
	fq_add(b4, b4, tmp1, *F);
	fq_add_ui(b4, b4, 1, *F);

	if(!fmpz_equal_ui(l, 3)) {

		// c2 = b2 - b4^2 / b3_
		fq_pow_ui(tmp1, b4, 2, *F);
		fq_inv(tmp2, b3_, *F);
		fq_mul(c2, tmp1, tmp2, *F);
		fq_sub(c2, b2, c2, *F);

		// b = -c2^3 / b3_
		fq_pow_ui(b, c2, 3, *F);
		fq_mul(b, b, tmp2, *F);
		fq_neg(b, b, *F);

		// c = 1-2* b4/b3_ * c2
		fq_mul(c, b4, tmp2, *F);
		fq_mul(c, c, c2, *F);
		fq_mul_ui(c, c, 2, *F);
		fq_neg(c, c, *F);
		fq_add_ui(c, c, 1, *F);
	}
	else {
		// c = 1-2b4/b3_
		fq_inv(c, b3_, *F);
		fq_mul(c, c, b4, *F);
		fq_mul_ui(c, c, 2, *F);
		fq_neg(c, c, *F);
		fq_add_ui(c, c, 1, *F);

		// b = -1/b3^2
		fq_neg(b, b3_, *F);
		fq_inv(b, b, *F);
	}

	TN_curve_set(rop, b, c, l, F);

	// Free memory
	fq_clear(y2, *F);
	fq_clear(tmp1, *F);
	fq_clear(tmp2, *F);
	fq_clear(b2, *F);
	fq_clear(b3_, *F);
	fq_clear(b4, *F);
	fq_clear(c2, *F);
	fq_clear(b, *F);
	fq_clear(c, *F);
}

/**
  Sets rop to the Montgomery form of op
  Returns 1 if successful, 0 otherwise.
  rop must be initialized.
TODO: Check torsion
*/
int TN_get_MG(MG_curve_t *rop, TN_curve_t * op){

	fq_t b2, b4, b6, c2, c4, tmp1, tmp2, alpha;
	fq_poly_t pol, pol_;
	fq_poly_factor_t fac;

	const fq_ctx_t *F = op->F;
	fq_init(b2, *F);
	fq_init(b4, *F);
	fq_init(b6, *F);
	fq_init(c2, *F);
	fq_init(c4, *F);
	fq_init(tmp1, *F);
	fq_init(tmp2, *F);
	fq_init(alpha, *F);
	fq_poly_init(pol, *F);
	fq_poly_init(pol_, *F);
	fq_poly_factor_init(fac, *F);

	// Set tmp1 to (c-1)/2 for re-use
	fq_sub_ui(tmp1, op->c, 1, *F);
	fq_inv_ui(tmp2, 2, *F);
	fq_mul(tmp1, tmp1, tmp2, *F);

	// Set b2 = (c-1)^2 / 4 - b or (c-1)^2/4 if l = 3
	fq_pow_ui(tmp2, tmp1, 2, *F);
	if(fmpz_equal_ui(op->l, 3)) fq_set(b2, tmp2, *F);
	else if(!fmpz_equal_ui(op->l, 3)) fq_sub(b2, tmp2, op->b, *F);

	// Set b4 = (c-1)b/2
	fq_mul(b4, tmp1, op->b, *F);

	// Set b6 = b^2 / 4
	fq_pow_ui(b6, op->b, 2, *F);
	fq_inv_ui(tmp1, 4, *F);
	fq_mul(b6, b6, tmp1, *F);

	//////// Prepare polynomial to extract x candidates
	//// set coefficients
	fq_set_ui(tmp1, 1, *F);
	fq_poly_set_coeff(pol, 0, b6, *F);
	fq_poly_set_coeff(pol, 1, b4, *F);
	fq_poly_set_coeff(pol, 2, b2, *F);
	fq_poly_set_coeff(pol, 3, tmp1, *F);

	//// Factor polynomial, catch errors, discard lead
	fq_poly_factor(fac, tmp1, pol, *F);
	if(fac->num < 2) return 0; // pol has no roots in the underlying field!

	//// Get roots candidates for x
	fq_t roots[fac->num];
	int roots_flag[fac->num];
	for(int i=0; i < fac->num; i++){
		/// Check if this factor is of degree 1, ITC leave 0 (non-root)
		fq_init(roots[i], *F);
		if(fq_poly_degree(fac->poly + i, *F) == 1) {
			fq_poly_get_coeff(roots[i], fac->poly + i, 0, *F);
			fq_neg(roots[i], roots[i], *F); // Careful there
			roots_flag[i] = 1;
		}
		else{
			roots_flag[i] = 0;
		}
	}

	//// Try roots candidates
	int ret = 0;
	for(int i=0; i < fac->num; i++) {
		if(roots_flag[i]) {


			// Set c4 = pol'(x), we need a square root of c4 to continue
			fq_poly_derivative(pol_, pol, *F);
			fq_poly_evaluate_fq(c4, pol_, roots[i], *F);
			int sqrt_ret = fq_sqr_from_polyfact(alpha, c4, *F);

			if(sqrt_ret == 1) {
				/// If successful, create A = c2 = (3x + b2)/alpha
				fq_mul_ui(c2, roots[i], 3, *F);
				fq_add(c2, c2, b2, *F);
				fq_div(c2, c2, alpha, *F);
				fq_set_ui(tmp1, 1, *F);
				MG_curve_set(rop, F, c2, tmp1);
				return 1;
			}
		}
	}

	fq_clear(b2, *F);
	fq_clear(b4, *F);
	fq_clear(b6, *F);
	fq_clear(c2, *F);
	fq_clear(c4, *F);
	fq_clear(tmp1, *F);
	fq_clear(tmp2, *F);
	fq_clear(alpha, *F);
	fq_poly_clear(pol, *F);
	fq_poly_clear(pol_, *F);
	fq_poly_factor_clear(fac, *F);

	if(ret == 0) return 0; // Error!
	return 1;
}

