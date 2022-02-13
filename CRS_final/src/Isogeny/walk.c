// @file walk.c
#include "walk.h"

/**
  Take k steps in the l-isogeny graph using radical isogeny.
	MG_get_TN should return an int error code.
	radical_isogeny should return an int error code.
**/
int walk_rad(MG_curve_t *rop, MG_curve_t *op, fmpz_t l, fmpz_t k) {

	int ec = 1;

	// Nothing to do
	if(fmpz_equal_ui(k, 0)) {
		MG_curve_set_(rop, op);
		return ec;
	}

	//// Init variables
	fmpz_t k_local;
	MG_point_t P;
	TN_curve_t E_TN_tmp1, E_TN_tmp2;
	fmpz_t card, r;

	fmpz_init(card);
	fmpz_init_set(k_local, k);
	MG_point_init(&P, op);
	TN_curve_init(&E_TN_tmp1, l, op->F);
	TN_curve_init(&E_TN_tmp2, l, op->F);
	fmpz_init(r);

	//// Direction of the walk
	if(fmpz_cmp_ui(k, 0) >= 0) {
		// case k>0
		fmpz_set_ui(r, 1);
		MG_curve_card_ext(card, op, r);
		ec = MG_curve_rand_torsion(&P, l, card);
	}
	else {
		// case k<0
		fmpz_set_ui(r, 2);
		fmpz_neg(k_local, k_local);
		MG_curve_card_ext(card, op, r);
		ec = MG_curve_rand_torsion_(&P, l, card);
	}

	//// Transform op in Tate-normal form
	MG_get_TN(&E_TN_tmp1, op, &P, l);

	//// Walk
	if(fmpz_equal_ui(l, 3)) radical_isogeny_3(&E_TN_tmp2, &E_TN_tmp1, k_local);
	else if(fmpz_equal_ui(l, 5)) radical_isogeny_5(&E_TN_tmp2, &E_TN_tmp1, k_local);
	else if(fmpz_equal_ui(l, 7)) radical_isogeny_7(&E_TN_tmp2, &E_TN_tmp1, k_local);
	else return 0;

	//// Transform result back into Mongomery form
	ec = TN_get_MG(rop, &E_TN_tmp2);

	//// Clear
	fmpz_clear(k_local);
	MG_point_clear(&P);
	TN_curve_clear(&E_TN_tmp1);
	TN_curve_clear(&E_TN_tmp2);
	fmpz_clear(r);
	fmpz_clear(card);

	return ec;
}

/**
  Take k steps in the l-isogeny graph using the sqrt-velu algorithm.
**/
int walk_velu(MG_curve_t *rop, MG_curve_t *op, fmpz_t l, fmpz_t k) {

	int ec = 1;

	//// Nothing to do
	if(fmpz_equal_ui(k, 0)) {
		MG_curve_set_(rop, op);
		return ec;
	}

	//// Init variables
	fq_t new_A, new_B;
	fmpz_t k_local;
	MG_point_t P;
	TN_curve_t E_TN_tmp1, E_TN_tmp2;
	fmpz_t card, r;

	fmpz_init(r);
	fmpz_init(card);
	fq_init(new_A, *(op->F));
	fq_init(new_B, *(op->F));
	fmpz_init_set(k_local, k);
	MG_point_init(&P, op);
	TN_curve_init(&E_TN_tmp1, l, op->F);
	TN_curve_init(&E_TN_tmp2, l, op->F);

	fmpz_set_ui(r, fq_ctx_degree(*(op->F)));

	//// Direction of the walk
	if(fmpz_cmp_ui(k, 0) >= 0) {
		// case k>0
		MG_curve_card_ext(card, op, r);

		//// Main loop
		for(int i = 0; fmpz_cmp_ui(k_local, i) > 0; i++) {
			ec = MG_curve_rand_torsion(&P, l, card);
			isogeny_from_torsion(&new_A, P, fmpz_get_ui(l));
		}
	}
	else {
		// case k<0, we're walking in the quadratic-twist-component
		fmpz_mul_ui(r, r, 2);
		fmpz_neg(k_local, k_local);
		MG_curve_card_ext(card, op, r);

		//// Main loop
		for(int i = 0; fmpz_cmp_ui(k_local, i) > 0; i++) {
			ec = MG_curve_rand_torsion_(&P, l, card);
			isogeny_from_torsion(&new_A, P, fmpz_get_ui(l));
		}
	}


	//// Set output
	fq_set_ui(new_B, 1, *(op->F));
	MG_curve_set(rop, op->F, new_A, new_B);

	//// Clear
	fq_clear(new_A, *(op->F));
	fq_clear(new_B, *(op->F));
	fmpz_clear(k_local);
	MG_point_clear(&P);
	TN_curve_clear(&E_TN_tmp1);
	TN_curve_clear(&E_TN_tmp2);
	fmpz_clear(card);
	fmpz_clear(r);

	return ec;
}

