#include "horner.h"

int horner_eval(const polynom_t *poly, int val) {
    int result = 0;

    for (size_t idx = poly->capacity; idx > 0; idx--) {
        result = result * val + poly->coeffs[idx - 1]; 
    }
    return result;
}

size_t trunc_zeroes(const polynom_t *poly) {
    if (!poly) return 0;

    size_t zero_cnt = 0;

    while (poly->coeffs[zero_cnt] == 0 && zero_cnt < poly->capacity) {
        zero_cnt ++;
    }
    
    return zero_cnt;
}
