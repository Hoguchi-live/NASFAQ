/// @file multieval.c
#include "multieval.h"

/**
  Build a remainder tree from a fq_t roots array.
  This is supposed to be wrapped by the remainderTree function.
  The offset is used to slice the array (represent the position of element 0).
  This function is not responsible for the handling of the memory associated with the bcells.
*/
void remainderCell(fq_poly_bcell_t *rop, fq_t *roots, uint offset_start, uint offset_end, const fq_ctx_t *F) {

	uint offset_split;
	fq_t tmp;
	fq_poly_t p;

	fq_init(tmp, *F);
	fq_poly_init(p, *F);

	if(offset_end - offset_start == 0) {

		//// Set rop to a unique cell containing the X-root polynomial
		fq_neg(tmp, roots[offset_start], *F);
		fq_poly_set_coeff(p, 0, tmp, *F);
		fq_set_ui(tmp, 1, *F);
		fq_poly_set_coeff(p, 1, tmp, *F);

		fq_poly_bcell_set(rop, p);
	}
	else {

		//// Recursively construct the tree
		fq_poly_bcell_t *left, *right;

		left = malloc(sizeof(fq_poly_bcell_t));
		right = malloc(sizeof(fq_poly_bcell_t));

		fq_poly_bcell_init(left, F);
		fq_poly_bcell_init(right, F);

		offset_split = offset_start + (offset_end - offset_start) / 2;

		remainderCell(left, roots, offset_start, offset_split , F);
		remainderCell(right, roots, offset_split + 1, offset_end , F);

		//// Compute product of child polynomials
		fq_poly_mul(p, left->data, right->data, *F);

		fq_poly_bcell_set_(rop, left, right, p);
	}

	fq_clear(tmp, *F);
	fq_poly_clear(p, *F);
}

/**
  See remainderCell.
  T should be initialized.
*/
void remainderTree(fq_poly_btree_t *T, fq_t *roots, uint len, const fq_ctx_t *F) {

	remainderCell(T->head, roots, 0, len-1, F);
}

void fq_poly_multieval_fromtree(fq_poly_bcell_t *c, fq_t *res, fq_poly_t P, uint *k, const fq_ctx_t *F) {

	fq_poly_t Q;

	fq_poly_init(Q, *F);

	//// New polynomial
	fq_poly_rem(Q, P, c->data, *F);

	if(c->left == NULL) {

		fq_poly_get_coeff(res[*k], Q, 0, *F);
		(*k)++;
	}
	else {
		fq_poly_multieval_fromtree(c->left, res, Q, k, F);
		fq_poly_multieval_fromtree(c->right, res, Q, k, F);
	}
	fq_poly_clear(Q, *F);
}

void fq_poly_multieval(fq_t * rop, fq_t * op, fq_poly_t P, uint len, const fq_ctx_t *F) {

	uint k = 0;
	fq_poly_btree_t T;

	fq_poly_btree_init(&T, F);

	//// Construct tree with modulos
	remainderTree(&T, op, len, F);

	//// Evaluate P by remainders
	fq_poly_multieval_fromtree(T.head, rop, P, &k, F);

	fq_poly_btree_clear(&T);
}

