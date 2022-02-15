#ifndef _VELU_H_
#define _VELU_H_

#include "../EllipticCurves/models.h"
#include "../EllipticCurves/memory.h"
#include "../EllipticCurves/arithmetic.h"
#include "../Polynomials/multieval.h"

void _init_lengths(uint *, uint *, uint *, uint);
void _F0pF1pF2_F0mF1pF2(fq_poly_t *, fq_poly_t *, MG_point_t, const fq_ctx_t);

void KPS(MG_point_t *, MG_point_t *, MG_point_t *, MG_point_t, uint, uint, uint, uint);
void xISOG(fq_t *, MG_point_t, uint, MG_point_t *, MG_point_t *, MG_point_t *, uint, uint, uint);

void isogeny_from_torsion(fq_t *, MG_point_t, uint);

#endif

