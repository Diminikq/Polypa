/**
 * Author: Dominik Makuka
 * VUT FIT Brno
 */

#ifndef DIVISORS_H
#define DIVISORS_H

#include <stdlib.h>

typedef struct IntArr {
    int *data;
    size_t size;
    size_t cpcity;
} IntArr_t;


void IntArr_init(IntArr_t *int_arr);
int IntArr_resize(IntArr_t *int_arr, size_t new_size);
int divs_factor(unsigned num, IntArr_t *divs);
void IntArr_free(IntArr_t *int_arr);

#endif
