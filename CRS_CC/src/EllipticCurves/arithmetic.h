#ifndef _ARITHMETIC_H_
#define _ARITHMETIC_H_

#include "string.h"
#include "stdbool.h"

#include "memory.h"
#include "models.h"
#include "auxiliary.h"
#include "pretty_print.h"

#include <gmp.h>
#include <flint/fmpz.h>
#include <flint/fq.h>

#include "../Polynomials/roots.h"

/*********************************************
   Base field embbeding
*********************************************/
void MG_curve_update_field(MG_curve_t *, MG_curve_t *, const fq_ctx_t *);
void MG_curve_update_field_(MG_curve_t *, const fq_ctx_t *);

/*********************************************
 Elliptic curves
*********************************************/
void SW_j_invariant(fq_t *, SW_curve_t *);
void MG_j_invariant(fq_t *, MG_curve_t *);
void TN_j_invariant(fq_t *, TN_curve_t *);

/*********************************************
 Points on elliptic curves
*********************************************/
void SW_point_isinfinity(bool *, SW_point_t *);
void MG_point_isinfinity(bool *, MG_point_t *);
bool MG_point_isinfinity_(MG_point_t *);
int SW_point_isvalid(bool *, SW_point_t *);
int MG_point_isvalid(bool *, MG_point_t *);
void MG_point_isinfty(bool *, MG_point_t *);
void MG_point_normalize(MG_point_t *);

/*********************************************
 Random torsion point generation
*********************************************/
void SW_point_rand_ninfty(SW_point_t *);
void MG_point_rand_ninfty(MG_point_t *, flint_rand_t);
void MG_point_rand_ninfty_nsquare(MG_point_t *, flint_rand_t);

/*********************************************
 Montgomery curve arithmetic
*********************************************/
int MG_curve_normalize(MG_curve_t *);
int MG_rec_y(fq_t, MG_point_t *);
void MG_xADD(MG_point_t *, MG_point_t, MG_point_t, MG_point_t);
void MG_xDBL(MG_point_t *, MG_point_t);
void MG_xDBL_const(MG_point_t *, MG_point_t ,const fq_t);

/*********************************************
 Montgomery ladder
*********************************************/
void MG_ladder_rec(MG_point_t *, MG_point_t *, fmpz_t, MG_point_t, const fq_ctx_t *);
void MG_ladder(MG_point_t *x0, fmpz_t k, MG_point_t P);
void MG_ladder_iter(MG_point_t *, MG_point_t *, fmpz_t, MG_point_t, fq_ctx_t *);
void MG_ladder_iter_(MG_point_t *, fmpz_t, MG_point_t *);

/*********************************************
 Torsion
*********************************************/
void MG_curve_trace(fmpz_t);
void MG_curve_card_base(fmpz_t, MG_curve_t *);
void MG_curve_card_ext(fmpz_t, MG_curve_t *, fmpz_t r);
int MG_curve_rand_torsion(MG_point_t *, fmpz_t, fmpz_t);
int MG_curve_rand_torsion_(MG_point_t *, fmpz_t, fmpz_t);

/*********************************************
 Tate normal curve and Montgomery conversion
*********************************************/
void MG_get_TN(TN_curve_t *, MG_curve_t *, MG_point_t *, fmpz_t);
int TN_get_MG(MG_curve_t *, TN_curve_t *);
#endif

