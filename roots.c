/**
 * Author: Dominik Makuka
 * VUT FIT Brno
 */


#include "roots.h"

int find_int_roots(const polynom_t *poly,
                   const IntArr_t *divisors,
                   IntArr_t *roots) {

    if (!poly || !divisors || !roots) return 0;

    // zero is a root if absolute term is zero
    if (poly->coeffs[0] == 0) {

            if (roots->size == roots->cpcity) {
                size_t new_cap = roots->cpcity ? roots->cpcity * 2 : 8;
                if (!IntArr_resize(roots, new_cap)) return 0;
            }

            roots->data[roots->size++] = 0;
        }

    for (size_t idx = 0; idx < divisors->size; idx++) {

        int d = divisors->data[idx];

        // positive root
        if (horner_eval(poly, d) == 0) {

            if (roots->size == roots->cpcity) {
                size_t new_cap = roots->cpcity ? roots->cpcity * 2 : 8;
                if (!IntArr_resize(roots, new_cap)) return 0;
            }

            roots->data[roots->size++] = d;
        }

        // negative root
        if (horner_eval(poly, -d) == 0) {

            if (roots->size == roots->cpcity) {
                size_t new_cap = roots->cpcity ? roots->cpcity * 2 : 8;
                if (!IntArr_resize(roots, new_cap)) return 0;
            }

            roots->data[roots->size++] = -d;
        }
    }

    return 1;
}

