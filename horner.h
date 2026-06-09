#ifndef HORNER_H
#define HORNER_H

#include <ctype.h>

#include "polynom.h"


int horner_eval(const polynom_t *poly, int val);

size_t trunc_zeroes(const polynom_t *poly);

//int horner_zeroes(polynom_t *poly, int *divisors, int *zeroes);

#endif
