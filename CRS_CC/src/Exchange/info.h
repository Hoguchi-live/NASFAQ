#ifndef _info_H_
#define _info_H_

#include <stdio.h>
#include <stdlib.h>

#include "../../src/EllipticCurves/models.h"
#include "../../src/Exchange/keygen.h"
#include "../../src/Exchange/setup.h"

#include <gmp.h>
#include <flint/fmpz.h>
#include <flint/fq.h>

void print_verbose_walk(uint, fmpz_t, fmpz_t, int, int);
void print_verbose_walk_total_time(int);
void print_timing_json(fmpz_t, float);

#endif

