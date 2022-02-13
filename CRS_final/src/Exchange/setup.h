#ifndef _setup_H_
#define _setup_H_

#include <stdio.h>
#include <stdlib.h>

#include "../../src/EllipticCurves/models.h"
#include "../../src/EllipticCurves/memory.h"

#include <gmp.h>
#include <flint/fmpz.h>
#include <flint/fq.h>

#define BASE_p "12037340738208845034383383978222801137092029451270197923071397735408251586669938291587857560356890516069961904754171956588530344066457839297755929645858769"
#define BASE_q "12037340738208845034383383978222801137092029451270197923071397735408251586669938291587857560356890516069961904754171956588530344066457839297755929645858769"

#define BASE_A	"10861338504649280383859950140772947007703646408372831934324660566888732797778932142488253565145603672591944602210571423767689240032829444439469242521864171"
#define BASE_B	"1"
#define BASE_t	"-147189550172528104900422131912266898599387555512924231762107728432541952979290"

#define NB_PRIMES 24
#define MAX_EXTENSION_DEGREE 9

/*********************************************
   l-primes structure
*********************************************/
typedef struct lprime_t{

	fmpz_t l;

	uint type; 		// Unused (0), Radical (1) or Velu (2)
	uint lbound, hbound;	// Bounds for the walk
	uint r;			// Working extension degree
	uint bkw;		// 1 if backward walking possible
} lprime_t ;

/*********************************************
   Global configuration structure
*********************************************/
typedef struct cfg_t{

	//// Base curve parameters
	MG_curve_t *E;		//base curve

	//// l-primes parameters
	uint nb_primes; 		// number of l-primes used
	lprime_t *lprimes;		// the l-primes ordered in an lprime_t array

	//// Base field and its extensions up to degree 9
	fq_ctx_t *fields;

	//// Random seed
	uint seed;
} cfg_t;


void lprime_init(lprime_t *);
lprime_t *lprime_init_();
void lprime_set(lprime_t *, fmpz_t, uint, uint, uint, uint, uint);
void lprime_clear(lprime_t *);

cfg_t *cfg_init_set();
void cfg_print(cfg_t *);
void cfg_clear(cfg_t *);

#endif

