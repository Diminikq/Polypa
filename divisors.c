
#include "divisors.h"

void divs_init(IntArr_t *divs) {
    if (!divs) return;

    divs->data = NULL;
    divs->cpcity = 0;
    divs->size = 0;
}

int divs_factor(int num, IntArr_t *divs) {
    if (!divs) return 0;

    // declare here
    // avoids checking d*d=num on every iteration 
    // if num is a perfect square, don t count d twice
    // move the check after for loop to save time
    size_t d = 0;

    for (; d * d < (size_t) num; d++) {
        if (num % d == 0) {
            if (divs->size == divs->cpcity) {
                if(!divs_resize(divs, divs->cpcity * 2)) {
                    divs_free(divs);
                    return 0;
                };
            }

            divs->data[divs->size++] = d;
            divs->data[divs->size++] = num / d;
        }
    }

    if (d * d == (size_t)num) {
        if (divs->size == divs->cpcity) {
            if(!divs_resize(divs, ++divs->cpcity)) {
                divs_free(divs);
                return 0;
            };
        }

        divs->data[divs->size++] = d;
    }
    
    return 1;
}

int divs_resize(IntArr_t *divs, size_t new_size) {
    if (!divs) return 0;

    int *tmp = realloc(divs->data, new_size * sizeof(int));
    if (!tmp) return 0;
    divs->data = tmp;
    divs->cpcity = new_size;
    
    return 1;
}

void divs_free(IntArr_t *divs) {
    free(divs->data);
    divs->cpcity = 0;
    divs->data = 0;
}
