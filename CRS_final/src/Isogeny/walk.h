#ifndef _WALK_H_
#define _WALK_H_

#include <stdio.h>
#include <stdlib.h>

#include <gmp.h>
#include <flint/fmpz.h>
#include <flint/fq.h>

#include "../EllipticCurves/models.h"
#include "../EllipticCurves/memory.h"

#include "radical.h"
#include "velu.h"

#include "../EllipticCurves/arithmetic.h"
#include "../EllipticCurves/pretty_print.h"

int walk_rad(MG_curve_t *, MG_curve_t *, fmpz_t, fmpz_t);
int walk_velu(MG_curve_t *, MG_curve_t *, fmpz_t, fmpz_t);

#endif

