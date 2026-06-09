#ifndef DIVISORS_H
#define DIVISORS_H

#include <stdlib.h>

typedef struct IntArr {
    int *data;
    size_t size;
    size_t cpcity;
} IntArr_t;

void divs_init(IntArr_t *divs);
int divs_resize(IntArr_t *divs, size_t new_size);
int divs_factor(int num, IntArr_t *divs);
void divs_free(IntArr_t *divs);

#endif
