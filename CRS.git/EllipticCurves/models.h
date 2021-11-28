#ifndef _MODELS_H_
#define _MODELS_H_

#include <gmp.h>
#include <flint/fmpz.h>
#include <flint/fq.h>

typedef struct SW_curve{

	const fq_ctx_t *F;	// base field
	//fmpz_t p;		// characteristic
	//slong d;		// degree

	fq_t a, b;		// curve parameters
} SW_curve;
#endif
