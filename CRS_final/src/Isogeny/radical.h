#ifndef _RADICAL_H_
#define _RADICAL_H_

#include <stdio.h>
#include <stdlib.h>

#include <gmp.h>
#include <flint/fmpz.h>
#include <flint/fq.h>

#include "../EllipticCurves/models.h"
#include "../EllipticCurves/memory.h"

void fq_nth_root_trick(fq_t, fq_t, fmpz_t, const fq_ctx_t);
void fq_nth_root_trick_ui(fq_t, fq_t, slong, const fq_ctx_t);

void radical_isogeny_3(TN_curve_t *, TN_curve_t *, fmpz_t);
void radical_isogeny_5(TN_curve_t *, TN_curve_t *, fmpz_t);
void radical_isogeny_7(TN_curve_t *, TN_curve_t *, fmpz_t);

#endif

