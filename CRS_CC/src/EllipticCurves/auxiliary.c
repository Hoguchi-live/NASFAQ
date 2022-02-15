/// @file auxiliary.c
#include "auxiliary.h"

/**
  Sets rop to the sum of op and x, where x is a string considered as an element of F.
*/
void fq_set_str(fq_t rop, char *s, const fq_ctx_t F) {

	fmpz_t tmp;
	fmpz_init(tmp);
	fmpz_set_str(tmp, s, 10);

	fq_set_fmpz(rop, tmp, F);

	fmpz_clear(tmp);
}

/**
  Sets rop to the sum of op and x, where x is an ulong considered as an element of F.
*/
void fq_add_ui(fq_t rop, fq_t op, ulong x, const fq_ctx_t F){

	fq_t xx;
	fq_init(xx, F);
	fq_set_ui(xx, x, F);

	fq_add(rop, op, xx, F);

	fq_clear(xx, F);
}

/**
  Sets rop to the sum of op and x, where x is a slong considered as an element of F.
*/
void fq_add_si(fq_t rop, fq_t op, slong x, const fq_ctx_t F){

	fq_t xx;
	fq_init(xx, F);
	fq_set_si(xx, x, F);

	fq_add(rop, op, xx, F);

	fq_clear(xx, F);
}

/**
  Sets rop to the difference of op and x, where x is an ulong considered as an element of F.
*/
void fq_sub_ui(fq_t rop, fq_t op, ulong x, const fq_ctx_t F){

	fq_t xx;
	fq_init(xx, F);
	fq_set_ui(xx, x, F);

	fq_sub(rop, op, xx, F);

	fq_clear(xx, F);
}


/**
  Sets rop to the sum of op and x, where x is a slong considered as an element of F.
*/
void fq_sub_si(fq_t rop, fq_t op, slong x, const fq_ctx_t F){

	fq_t xx;
	fq_init(xx, F);
	fq_set_si(xx, x, F);

	fq_sub(rop, op, xx, F);

	fq_clear(xx, F);
}

/**
  Sets rop to the quotient of op and x, reducing the output in the given context.
*/
void fq_div_si(fq_t rop, fq_t op, slong x, const fq_ctx_t F) {

	fq_t xx;
	fq_init(xx, F);
	fq_set_si(xx, x, F);

	fq_div(rop, op, xx, F);

	fq_clear(xx, F);
}

/**
  Sets rop to the quotient of op and x, reducing the output in the given context.
*/
void fq_div_ui(fq_t rop, fq_t op, ulong x, const fq_ctx_t F) {

	fq_t xx;
	fq_init(xx, F);
	fq_set_ui(xx, x, F);

	fq_div(rop, op, xx, F);

	fq_clear(xx, F);
}

/**
  Sets rop to the inverse of op, reducing the output in the given context.
*/
void fq_inv_ui(fq_t rop, ulong op, const fq_ctx_t F) {

	fq_t tmp;
	fq_init(tmp, F);
	fq_set_ui(tmp, op, F);

	fq_inv(rop, tmp, F);

	fq_clear(tmp, F);
}


/**************************************************************************************
  fmpz
**************************************************************************************/

/*
   Sets rop to the power of op to the n.
   n should be an ulong-sized fmpz_t.
*/
void fmpz_pow(fmpz_t rop, fmpz_t op, fmpz_t n) {

	fmpz_pow_ui(rop, op, fmpz_get_ui(n));
}

/*
   Sets rop to the op2-valuation of op.
   op2 should not be 0 or 1.
   rop should be initialized to 0.
   TODO: handle 0 and 1 via error.
*/
void fmpz_val(fmpz_t rop, fmpz_t op1, fmpz_t op2) {

	if(fmpz_is_one(op2) || fmpz_is_zero(op2)) return;

	fmpz_t quo, rem;

	fmpz_init(quo);
	fmpz_init(rem);

	fmpz_set(quo, op1);
	fmpz_tdiv_qr(quo, rem, op1, op2);

	while(fmpz_is_zero(rem)) {

		fmpz_tdiv_qr(quo, rem, quo, op2);
		fmpz_add_ui(rop, rop, 1);
	}

	fmpz_clear(rem);
	fmpz_clear(quo);
}

/*
   Sets rop1 to the op2-valuation of op and rop2 to the quotient op1/op2^rop.
   op2 should not be 0 or 1.
   rop1 and rop2 should be initialized to 0.
   TODO: handle 0 and 1 via error.
*/
void fmpz_val_q(fmpz_t rop1, fmpz_t rop2, fmpz_t op1, fmpz_t op2) {

	if(fmpz_is_one(op2) || fmpz_is_zero(op2)) return;

	fmpz_t rem, tmp_rop1, tmp_rop2;

	fmpz_init(rem);
	fmpz_init(tmp_rop1);
	fmpz_init(tmp_rop2);

	// op1 = op2*tmp_rop2 + rem
	fmpz_set(tmp_rop2, op1); // if val = 0, op1 = 1*rop2

	fmpz_tdiv_qr(tmp_rop2, rem, op1, op2);
	if(fmpz_is_zero(rem)) {
		fmpz_add_ui(tmp_rop1, tmp_rop1, 1);
		fmpz_set(rop1, tmp_rop1);
		fmpz_set(rop2, tmp_rop2);
	}

	while(fmpz_is_zero(rem)) {

		fmpz_tdiv_qr(tmp_rop2, rem, tmp_rop2, op2);
		if(fmpz_is_zero(rem)) {
			fmpz_add_ui(tmp_rop1, tmp_rop1, 1);
			fmpz_set(rop1, tmp_rop1);
			fmpz_set(rop2, tmp_rop2);
		}
	}

	fmpz_clear(rem);
	fmpz_clear(tmp_rop1);
	fmpz_clear(tmp_rop2);
}

