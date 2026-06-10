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

int horner_divide(const polynom_t *poly, int val, div_result_t *res) {
    if (!poly || !res) return 0;

    size_t res_capacity = poly->capacity - 1; 
    int result = poly->coeffs[res_capacity];

    for (size_t idx = res_capacity; idx > 0; idx--) {
        if (res->quotient.coeffs && idx <= res->quotient.capacity) {
            res->quotient.coeffs[idx - 1] = result;
        }

        result = result * val + poly->coeffs[idx - 1];
        
    }
    
    res->remainder = result;

    return 1;
}

int div_res_alloc(size_t dividend_capaciy, div_result_t *res) {
    size_t res_capacity = dividend_capaciy - 1;

    res->quotient.coeffs = malloc(res_capacity * sizeof(int));
    if (!res->quotient.coeffs) return 0;

    res->quotient.capacity = res_capacity;
    return 1;
}

void div_res_free(div_result_t *res) {
    free(res->quotient.coeffs);
    res->quotient.capacity = 0;
}
