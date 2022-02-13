/// @file binary_trees.h
#ifndef _BINARY_TREES_H_
#define _BINARY_TREES_H_

#include <gmp.h>
#include <flint/fmpz.h>
#include <flint/fq.h>
#include <flint/fq_poly.h>


/*********************************
  Structures
*********************************/
/**
  Representation of binary links with fq_poly as nodes.
*/
typedef struct fq_poly_bcell_t {
	const fq_ctx_t *F;
	fq_poly_t data;
	//fq_poly_bcell_t *parent;
	struct fq_poly_bcell_t *left;
	struct fq_poly_bcell_t *right;
} fq_poly_bcell_t;

/**
  Representation of binary tree with fq_poly_bcell_t nodes.
*/
typedef struct fq_poly_btree_t {
	const fq_ctx_t *F;
	fq_poly_bcell_t *head;
} fq_poly_btree_t;


/*********************************
  Functions
*********************************/
void fq_poly_bcell_init(fq_poly_bcell_t *, const fq_ctx_t *);
void fq_poly_bcell_set(fq_poly_bcell_t *, fq_poly_t);
void fq_poly_bcell_set_(fq_poly_bcell_t *, fq_poly_bcell_t *, fq_poly_bcell_t *, fq_poly_t);
int fq_poly_bcell_set_right(fq_poly_bcell_t *, fq_poly_bcell_t *);
int fq_poly_bcell_set_left(fq_poly_bcell_t *, fq_poly_bcell_t *);
void fq_poly_bcell_clear(fq_poly_bcell_t *);

void fq_poly_btree_init(fq_poly_btree_t *, const fq_ctx_t *);
void fq_poly_btree_set(fq_poly_btree_t *, fq_poly_bcell_t *);
void fq_poly_btree_clear(fq_poly_btree_t *);

#endif
