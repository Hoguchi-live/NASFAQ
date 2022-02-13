#include "velu.h"

/**
  Computes the lengths b, b', lenK of the three KPS arrays.
*/
void _init_lengths(uint* b, uint* bprime, uint* lenK, uint l) {
	*bprime = l-1;
	*b = 1;
	while (((*b)+1)*((*b)+1) <= *bprime) {
		(*b)++;
	}
	*b = *b/2;
	*bprime = (*bprime)/(4*(*b));
	*lenK = (l-1-4*(*b)*(*bprime))/2;
}

/**
  Fills the arrays I,J,K with the multiples of P required by xISOG
*/
void KPS(MG_point_t *I, MG_point_t *J, MG_point_t *K, MG_point_t P, uint l, uint b, uint bprime, uint lenK) {
	// array I of length brpime
	// array J of length b
	// array K of length lenK


	MG_curve_t *E = P.E;
	MG_point_t P2, P4, P4b;

	MG_point_init(&P2, E);
	MG_point_init(&P4, E);
	MG_point_init(&P4b, E);

	MG_xDBL(&P2, P); //P2 = 2*P
	MG_xDBL(&P4, P2); //P4 = 4*P

	//computing J = {(2j+1)*P for j = 1, ..., b-1}
	MG_point_set_(&J[0], &P);

	// If l>17 then bprime >= b >= 2 therefore J has at least two elements
	// If l = 11 or 13, then b = 1, bprime >= 2
	if(l >= 17) MG_xADD(&(J[1]), P, P2, P); //J[1] = 3*P

	for (int j=2; j<b; j++) {
		MG_xADD(&(J[j]), J[j-1], P2, J[j-2]);
	}

	//computing I = {2b(2i+1)*P for i = 1, ..., bprime-1}
	//// Set I[0] = 2b*P
	if (b%2 == 0) {
		MG_xADD(&I[0], J[b/2], J[b-(b/2) - 1], P2);
	}
	else {
		//MG_xADD(&I[0], J[b/2], J[b-(b/2)], P4);
		MG_xDBL(&I[0], J[b/2]);
	}

	MG_xDBL(&P4b, I[0]); // P4b = 4b*P
	MG_xADD(&I[1], P4b, I[0], I[0]); // I[1] = 6b*P = 4b*P + 2b*P

	for (int i=2; i<bprime; i++) {
		MG_xADD(&I[i], I[i-1], P4b, I[i-2]);
	}


	//computing K = {i*P for i = 4*b*bprime+1, ..., l-4, l-2}

	if (lenK>0) {
		MG_point_set_(&(K[lenK-1]), &P2); // (l-2)*P = -2*P
		if (lenK>1) {
			MG_point_set_(&(K[lenK-2]), &P4); // (l-4)*P = -4*P
		}
	}

	for (int i = lenK-3; i>=0; i--) {
		MG_xADD(&K[i], K[i+1], P2, K[i+2]);
	}


	// Memory management

	MG_point_clear(&P2);
	MG_point_clear(&P4);
	MG_point_clear(&P4b);
}

/**
  Sets A2 to the geometry parameter of a degree l MG_curve_t isogenous to the base curve.
  I,J,K must be pre-computed via KPS.
  A2 must be initialized.
*/
void xISOG(fq_t *A2, MG_point_t P, uint l, MG_point_t I[], MG_point_t J[], MG_point_t K[], uint b, uint bprime, uint lenK) {

	const fq_ctx_t *F;
	F = (P.E)->F;

	fq_poly_t E0, E1, tmp1, tmp2;
	fq_t R0, R1, M0, M1, tmp;

	fq_init(R0, *F);
	fq_init(R1, *F);
	fq_init(M0, *F);
	fq_init(M1, *F);
	fq_init(tmp, *F);
	fq_poly_init(E0, *F);
	fq_poly_init(E1, *F);
	fq_poly_init(tmp1, *F);
	fq_poly_init(tmp2, *F);

	/*
	fq_poly_t h
	fq_poly_init(h, *F);
	fq_poly_one(h, *F);

	fq_poly_t f, c;
	fq_poly_init(f, *F);
	fq_poly_init(c, *F);
	for (int i=0; i<bprime; i++) {
		fq_poly_gen(f, *F);
		fq_poly_set_fq(c, I[i].X, *F);
		fq_poly_sub(f, f, c, *F);
		fq_poly_mul(h, h, f, *F);
	}
	fq_poly_clear(f, *F);
	fq_poly_clear(c, *F);
	*/

	// computing E0, E1
	fq_poly_one(E0, *F);
	fq_poly_one(E1, *F);
	for (uint j=0; j<b; j++) {
		//// NORMALIZE
		MG_point_normalize(J+j);
		_F0pF1pF2_F0mF1pF2(&tmp1, &tmp2, J[j], *F);
		fq_poly_mul(E0, E0, tmp1, *F);
		fq_poly_mul(E1, E1, tmp2, *F);
	}

	// computing resultants R0, R1
	fq_one(R0, *F);
	fq_one(R1, *F);

	fq_t Ix[bprime];
	fq_t eval[bprime];
	for (uint i=0; i<bprime; i++) {
		fq_init(Ix[i], *F);
		/// NORMALIZE
		MG_point_normalize(I+i);
		fq_set(Ix[i], I[i].X, *F);
		//Ix[i] = I[i].X;
		fq_init(eval[i], *F);
	}

	fq_poly_multieval(eval, Ix, E0, bprime, F);
	for (uint i=0; i<bprime; i++) {
		fq_mul(R0, R0, eval[i], *F);
	}

	fq_poly_multieval(eval, Ix, E1, bprime, F);
	for (uint i=0; i<bprime; i++) {
		fq_mul(R1, R1, eval[i], *F);
		fq_clear(eval[i], *F);
		fq_clear(Ix[i], *F);
	}

	// computing M0, M1
	fq_one(M0, *F);
	fq_one(M1, *F);
	for (uint i=0; i<lenK; i++) {
		//// NORMALIZE
		MG_point_normalize(K+i);
		fq_sub_one(tmp, K[i].X, *F);
		fq_neg(tmp, tmp, *F);
		fq_mul(M0, M0, tmp, *F);
		fq_neg(tmp, K[i].X, *F);
		fq_sub_one(tmp, tmp, *F);
		fq_mul(M1, M1, tmp, *F);
	}

	// computing A2
	fq_mul(M0, M0, R0, *F);
	fq_mul(M1, M1, R1, *F);
	fq_div(M0, M0, M1, *F);
	fq_pow_ui(M0, M0, 8, *F); // M0 = [ (M0*R0)/(M1*R1) ]^8

	fq_sub_one(R0, (P.E)->A, *F);
	fq_sub_one(R0, R0, *F);
	fq_add_ui(R1, (P.E)->A, 2, *F);
	fq_div(R0, R0, R1, *F);
	fq_pow_ui(R0, R0, l, *F); // R0 = [ (A-2)/(A+2) ]^l

	fq_mul(M0, M0, R0, *F); // M0 =: d

	fq_sub_one(R0, M0, *F);
	fq_neg(R0, R0, *F);
	fq_add_ui(M0, M0, 1, *F);
	fq_div(M0, M0, R0, *F);  // M0 = (d+1)/(d-1)
	fq_mul_ui(*A2, M0, 2, *F);

	// Memory clear
	fq_clear(R0, *F);
	fq_clear(R1, *F);
	fq_clear(M0, *F);
	fq_clear(M1, *F);
	fq_clear(tmp, *F);
	fq_poly_clear(E0, *F);
	fq_poly_clear(E1, *F);
	fq_poly_clear(tmp1, *F);
	fq_poly_clear(tmp2, *F);
}

/**
   Auxiliary function for xISOG.
   The two polynomials rop1 and rop2 are computed concurrently to remove unnecessary duplicate computations.
*/
void _F0pF1pF2_F0mF1pF2(fq_poly_t *rop1, fq_poly_t *rop2, MG_point_t P, const fq_ctx_t ctx) {
	fq_poly_zero(*rop1, ctx);
	fq_poly_zero(*rop2, ctx);
	fq_t tmp1, tmp2;
	fq_init(tmp1, ctx);
	fq_init(tmp2, ctx);

	// set tmp1 = x^2 - 2x + 1
	fq_add_si(tmp1, P.X, -2, ctx);
	fq_mul(tmp1, tmp1, P.X, ctx);
	fq_add_si(tmp1, tmp1, 1, ctx);
	fq_poly_set_coeff(*rop1, 2, tmp1, ctx);
	fq_poly_set_coeff(*rop1, 0, tmp1, ctx);

	// set tmp1 = x^2 + 2x + 1
	fq_mul_si(tmp2, P.X, 4, ctx);
	fq_add(tmp1, tmp1, tmp2, ctx);
	fq_poly_set_coeff(*rop2, 2, tmp1, ctx);
	fq_poly_set_coeff(*rop2, 0, tmp1, ctx);

	// set tmp1 = -2x^2 - 4Ax - 4x - 2
	fq_add_si(tmp1, (P.E)->A, 1, ctx);
	fq_mul_si(tmp1, tmp1, 2, ctx);
	fq_add(tmp1, tmp1, P.X, ctx);
	fq_mul(tmp1, tmp1, P.X, ctx);
	fq_add_si(tmp1, tmp1, 1, ctx);
	fq_mul_si(tmp1, tmp1, -2, ctx);
	fq_poly_set_coeff(*rop1, 1, tmp1, ctx);

	// set tmp1 = 2x^2 + 4Ax - 4x + 2
	fq_mul_si(tmp2, tmp2, 2, ctx);
	fq_add(tmp1, tmp1, tmp2, ctx);
	fq_neg(tmp1, tmp1, ctx);
	fq_poly_set_coeff(*rop2, 1, tmp1, ctx);

	fq_clear(tmp1, ctx);
	fq_clear(tmp2, ctx);
}

/**
  Wrapper for xISOG and KPS.
  Computes Vélu Step curve parameter A2 from the l-torsion point P.
  A2 must be initialized.
*/
void isogeny_from_torsion(fq_t *A2, MG_point_t P, uint l) {

	uint b, bprime, lenK;
	_init_lengths(&b, &bprime, &lenK, l);

	MG_point_t I[bprime];
	MG_point_t J[b];
	MG_point_t K[lenK];

	for (int i=0; i<bprime; i++) {
		MG_point_init(&I[i], P.E);
	}
	for (int i=0; i<b; i++) {
		MG_point_init(&J[i], P.E);
	}
	for (int i=0; i<lenK; i++) {
		MG_point_init(&K[i], P.E);
	}

	KPS(I, J, K, P, l, b, bprime, lenK);

	xISOG(A2, P, l, I, J, K, b, bprime, lenK);

	for (int i=0; i<bprime; i++) {
		MG_point_clear(&I[i]);
	}
	for (int i=0; i<b; i++) {
		MG_point_clear(&J[i]);
	}
	for (int i=0; i<lenK; i++) {
		MG_point_clear(&K[i]);
	}
}

