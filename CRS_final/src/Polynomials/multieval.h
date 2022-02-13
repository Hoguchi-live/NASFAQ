/// @file multieval.h
#ifndef _MULTIEVAL_H_
#define _MULTIEVAL_H_

#include <gmp.h>
#include <flint/fmpz.h>
#include <flint/fq.h>
#include <flint/fq_poly.h>

#include "binary_trees.h"

void remainderCell(fq_poly_bcell_t *, fq_t *, uint, uint, const fq_ctx_t *);
void remainderTree(fq_poly_btree_t *, fq_t *, uint, const fq_ctx_t *);
void fq_poly_multieval(fq_t *, fq_t *, fq_poly_t, uint, const fq_ctx_t *);
#endif

