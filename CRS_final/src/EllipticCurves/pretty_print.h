#ifndef _PRETTY_PRINT_H_
#define _PRETTY_PRINT_H_

#include <string.h>

#include <gmp.h>
#include <flint/fmpz.h>
#include <flint/fq.h>

#include "models.h"

void SW_curve_print(SW_curve_t *);
void SW_point_print(SW_point_t *);
void MG_curve_print(MG_curve_t *);
void MG_point_print(MG_point_t *);
void TN_curve_print(TN_curve_t *);

void SW_curve_print_full(SW_curve_t *);
void SW_point_print_full(SW_point_t *);
void MG_curve_print_full(MG_curve_t *);
void MG_point_print_full(MG_point_t *);

//compact

#endif

