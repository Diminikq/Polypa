#include "horner.h"

int horner_eval(const polynom_t *poly, int val) {
    int result = 0;

    for (size_t idx = poly->capacity; idx > 0; idx--) {
        result = result * val + poly->coeffs[idx - 1]; 
    }
    return result;
}

size_t trunc_zeroes(const polynom_t *poly) {
    if (!poly || !poly->coeffs) return 0;

    size_t zero_cnt = 0;

    while (poly->coeffs[zero_cnt] == 0 && zero_cnt < poly->capacity) {
        zero_cnt ++;
    }
    
    return zero_cnt;
}

int horner_divide(const polynom_t *poly, int val, div_result_t *res) {
    if (!poly || !poly->coeffs || !res || !res->quotient.coeffs)
        return 0;

    if (poly->capacity == 0)
        return 0;

    if (res->quotient.capacity < poly->capacity - 1)
        return 0;

    
    size_t last = poly->capacity - 1;

    int result = poly->coeffs[last];

    for (size_t i = last; i > 0; i--) {
        res->quotient.coeffs[i - 1] = result;
        result = result * val + poly->coeffs[i - 1];
    }

    res->remainder = result;

    return 1;
}

int div_res_alloc(size_t dividend_capacity, div_result_t *res) {
    if (!res || dividend_capacity == 0)
        return 0;

    // if dividend is a constant, the result will be a constant
    size_t res_capacity = dividend_capacity == 1 ? 1 : dividend_capacity - 1;

    res->quotient.coeffs = calloc(res_capacity, sizeof(int));
    if (!res->quotient.coeffs)
        return 0;

    res->quotient.capacity = res_capacity;
    res->remainder = 0;

    return 1;
}

void div_res_free(div_result_t *res) {
    free(res->quotient.coeffs);
    res->quotient.coeffs = NULL;
    res->quotient.capacity = 0;
    res->remainder = 0;
}

// divide fact.remainder by d
int try_extract_root(polynom_t *current, int d, size_t *mult) {
    if (!current) return 0;

    size_t multiplic = 0;

    div_result_t divtmp;
    if (!div_res_alloc(current->capacity, &divtmp))
        return 0;

    while (1) {
        if (!horner_divide(current, d, &divtmp))
            goto error;

        if (divtmp.remainder != 0)
            break;
        
        poly_copy(current, &divtmp.quotient);
        current->capacity--;
        ++multiplic;
        
    }

    if (mult)
        *mult = multiplic;

    div_res_free(&divtmp);
    return 1;

error:
    div_res_free(&divtmp);
    return 0;
}

// allocates fact as needed
int horner_factor(const polynom_t *poly,
                  IntArr_t *candidates,
                  factor_result_t *fact)
{
    if (!poly || !candidates || !fact)
        return 0;

    polynom_t current;
    if (!poly_alloc(&current, poly->capacity))
        return 0;

    poly_copy(&current, poly);

    for (size_t i = 0; i < candidates->size; ++i) {

        int div = candidates->data[i];
        size_t multiplic;

        if (!try_extract_root(&current, div, &multiplic))
            goto error;

        if (multiplic > 0) {
            if (!root_pair_append(&fact->root_pairs,
                                  multiplic,
                                  div))
                goto error;
        }

        if (!try_extract_root(&current, -div, &multiplic))
            goto error;

        if (multiplic > 0) {
            if (!root_pair_append(&fact->root_pairs,
                                  multiplic,
                                  -div))
                goto error;
        }
    }

    poly_copy(&fact->remainder, &current);
    // shrink the result to avoid printing zeroes without realloc
    fact->remainder.capacity = current.capacity;
    
    poly_free(&current);
    return 1;

error:
    poly_free(&current);
    return 0;
}

int horner_normalize(const polynom_t *poly,
                     polynom_t *normalized,
                     root_pairs_t *pairs)
{
    if (!poly || !normalized)
        return 0;

    if (is_zero_polynomial(poly)) {
        // define canonical zero polynomial
        if (!poly_alloc(normalized, 1))
            return 0;

        normalized->coeffs[0] = 0;

        if(!root_pair_append(pairs, poly->capacity, 0)){
            root_pair_free(pairs);
            return 0;
        }  

        return 1;
    }

    size_t zero_cnt = trunc_zeroes(poly);
    size_t new_size = poly->capacity - zero_cnt;

    if (!poly_alloc(normalized, new_size))
        return 0;

    for (size_t i = 0; i < new_size; i++) {
        normalized->coeffs[i] = poly->coeffs[i + zero_cnt];
    }

    if (pairs && zero_cnt > 0) {
        if(!root_pair_append(pairs, zero_cnt, 0)){
            root_pair_free(pairs);
            return 0;
        }        
    }

    return 1;
}

void root_pair_init(root_pairs_t *pairs) {
    if(!pairs) return;

    pairs->pairs = NULL;
    pairs->root_pairs_cpcity = 0;
    pairs->root_pairs_size = 0;
}

int root_pair_append(root_pairs_t *pairs, size_t multiplic, int root) {
    if (!pairs) return 0;

    if (pairs->root_pairs_size == pairs->root_pairs_cpcity) {
        size_t new_cap = pairs->root_pairs_cpcity == 0 ? 4 : pairs->root_pairs_cpcity * 2;

        root_t *tmp = realloc(pairs->pairs, new_cap * sizeof(root_t));
        if (!tmp) return 0;

        pairs->root_pairs_cpcity = new_cap;
        pairs->pairs = tmp;
    }

    pairs->pairs[pairs->root_pairs_size].root = root;
    pairs->pairs[pairs->root_pairs_size].multiplicity = multiplic;
    ++pairs->root_pairs_size;
    
    return 1;
}

int root_pair_alloc(root_pairs_t *pairs, size_t capacity) {
    //if (!pairs) return 0;

    pairs->pairs = calloc(capacity, sizeof(root_t));
    if (!pairs->pairs) {
        pairs->root_pairs_cpcity = 0;
        pairs->root_pairs_size = 0;
        return 0;
    }

    pairs->root_pairs_cpcity = capacity;
    pairs->root_pairs_size = 0;

    return 1;
}

void root_pair_free(root_pairs_t *pairs) {
    if (!pairs) return;

    free(pairs->pairs);
    pairs->pairs = NULL;
    pairs->root_pairs_cpcity = 0;
    pairs->root_pairs_size = 0;
}

void factor_result_init(factor_result_t *res) {
    if (!res) return;

    res->remainder.coeffs = NULL;
    res->remainder.capacity = 0;

    root_pair_init(&res->root_pairs);
}

void factor_result_free(factor_result_t *res) {
    if (!res) return;

    poly_free(&res->remainder);
    root_pair_free(&res->root_pairs);
}
