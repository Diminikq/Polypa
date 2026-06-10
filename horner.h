#ifndef HORNER_H
#define HORNER_H

#include <ctype.h>

#include "polynom.h"
#include "divisors.h"

typedef struct div_res { 
    polynom_t quotient;
    int remainder;
} div_result_t;

typedef struct {
    IntArr_t roots;
    polynom_t remainder;
} factor_result_t;


int horner_eval(const polynom_t *poly, int val);

int horner_divide(const polynom_t *poly, int val, div_result_t *res);

size_t trunc_zeroes(const polynom_t *poly);

//int horner_zeroes(polynom_t *poly, int *divisors, int *zeroes);

int div_res_alloc(size_t dividend_size, div_result_t *res);
void div_res_free(div_result_t *res);

#endif
