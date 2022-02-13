#ifndef _dh_H_
#define _dh_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../../src/EllipticCurves/arithmetic.h"
#include "../../src/EllipticCurves/models.h"
#include "../../src/EllipticCurves/memory.h"
#include "../../src/Isogeny/walk.h"
#include "../../src/Exchange/keygen.h"
#include "../../src/Exchange/setup.h"
#include "../../src/Exchange/info.h"

#include <gmp.h>
#include <flint/fmpz.h>
#include <flint/fq.h>

int apply_key(MG_curve_t *, MG_curve_t *, key__t *, cfg_t *);

#endif
