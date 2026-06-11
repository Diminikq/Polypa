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

// divide fact.remainder by d
int try_extract_root(const polynom_t *poly, factor_result_t *fact, int d, size_t *mult) {
    if (!poly || !fact) return 0;

    div_result_t res;
    if (!div_res_alloc(poly->capacity, &res)) {
        div_res_free(&res);
        return 0;
    }

    size_t multiplic = 0;
    while (horner_eval(&res.quotient, d) == 0) {
            if (!horner_divide(&res.quotient, d, &fact->remainder)) goto err;
            if (!poly_copy(&res.quotient, &fact->remainder)) goto err;
            multiplic ++;
    }
    if (mult) {
        *mult = multiplic;
    }
    goto exit;
    err:
        div_res_free(&res);
        return 0;
    exit:
        div_res_free(&res);
        return 1;
}

// allocates fact as needed
int horner_factor(const polynom_t *poly, IntArr_t *candidates, factor_result_t *fact) {
    if (!poly || !candidates || !fact) return 0;

    size_t poly_maxdeg = poly->capacity - 1;
    
    // there can be at most poly_degree roots
    if (!root_pair_alloc(fact->root_pairs, poly_maxdeg)) {
        root_pair_free(fact->root_pairs);
        return 0;
    }


    size_t multiplic;
    int div;
    for (size_t d = 0; d < candidates->size; d++) {

        multiplic = 0;
        div = candidates->data[d];
        
        if (!try_extract_root(poly, fact, div, &multiplic)) goto error;
        
        if (multiplic > 0) {
            if(!root_pair_append(&fact->root_pairs, multiplic, div)) goto error;
        }

        if (!try_extract_root(poly, fact, -div, &multiplic)) goto error;
        
        if (multiplic > 0) {
            if(!root_pair_append(&fact->root_pairs, multiplic, -div)) goto error;
        }
    }
    goto exit;

    error:
        root_pair_free(fact->root_pairs);
        return 0;
    exit:
        root_pair_free(fact->root_pairs);
        return 1;
}

int horner_normalise(const polynom_t *poly,
                     polynom_t *normalized,
                     root_t *root)
{
    if (!poly || !normalized)
        return 0;

    if (is_zero_polynomial(poly)) {
        // define canonical zero polynomial
        if (!poly_alloc(normalized, 1))
            return 0;

        normalized->coeffs[0] = 0;

        if (root) {
            root->root = 0;
            root->multiplicity = poly->capacity; // or special value
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

    if (root && zero_cnt > 0) {
        root->root = 0;
        root->multiplicity = zero_cnt;
    }

    return 1;
}

int poly_copy(polynom_t *dst, const polynom_t *src)
{
    if (dst->capacity != src->capacity) return 0;


    for (size_t i = 0; i < src->capacity; i++)
        dst->coeffs[i] = src->coeffs[i];

    return 1;
}

int root_pair_append(root_pairs_t *pairs, size_t muliplic, int root) {
    if (!pairs) return 0;

    if (pairs->root_pairs_size == pairs->root_pairs_cpcity) {
        size_t new_cap = pairs->root_pairs_cpcity == 0 ? 4 : pairs->root_pairs_cpcity * 2;

        root_t *tmp = realloc(pairs->pairs, new_cap * sizeof(root_t));
        if (!tmp) return 0;

        pairs->root_pairs_cpcity = new_cap;
        pairs->pairs = tmp;
    }

    pairs->pairs[pairs->root_pairs_size].root = root;
    pairs->pairs[pairs->root_pairs_size].multiplicity = muliplic;
    ++pairs->root_pairs_size;
    
    return 1;
}

int root_pair_alloc(root_pairs_t *pairs, size_t cpcity) {
    if (!pairs) return 0;

    pairs->pairs = malloc(cpcity * sizeof(root_t));
    if (!pairs->pairs) return 0;
    pairs->root_pairs_cpcity = cpcity;
    return 1;
}

void root_pair_free(root_pairs_t *pairs) {
    if (!pairs) return;

    free(pairs->pairs);
    pairs->root_pairs_cpcity = 0;
    pairs->root_pairs_size = 0;
}
