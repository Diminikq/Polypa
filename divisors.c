/**
 * Author: Dominik Makuka
 * VUT FIT Brno
 */

#include "divisors.h"

void IntArr_init(IntArr_t *int_arr) {
    if (!int_arr) return;

    int_arr->data = NULL;
    int_arr->cpcity = 0;
    int_arr->size = 0;
}

int divs_factor(unsigned num, IntArr_t *divs) {
    if (!divs || num <= 0) return 0;

    for (unsigned d = 1; d * d <= num; d++) {

        if (num % d == 0) {

            // ensure capacity
            if (divs->size == divs->cpcity) {
                size_t new_cap = divs->cpcity ? divs->cpcity * 2 : 8;
                if (!IntArr_resize(divs, new_cap)) {
                    IntArr_free(divs);
                    return 0;
                }
            }

            divs->data[divs->size++] = d;

            if (d != num / d) {
                divs->data[divs->size++] = num / d;
            }
        }
    }

    return 1;
}

int IntArr_resize(IntArr_t *int_arr, size_t new_size) {
    if (!int_arr) return 0;

    int *tmp = realloc(int_arr->data, new_size * sizeof(int));
    if (!tmp) return 0;
    int_arr->data = tmp;
    int_arr->cpcity = new_size;
    
    return 1;
}

void IntArr_free(IntArr_t *int_arr) {
    free(int_arr->data);
    int_arr->data = NULL;
    int_arr->cpcity = 0;
    int_arr->size = 0;
}
