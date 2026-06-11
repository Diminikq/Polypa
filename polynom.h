/**
 * Author: Dominik Makuka
 * VUT FIT Brno
 */

#ifndef POLYNOM_H
#define POLYNOM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define CONV_DIGIT(c) ((c) - '0')

#define CONCAT_DIGIT(num, base, dig) ((num) * (base) + (dig))

#define CMP_ASSIGN_POW(curr, max) do { \
        if ((curr) > (max)) (max) = (curr);\
    } while (0)

typedef struct polynom {
    size_t capacity;
    int *coeffs; // pos = power, value = coeff
} polynom_t;

typedef struct term {
    size_t pow;
    int coeff;
    int sign;
} term_t;

/**
 * @brief true if c is a sign (+/-)
 * @param c a char
 */
bool issign(char c);

/**
 * @brief return (+1/-1) depending on sign
 * @param c a char
 * @return -1 if c=='-' else 1
 */
int sign_mul(char c); // returns -1 if sign is -

void term_rst(term_t *trm);

void inv_seq_err(const char *seq, int pos);

/**
 * @brief initializes a polynom_t to zero polynomial
 * poly.capacity = 1, poly.coeffs[0] = 0
 * @param poly polynom
 * @return 1 on success, else 0
 */
int poly_init(polynom_t *poly);

/**
 * @brief initializes a polynom_t according to max degree in string
 * @param poly polynomial
 * @param poly_s string containing polynomial
 * @return 1 on success, else 0
 */
int poly_maxpow_init(polynom_t *poly, const char * const poly_s);

void poly_free(polynom_t *poly);
int poly_resize(polynom_t *poly, size_t len);
int poly_write(polynom_t *poly, term_t *term);
int poly_resize_write(polynom_t *poly, term_t *term);
int poly_alloc(polynom_t *poly, size_t len);

int poly_copy(polynom_t *dst, const polynom_t *src);

size_t get_max_pow(const char * const polynm_s);
int parse_polynom(const char * const polynom_s, polynom_t *poly);

bool is_var_uniq(char *ref, char *var);
bool is_zero_polynomial(const polynom_t *poly);

void print_linear(int val, char var);
void print_polynom(polynom_t *poly, char var, bool ascd_order);
char first_variable_used(const char * s);

#endif
