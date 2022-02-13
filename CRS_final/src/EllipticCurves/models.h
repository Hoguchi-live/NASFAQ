#ifndef _MODELS_H_
#define _MODELS_H_

#include <stdio.h>
#include <stdlib.h>

#include "auxiliary.h"

#include <gmp.h>
#include <flint/fmpz.h>
#include <flint/fq.h>

/*********************************************
 Short Weierstrass curves structure
*********************************************/
typedef struct SW_curve_t{

	const fq_ctx_t *F;	// base field
	fq_t a, b;		// curve parameters
} SW_curve_t;

/*********************************************
 Short Weierstrass points structure
*********************************************/
typedef struct SW_point_t{

	fq_t x, y, z;	// coordinates
	SW_curve_t *E;	// base curve
} SW_point_t;

/*********************************************
 Montgomery curves structure
*********************************************/
typedef struct MG_curve_t{

	const fq_ctx_t *F;
	fq_t A, B;
} MG_curve_t;

/*********************************************
 Montgomery points structure
*********************************************/
typedef struct MG_point_t{

	MG_curve_t *E;	// base curve
	fq_t X, Z;	// coordinates
} MG_point_t;

/*********************************************
 Tate normal curves structure
*********************************************/
typedef struct TN_curve_t{

	const fq_ctx_t *F;	// base field
	fmpz_t l;		// torsion of (0,0)
	fq_t b, c;		// coordinates
} TN_curve_t;

#endif

