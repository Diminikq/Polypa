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

typedef struct {
    root_t *data;
    size_t size;
    size_t capacity;
} root_list_t;

// remainder, 
typedef struct {
    root_list_t roots;
    polynom_t remainder;
} factor_result_t;

int horner_normalise(const polynom_t *poly, polynom_t *normalized, root_t *root);

int horner_eval(const polynom_t *poly, int val);

int horner_divide(const polynom_t *poly, int val, div_result_t *res);
int horner_factor(const polynom_t *poly, factor_result_t *fact);

//static int try_extract_root(polynom_t *current, int r, size_t *mult);
//static int add_root(factor_result_t *out, int r, size_t mult);
int copy_poly(polynom_t *dst, const polynom_t *src);

size_t trunc_zeroes(const polynom_t *poly);

//int horner_zeroes(polynom_t *poly, int *divisors, int *zeroes);

int div_res_alloc(size_t dividend_size, div_result_t *res);
void div_res_free(div_result_t *res);


#endif
