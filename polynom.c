#include "polynom.h"

enum state {
    STATE_INIT,
    STATE_COEFF,
    STATE_POW,
    STATE_SIGN,
    STATE_VAR,
    STATE_MUL,
    STATE_CARET,
    STATE_POW_1ASTRX,
    STATE_POW_2ASTRX
};



bool issign(char c) {
    return (c == '+' || c == '-');
}

int sign_mul(char c) {
    if (c == '-') return -1;
    return 1;
}

int poly_init(polynom_t *poly) {
    if (!poly) return 0;

    poly->coeffs = calloc(1, sizeof(int));
    if (!poly->coeffs)
        return 0;

    poly->capacity = 1;   // coeffs[0] exists
    return 1;
}

int poly_maxpow_init(polynom_t *poly, const char * const poly_s) {
    size_t max = get_max_pow(poly_s);

    size_t cpcity = max + 1; // one more than max (pows 0-n -> n+1)
    
    int *tmp = calloc(cpcity, sizeof(int));
    if (!tmp) return 0;

    poly->coeffs = tmp;
    poly->capacity = cpcity;

    return 1;
}

void poly_free(polynom_t *poly) {
    free(poly->coeffs);
}

int poly_resize(polynom_t *poly, size_t new_capacity) {
    if (!poly) return 0;
    if (new_capacity <= poly->capacity)
        return 1;

    int *tmp = realloc(poly->coeffs,
                       new_capacity * sizeof(*tmp));
    if (!tmp)
        return 0;

    memset(tmp + poly->capacity,
           0,
           (new_capacity - poly->capacity) * sizeof(*tmp));

    poly->coeffs = tmp;
    poly->capacity = new_capacity;

    return 1;
}

int poly_write(polynom_t *poly, term_t *term) {
    if (!poly || !term)
        return 0;
    
    poly->coeffs[term->pow] += term->sign * term->coeff;
    return 1;
}

// write at a position, automatic resize
int poly_resize_write(polynom_t *poly, term_t *term) {
    if (!poly || !term)
        return 0;

    size_t needed = term->pow + 1;

    if (needed > poly->capacity) {
        if (!poly_resize(poly, needed))
            return 0;
    }

    poly->coeffs[term->pow] += term->sign * term->coeff;
    return 1;
}

int parse_polynom(const char * const polynom_s, polynom_t *poly) {
    if (!polynom_s || !poly) return 0;
    
    enum state next_state = STATE_INIT;

    term_t term = {.coeff = 0, .pow = 0, .sign = 1};
    char var = '\0';
    int pos = 0;
    
    const char * poly_ptr = polynom_s;
    
    char c;
    while (true) {
        c = *(poly_ptr++);
        switch (next_state) {

        case STATE_INIT:
        
            if (isdigit(c)) {
                next_state = STATE_COEFF;
                term.coeff = CONV_DIGIT(c);
            }

            else if (isalpha(c)) {
                next_state = STATE_VAR;
                term.coeff = 1;
                var = c;
            }

            else if (issign(c)) {
                next_state = STATE_SIGN;
                term.sign *= sign_mul(c);
            }

            else if (c == '\0') {
                return 1;
            }

            else {
                inv_seq_err(polynom_s, pos);
                return 0;
            }
            
            ++pos;
            break;
        
        case STATE_COEFF:
        
            while (isdigit(c)) {
                term.coeff = CONCAT_DIGIT(term.coeff, 10, CONV_DIGIT(c));
                
                c = *poly_ptr++;
                ++pos; 
            }

            if (c == '*') {
                next_state = STATE_MUL;
            }

            else if (c == '\0') {
                term.pow = 0;
                if(!poly_write(poly, &term)) goto error;
                term_rst(&term);

                return 1;
            }

            else if (isalpha(c)) {
                if (!is_var_uniq(&var, &c)) {
                    inv_seq_err(polynom_s, pos);
                    fprintf(stderr, "Single-variable expression only\n");
                    return 0;
                }
                next_state = STATE_VAR;
            }

            else if (issign(c)) {
                next_state = STATE_SIGN;

                term.sign *= sign_mul(c);
                term.pow = 0;

                if(!poly_write(poly, &term)) goto error;
                term_rst(&term);
            }

            else {
                inv_seq_err(polynom_s, pos);
                return 0;
            }

            ++pos;
            break;
        
        case STATE_POW:

            while (isdigit(c)) {
                    term.pow = CONCAT_DIGIT(term.pow, 10, CONV_DIGIT(c));
                    
                    c = *poly_ptr++;
                    ++pos; 
            }
            
            if (issign(c)) {
                next_state = STATE_SIGN;
                // write down the term
                if(!poly_write(poly, &term)) goto error;
                // rst
                term_rst(&term);

                // write down the next term sign
                term.sign *= sign_mul(c);
            }

            else if (c == '\0') {
                if(!poly_write(poly, &term)) goto error;
                return 1;
            }

            else {
                inv_seq_err(polynom_s, pos);
                return 0;
            }
            
            ++pos;
            break;
        
        case STATE_SIGN:

            while (issign(c)) {
                    term.sign *= sign_mul(c);
                    
                    c = *poly_ptr++;
                    ++pos; 
            }

            if (isdigit(c)) {
                next_state = STATE_COEFF;

                // reset coeff by setting the next first digit
                term.coeff = CONV_DIGIT(c);
            }

            else if (isalpha(c)) {
                if (!is_var_uniq(&var, &c)) {
                    inv_seq_err(polynom_s, pos);
                    fprintf(stderr, "Single-variable expression only\n");
                    return 0;
                }
        
                next_state = STATE_VAR;
                term.coeff = 1;
            }

            else {
                inv_seq_err(polynom_s, pos);
                return 0;
            }

            ++pos;
            break;

        case STATE_VAR:
            
            if (isdigit(c)) {
                next_state = STATE_POW;
                // rst pow
                term.pow = CONV_DIGIT(c);
            }

            else if (c == '*') {
                next_state = STATE_POW_1ASTRX;
            }

            else if (c == '^') {
                next_state = STATE_CARET;
            }

            else if (c == '\0') {
                term.pow = 1;

                if(!poly_write(poly, &term)) goto error;
                return 1;
            }

            else if (issign(c)) {
                next_state = STATE_SIGN;

                term.pow = 1;

                if(!poly_write(poly, &term)) goto error;
                term_rst(&term);

                term.sign *= sign_mul(c);
            }

            else {
                inv_seq_err(polynom_s, pos);
                return 0;
            }

            ++pos;
            break;
        
        case STATE_MUL:
            
            if (isalpha(c)) {
                if (!is_var_uniq(&var, &c)) {
                    inv_seq_err(polynom_s, pos);
                    fprintf(stderr, "Single-variable expression only\n");
                    return 0;
                }
                next_state = STATE_VAR;
            }

            else {
                inv_seq_err(polynom_s, pos);
                return 0;
            }

            ++pos;
            break;

        case STATE_CARET:

            if (isdigit(c)) {
                next_state = STATE_POW;

                term.pow = CONV_DIGIT(c);
            }

            else {
                inv_seq_err(polynom_s, pos);
                return 0;
            }

            ++pos;
            break;

        case STATE_POW_1ASTRX:

            if (c == '*') {
                next_state = STATE_POW_2ASTRX;
            }

            else {
                inv_seq_err(polynom_s, pos);
                return 0;
            }

            ++pos;
            break;
        
        case STATE_POW_2ASTRX:

            if (isdigit(c)) {
                next_state = STATE_POW;
                
                term.pow = CONV_DIGIT(c);
            }

            else {
                inv_seq_err(polynom_s, pos);
                return 0;
            }

            ++pos;
            break;

        error:
            fprintf(stderr, "Memory error\n");
            return 0;
        default:
            break;
        }
    }

    return 1;
}

void inv_seq_err(const char *seq, int pos) {
    fprintf(stderr, "Unexpected character sequence\n");
    fprintf(stderr, "%s\n", seq);
    for (int i = 0; i < pos; i++) {
        printf(" ");
    }
    printf("^\n");
}

void term_rst(term_t *trm) {
    if (trm) {
        trm->coeff = 0;
        trm->pow = 0;
        trm->sign = 1;
    }
}

bool is_var_uniq(char *ref, char *var) {
    if (*ref == '\0') {
        *ref = *var;
        return true;
    }
    else if (*ref != *var) {
        return false;
    }
    return true;
}

size_t get_max_pow(const char * const polynm_s) {
    enum state next_state = STATE_INIT;
    size_t max_pow = 0;

    size_t curr_pow;
    const char *poly_ptr = polynm_s;
    char c;
    while(true) {
        c = *poly_ptr++;
        switch (next_state) {
        
        case STATE_INIT:
            
            while (!isalpha(c) && c != '\0') {
                c = *poly_ptr++;
            }
            
            if (c == '\0') {
                return max_pow;
            }

            next_state = STATE_VAR;
        
            break;

        case STATE_VAR:
            
            if (issign(c)) {
                next_state = STATE_INIT;

                curr_pow = 1;
                CMP_ASSIGN_POW(curr_pow, max_pow);
            }

            else {
                while (!isdigit(c)) {
                    if (c == '\0') return max_pow;
                    c = *poly_ptr++;
                }

                next_state = STATE_POW;

                curr_pow = CONV_DIGIT(c);
            }
            break;
        
        case STATE_POW:
            
            while (isdigit(c)) {
                curr_pow = CONCAT_DIGIT(curr_pow, 10, CONV_DIGIT(c));
                c = *poly_ptr++;
            }

            CMP_ASSIGN_POW(curr_pow, max_pow);

            if (c == '\0') {
                return max_pow;
            }

            else {
                next_state = STATE_INIT;
            }

            break;

        default:
            break;
        }
    }
}
