/**
 * Author: Dominik Makuka
 * VUT FIT Brno
 */

#ifndef HORNER_H
#define HORNER_H

#include <ctype.h>

#include "polynom.h"
#include "divisors.h"

typedef struct div_res { 
    polynom_t quotient;
    int remainder;
} div_result_t;

// stores a root and its multiplicity
typedef struct {
    int root;
    size_t multiplicity;
} root_t;

typedef struct root_pairs {
    root_t *pairs;
    size_t root_pairs_size;
    size_t root_pairs_cpcity;
} root_pairs_t;

// remainder, 
typedef struct {
    root_pairs_t root_pairs;
    polynom_t remainder;
} factor_result_t;

int horner_normalize(const polynom_t *poly, polynom_t *normalized, root_pairs_t *pairs);

int horner_eval(const polynom_t *poly, int val);

int horner_divide(const polynom_t *poly, int val, div_result_t *res);
int horner_factor(const polynom_t *poly, IntArr_t *candidates, factor_result_t *fact);

int try_extract_root(polynom_t *current, int d, size_t *mult);

size_t trunc_zeroes(const polynom_t *poly);

//int horner_zeroes(polynom_t *poly, int *divisors, int *zeroes);

int div_res_alloc(size_t dividend_size, div_result_t *res);
void div_res_free(div_result_t *res);

void root_pair_init(root_pairs_t *pairs);
int root_pair_append(root_pairs_t *pairs, size_t muliplic, int root);
int root_pair_alloc(root_pairs_t *pairs, size_t cpcity);
void root_pair_free(root_pairs_t *pairs);

void factor_result_init(factor_result_t *res);
void factor_result_free(factor_result_t *res);


#endif
