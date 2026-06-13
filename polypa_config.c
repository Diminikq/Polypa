
#include "polypa_config.h"


int parse_option(const char *st) {
    if (strcmp(st, "-e") == 0) return OPT_EVAL;
    else if (strcmp(st, "-z") == 0) return OPT_ZERO;
    else if (strcmp(st, "-p") == 0) return OPT_PARSE;
    else if (strcmp(st, "-d") == 0) return OPT_DIVIDE;
    else if (strcmp(st, "-f") == 0) return OPT_FACTOR;
    else if (strcmp(st, "-h") == 0) return OPT_HELP;
    else if (strcmp(st, "--") == 0) return OPT_POLY;
    else if (strcmp(st, "-v") == 0) return OPT_VERBOSE;
    else if (strcmp(st, "-a") == 0) return OPT_ASCD;
    else return OPT_UNKNOWN;
}

void config_init(config_t *config) {
    config->divide_flag = false;
    config->eval_flag = false;
    config->factor_flag = false;
    config->parse_flag = false;
    config->zero_flag = false;
    config->verbose_flag = false;
    config->asc_order = false;

    config->action_cnt = 0;

    config->poly = NULL;
}

void help_print(void) {
    printf("Usage: ./polypa [operation] [value] [modifiers] -- polynomial\n");
    printf(
        "-e [value] = evaluate polynomial at value\n"
        "-d [value] = divide polynomial by (x-value) linear term\n"
        "-f = try to factor out polynomial into linear terms\n"
        "-z = find integer roots of polynomial\n"
        "-p = parse polynomial into (implicitly descending) power-ordered coefficient format\n"
        "-v = verbose\n"
        "-a = ascending printing order specifier\n"
        "-- [polynomial] = mark polynomial\n"
    );
}

int configure(config_t *config, int argc, const char *argv[]) {
    if (!config || !argv) return 1;

    for (int arg = 1; arg < argc; arg++) {
        char * junk; // control for safe

        switch (parse_option(argv[arg])) {

        case OPT_PARSE:
            config->parse_flag = true;
            config->action_cnt++;
            break;
        
        case OPT_EVAL:
            config->eval_flag = true;
            
            if (++arg >= argc) {
                fprintf(stderr, "Missing evaluation value\n");
                return 0;
            }
            config->eval_val = strtol(argv[arg], &junk, 10); // base 10
            if (*junk != '\0') {
                fprintf(stderr, "Not INT: %s\n", argv[arg]);
                return 0;
            }
            config->action_cnt++;
            break;

        case OPT_ZERO:
            config->zero_flag = true;
            config->action_cnt++;
            break;

        case OPT_DIVIDE:
            config->divide_flag = true;
            
            if (++arg >= argc) {
                fprintf(stderr, "Missing division value\n");
                return 0;
            }
            config->divide_val = strtol(argv[arg], &junk, 10); // base 10
            if (*junk != '\0') {
                fprintf(stderr, "Not INT: %s\n", argv[arg]);
                return 0;
            }
            config->action_cnt++;
            break;

        case OPT_FACTOR:
            config->factor_flag = true;
            config->action_cnt++;
            break;
        
        case OPT_HELP:
            help_print();
            return -1;

        case OPT_POLY:
            
            if (++arg >= argc) {
                fprintf(stderr, "Missing polynomial\n");
                return 0;
            }
            config->poly = concat_args(arg, argc, argv);
            if (!config->poly) {
                free(config->poly);
                return 0;
            }
            return 1;
        
        case OPT_VERBOSE:
            config->verbose_flag = true;
            break;

        case OPT_ASCD:
            
            config->asc_order = true;

            break;

        case OPT_UNKNOWN:
            fprintf(stderr, "unknown option: %s\n", argv[arg]);
            fprintf(stderr, "Rerun with -h for help\n");
            return 0;

        default:
            break;
        }
    }
    return 1;
}

int exec_opts(config_t *config, polynom_t *poly) {
    if (!config || !poly) return 0;

    if (config->action_cnt > 1) return -1;

    bool is_first_print = true;

    // the first variable is the only one
    // else parser error earlier
    char var = first_variable_used(config->poly);
    

    if (config->parse_flag) {
        FIRST_PRINT_ENDLINE(is_first_print);
        if (config->verbose_flag) {
            print_polynom(poly, var, config->asc_order);
        }

        else {
            print_polynom(poly, '\0', config->asc_order);
        }
        printf("\n");
    }

    if (config->eval_flag) {
        FIRST_PRINT_ENDLINE(is_first_print);
        if (config->verbose_flag) {
            printf("P(%d) = ", config->eval_val);
        }
        int val = horner_eval(poly, config->eval_val);
        printf("%d\n", val);
    }

    if (config->divide_flag) {
        FIRST_PRINT_ENDLINE(is_first_print);
        div_result_t res;

        if (!div_res_alloc(poly->capacity, &res)) {
            div_res_free(&res);
            return 0;
        }

        horner_divide(poly, config->divide_val, &res);

        if (config->verbose_flag) {
            
            printf("(");
            print_polynom_verbose(poly, var, config->asc_order);
            printf(") / ");
            
            print_linear(config->divide_val, var);
            printf(" = ");
            print_polynom(&res.quotient, var, config->asc_order);
            printf(" with the remainder of %d\n", res.remainder);
            
        }

        else {
            print_polynom(&res.quotient, '\0', config->asc_order);
            printf("| ");
            printf("%d\n", res.remainder);
        }

        div_res_free(&res);
    }

    if (config->factor_flag || config->zero_flag) {

        if (is_zero_polynomial(poly)) {

            if (config->factor_flag) {
                FIRST_PRINT_ENDLINE(is_first_print);
                if (config->verbose_flag) {
                    printf("Zero polynomial: all divide it\n");
                }

                else {
                    printf("0\n");
                }
            }

            if (config->zero_flag) {
                FIRST_PRINT_ENDLINE(is_first_print);
                if (config->verbose_flag) {
                    printf("P(x) = 0 <=> forall x (zero polynomial)\n");
                }

                else {
                    printf("All integers (zero polynomial)\n");
                }
            }

            return 1;
        }

        // will store normalized polynomial
        polynom_t normalized;

        // will store constant term divisors
        IntArr_t divisors;
        IntArr_init(&divisors);

        // will store division result
        factor_result_t res;
        factor_result_init(&res);

        // there can be at most max deg roots, thats why capacity
        // could be done by reallocing, but slower
        if (!root_pair_alloc(&res.root_pairs, poly->capacity)) goto fz_error;

        // normalize poly, write the multiplicity of 0 if > 0;
        if (!horner_normalize(poly, &normalized, &res.root_pairs)) goto fz_error;

        // alloc space for remainder
        if (!poly_alloc(&res.remainder, normalized.capacity)) goto fz_error;

        unsigned const_term = abs(normalized.coeffs[0]);

        if (!divs_factor(const_term, &divisors)) goto fz_error;

        if (!horner_factor(&normalized, &divisors, &res)) goto fz_error;


        bool is_constant = (res.remainder.capacity == 1);
        bool is_const_one = (is_constant && (res.remainder.coeffs[0] == 1));

        if (config->factor_flag) {

            FIRST_PRINT_ENDLINE(is_first_print);

            if (config->verbose_flag) {
                print_polynom_verbose(poly, var, config->asc_order);
                printf(" = ");
            }
            
            // remainder is a constant but not 1
            if (is_constant) {
                if (!is_const_one) {
                    print_polynom_verbose(poly, var, config->asc_order);
                }
            }

            for (size_t idx = 0; idx < res.root_pairs.root_pairs_size; idx++) {
                print_linear(res.root_pairs.pairs[idx].root, var);

                // power greater than 1
                if (res.root_pairs.pairs[idx].multiplicity > 1) {
                    printf("^%zu", res.root_pairs.pairs[idx].multiplicity);
                }
            }

            // remainder not a constant, print it last
            if (!is_constant) {
                printf("(");
                print_polynom(&res.remainder, var, config->asc_order);
                printf(")");
            }
            printf("\n");
        
        }

        if (config->zero_flag) {

            FIRST_PRINT_ENDLINE(is_first_print);

            if (res.root_pairs.root_pairs_size == 0) {

            if (config->verbose_flag) {
                printf("Polynomial has no integer roots\n");
            }

            else printf("None\n");
            
            goto fz_exit;
        }

        if (config->verbose_flag) {
            printf("P(x) = 0 <=> x = ");
        }

        // else print all roots
        for (size_t idx = 0; idx < res.root_pairs.root_pairs_size; idx++) {

            printf("%d ", res.root_pairs.pairs[idx].root);
        }
        printf("\n");
        }

        goto fz_exit;

        fz_error:
            poly_free(&normalized);
            IntArr_free(&divisors);
            factor_result_free(&res);

            return 0;

        fz_exit:
            poly_free(&normalized);
            IntArr_free(&divisors);
            factor_result_free(&res);
    }

    return 1;
}

char *concat_args(int arg_start, int argc, const char *argv[]) {
    size_t total_len = 1; // '\0'

    for (int arg = arg_start; arg < argc; arg++) {
        total_len += strlen(argv[arg]);
    }

    char *result = malloc(total_len);
    if (!result) {
        return NULL;
    }

    char *p = result;

    for (int arg = arg_start; arg < argc; arg++) {
        size_t len = strlen(argv[arg]);
        memcpy(p, argv[arg], len);
        p += len;
    }

    *p = '\0';

    return result;
}
