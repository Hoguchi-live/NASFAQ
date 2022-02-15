// @file radical.c
#include "radical.h"

/**
  Extract n-th root of op using the following trick:
  	we always have
  	op ^ (p + 1) = op ^ 2
	and assuming
	p + 1 = 2*l*k
	for some natural number k, we get
	(op ^ e)^l = +/- op
	for e = (p + 1) / (2 * l).
  We get rid of the +/- sign by testing via fast exponentiation.
  A NAS condition for this to work is p + 1 = 0 % 2 * l.
  This is about 1000 times faster than bruteforcing roots of X^l - op.
**/
void fq_nth_root_trick(fq_t rop, fq_t op, fmpz_t l, const fq_ctx_t F) {

	fmpz_t tmp, e, p;
	fq_t sgn_check, alpha;

	fq_init(alpha, F);
	fq_init(sgn_check, F);
	fmpz_init(p);
	fmpz_init(e);
	fmpz_init(tmp);
	fmpz_set(p, fq_ctx_prime(F));

	//// Compute e
	fmpz_mul_ui(tmp, l, 2);
	fmpz_add_ui(e, p, 1);
	fmpz_fdiv_q(e, e, tmp);

	//// Compute alpha = op ^ e
	fq_pow(alpha, op, e, F);

	//// Check for sign
	fq_pow(sgn_check, alpha, l, F);
	if(!fq_equal(op, sgn_check, F)) fq_neg(alpha, alpha, F);

	//// Copy buffer
	fq_set(rop, alpha, F);

	fq_clear(alpha, F);
	fq_clear(sgn_check, F);
	fmpz_clear(p);
	fmpz_clear(e);
	fmpz_clear(tmp);
}

/**
  Unsigned int wrapper for fq_nth_root_trick
*/
void fq_nth_root_trick_ui(fq_t rop, fq_t op, slong l, const fq_ctx_t F) {

	fmpz_t ll;
	fmpz_init_set_ui(ll, l);
	fq_nth_root_trick(rop, op, ll, F);
	fmpz_clear(ll);
}

/**
  Sets rop as the target curve of k steps starting from op in the 3-isogeny graph
*/
void radical_isogeny_3(TN_curve_t *rop, TN_curve_t *op, fmpz_t k) {

	fmpz_t l;
	fq_t a1, a3, tmp1, tmp2, tmp3, tmp4, alpha;

	const fq_ctx_t *F = op->F;
	fq_init(a1, *F);
	fq_init(a3, *F);
	fq_init(tmp1, *F);
	fq_init(tmp2, *F);
	fq_init(tmp3, *F);
	fq_init(tmp4, *F);
	fq_init(alpha, *F);
	fmpz_init_set_ui(l, 3);

	// a1 = 1-c, a3 = -b
	fq_set_ui(a1, 1, *F);
	fq_sub(a1, a1, op->c, *F);

	// a3 = -b
	fq_neg(a3, op->b, *F);

	// Main loop that goes through k isogeny steps
	for(int step=0; fmpz_cmp_ui(k, step) > 0; step++) {

		//// Extract root of rho = -a3 = b
		fq_neg(tmp1, a3, *F);
		fq_nth_root_trick_ui(alpha, tmp1, 3, *F);

		//// Compute new a1 = -6*alpha + a1
		fq_mul_ui(tmp2, alpha, 6, *F);
		fq_sub(tmp2, a1, tmp2, *F);

		//// Compute new a3' = 3*a1*alpha^2 - a1*alpha + 9*a3
		fq_mul_ui(tmp3, a3, 9, *F);

		fq_mul_ui(tmp4, alpha, 3, *F);
		fq_sub(tmp4, tmp4, a1, *F);
		fq_mul(tmp4, tmp4, alpha, *F);
		fq_mul(tmp4, tmp4, a1, *F);

		fq_add(tmp3, tmp3, tmp4, *F);

		//// Copy buffer
		fq_set(a1, tmp2, *F);
		fq_set(a3, tmp3, *F);
	}
	//// Set curve
	fq_neg(tmp1, a1, *F);
	fq_add_ui(tmp1, tmp1, 1, *F);
	fq_neg(tmp2, a3, *F);
	TN_curve_set(rop, tmp2, tmp1, l, op->F);

	//// Clear
	fq_clear(a1, *F);
	fq_clear(a3, *F);
	fq_clear(tmp1, *F);
	fq_clear(tmp2, *F);
	fq_clear(tmp3, *F);
	fq_clear(tmp4, *F);
	fq_clear(alpha, *F);
	fmpz_clear(l);
}

/**
  Sets rop as the target curve of k steps starting from op in the 5-isogeny graph
*/
void radical_isogeny_5(TN_curve_t *rop, TN_curve_t *op, fmpz_t k) {

	// Nothing to do
	if(fmpz_equal_ui(k, 0)) {
		TN_curve_set_(rop, op);
		return;
	}

	fmpz_t l;
	fq_t b, alpha, tmp1, tmp2, tmp3, num, den, res;
	fq_t alpha_pow[4];

	const fq_ctx_t *F = op->F;
	fq_init(b, *F);
	fq_init(alpha, *F);
	fq_init(tmp1, *F);
	fq_init(tmp2, *F);
	fq_init(tmp3, *F);
	fq_init(res, *F);
	fq_init(num, *F);
	fq_init(den, *F);
	fmpz_init_set_ui(l, 5);

	// Init b = op->b
	fq_set(b, op->b, *F);

	// Main loop that goes through k isogeny steps
	for(int step=0; fmpz_cmp_ui(k, step) > 0; step++) {

		//// Extract root of rho = b
		fq_nth_root_trick_ui(alpha, b, 5, *F);

		//// Store alpha ^ i for i = 1 to 4
		for(int i=0; i< 4; i++){
			fq_init(alpha_pow[i], *F);
			if(i == 0) fq_set(alpha_pow[i], alpha, *F);
			else fq_mul(alpha_pow[i], alpha_pow[i-1], alpha, *F);
		}

		//// Precompute 2*alpha, 3*alpha, 4*alpha^2
		fq_mul_ui(tmp1, alpha, 2, *F);
		fq_add(tmp2, tmp1, alpha, *F);
		fq_mul_ui(tmp3, alpha_pow[1], 4, *F);

		// Compute base shared by numerator and denominator: alpha^4 + 4alpha^2 + 1
		fq_add(num, alpha_pow[3], tmp3, *F);
		fq_add_ui(num, num, 1, *F);
		fq_set(den, num, *F);

		//// Finish num/den
		fq_add(num, num, tmp1, *F);
		fq_mul(tmp3, tmp2, alpha_pow[1], *F); // 3alpha * alpha ^ 2
		fq_add(num, num, tmp3, *F);

		fq_sub(den, den, tmp2, *F);
		fq_mul(tmp1, tmp1, alpha_pow[1], *F); // 2alpha * alpha ^ 2
		fq_sub(den, den, tmp1, *F);

		//// Finally compute alpha * num / den
		fq_mul(res, alpha, num, *F);
		fq_inv(tmp1, den, *F);
		fq_mul(res, res, tmp1, *F);

		//// Copy buffer
		fq_set(b, res, *F);
	}
	//// Set curve (here b = c)
	TN_curve_set(rop, b, b, l, F);

	fmpz_clear(l);
	fq_clear(alpha, *F);
	fq_clear(tmp1, *F);
	fq_clear(tmp2, *F);
	fq_clear(tmp3, *F);
	fq_clear(res, *F);
	fq_clear(num, *F);
	fq_clear(den, *F);
	for(int i=0; i< 4; i++) fq_clear(alpha_pow[i], *F);
}

/**
  Sets rop as the target curve of k steps starting from op in the 7-isogeny graph
*/
void radical_isogeny_7(TN_curve_t *rop, TN_curve_t *op, fmpz_t k) {

	fmpz_t l;
	fq_t b, c, A, rho, alpha, tmp1, tmp2, tmp3, tmp4, num, den;
	fq_t alpha_pow[6], A_pow[4];

	const fq_ctx_t *F = op->F;
	fq_init(b, *F);
	fq_init(c, *F);
	fq_init(A, *F);
	fq_init(rho, *F);
	fq_init(alpha, *F);
	fq_init(tmp1, *F);
	fq_init(tmp2, *F);
	fq_init(tmp3, *F);
	fq_init(tmp4, *F);
	fq_init(num, *F);
	fq_init(den, *F);
	fmpz_init_set_ui(l, 7);

	// We're only using A = b/c and b = A^2(A-1) in the loop
	fq_div(A, op->b, op->c, *F);

	// Main loop that goes through k isogeny steps
	for(int step=0; fmpz_cmp_ui(k, step) > 0; step++) {

		//// Set rho = A^2 * b = A^4(A-1)
		fq_pow_ui(rho, A, 4, *F);
		fq_sub_ui(tmp1, A, 1, *F);
		fq_mul(rho, rho, tmp1, *F);

		//// Extract root of rho = b^3 / c^2 = A^2 * b
		fq_nth_root_trick_ui(alpha, rho, 7, *F);

		//// Store alpha ^ i for i = 1 to 6
		for(int i=0; i< 6; i++){
			fq_init(alpha_pow[i], *F);
			if(i == 0) fq_set(alpha_pow[i], alpha, *F);
			else fq_mul(alpha_pow[i], alpha_pow[i-1], alpha, *F);
		}

		//// Store A ^ i for i = 1 to 4
		for(int i=0; i< 4; i++){
			fq_init(A_pow[i], *F);
			if(i == 0) fq_set(A_pow[i], A, *F);
			else fq_mul(A_pow[i], A_pow[i-1], A, *F);
		}

		//// Precompute A*alpha^4, A^3*alpha^2, A^3*alpha
		fq_mul(tmp1, A, alpha_pow[3], *F);
		fq_mul(tmp2, A_pow[2], alpha_pow[1], *F);
		fq_mul(tmp3, A_pow[2], alpha, *F);

		//// Compute num
		fq_add(num, alpha_pow[5], A_pow[3], *F);
		fq_sub(num, num, tmp3, *F);
		fq_mul(tmp4, alpha, tmp1, *F);
		fq_add(num, num, tmp4, *F);
		fq_mul_ui(tmp4, tmp2, 2, *F);
		fq_add(num, num, tmp4, *F);

		//// Compute den
		fq_sub(den, A_pow[3], alpha_pow[5], *F);
		fq_add(den, den, tmp1, *F);
		fq_add(den, den, tmp2, *F);
		fq_mul_ui(tmp4, tmp3, 2, *F);
		fq_sub(den, den, tmp4, *F);

		//// Finally compute A_new =  * num / den
		fq_inv(tmp4, den, *F);
		fq_mul(A, num, tmp4, *F);
	}
	// Set curve (here c = A(A-1) and b = Ac)
	fq_sub_ui(tmp4, A, 1, *F);
	fq_mul(c, tmp4, A, *F);

	fq_mul(b, c, A, *F);

	TN_curve_set(rop, b, c, l, F);

	fmpz_clear(l);
	fq_clear(alpha, *F);
	fq_clear(tmp1, *F);
	fq_clear(tmp2, *F);
	fq_clear(tmp3, *F);
	fq_clear(tmp4, *F);
	fq_clear(num, *F);
	fq_clear(den, *F);
	fq_clear(rho, *F);
	fq_clear(A, *F);
	fq_clear(b, *F);
	fq_clear(c, *F);
	for(int i=0; i< 6; i++) fq_clear(alpha_pow[i], *F);
	for(int i=0; i< 4; i++) fq_clear(A_pow[i], *F);
}

