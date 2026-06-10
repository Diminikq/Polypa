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


bool issign(char c);
int sign_mul(char c); // returns -1 if sign is -

void term_rst(term_t *trm);

void inv_seq_err(const char *seq, int pos);

int poly_init(polynom_t *poly);
int poly_maxpow_init(polynom_t *poly, const char * const poly_s);
void poly_free(polynom_t *poly);
int poly_resize(polynom_t *poly, size_t len);
int poly_write(polynom_t *poly, term_t *term);
int poly_resize_write(polynom_t *poly, term_t *term);
int poly_alloc(polynom_t *poly, size_t len);

size_t get_max_pow(const char * const polynm_s);
int parse_polynom(const char * const polynom_s, polynom_t *poly);

bool is_var_uniq(char *ref, char *var);
bool is_zero_polynomial(polynom_t *poly);

void print_linear(int val, char var);
void print_polynom(polynom_t *poly, char var, bool ascd_order);
char first_variable_used(const char * s);

#endif
