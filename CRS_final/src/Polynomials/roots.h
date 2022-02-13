/// @file roots.h
#ifndef _ROOTS_H_
#define _ROOTS_H_

#include <gmp.h>
#include <flint/fmpz.h>
#include <flint/fq.h>
#include <flint/fq_poly.h>
#include <flint/fq_poly_factor.h>

//int fq_poly_anyroot(fq_t, fq_poly_t, const fq_ctx_t);
int fq_sqr_from_polyfact(fq_t, fq_t, const fq_ctx_t);

#endif

