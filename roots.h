/**
 * Author: Dominik Makuka
 * VUT FIT Brno
 */


#ifndef ROOTS_H
#define ROOTS_H

#include "polynom.h"
#include "divisors.h"
#include "horner.h"


int find_int_roots(const polynom_t *poly, 
    const IntArr_t *divisors, IntArr_t *roots);

#endif
