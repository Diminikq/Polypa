/**
 * Author: Dominik Makuka
 * VUT FIT Brno
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "polynom.h"
#include "horner.h"
#include "divisors.h"
#include "roots.h"

enum options {
    OPT_EVAL,
    OPT_PARSE,
    OPT_ZERO,
    OPT_DIVIDE,
    OPT_FACTOR,
    OPT_HELP,
    OPT_POLY,
    OPT_VERBOSE,
    OPT_ORDER,
    OPT_UNKNOWN
};

typedef struct {
    bool eval_flag;
    int eval_val;

    bool divide_flag;
    int divide_val;

    bool asc_order; // printing order

    bool parse_flag;
    bool zero_flag;
    bool factor_flag;
    bool verbose_flag;

    char *poly;
} config_t;

void config_init(config_t *config);

int configure(config_t *config, int argc, const char *argv[]);
int exec_opts(config_t * config, polynom_t *poly);

int parse_option(const char *st);
void help_print(void);

char* concat_args(int arg_start, int argc, const char *argv[]);

/* todo:
    in divide, print_polynomial instead of string
    only after print_polynomial works imlicitly

    space is treated as error in parser, fix

    static bool first print
*/
int main(int argc, const char *argv[]){

    if (argc == 1) {
        printf("Nothing to do, rerun with -h for help \n");
        return 0;
    }

    config_t config;
    config_init(&config);

    int conf_out = configure(&config, argc, argv);
    if (conf_out == -1) return 0; // help option
    if (conf_out == 0) return 1; // error


    if (!config.poly) return 1;

    polynom_t poly;
    poly_init(&poly);

    if (!poly_maxpow_init(&poly, config.poly)) goto m_error;

    if (!parse_polynom(config.poly, &poly)) goto m_error;

    if (!exec_opts(&config, &poly)) {
        fprintf(stderr, "Error executing options\n");
        goto m_error;
    }

    goto m_exit;

    m_error:
        poly_free(&poly);
        free(config.poly);
        return 1;
    
    m_exit:
        poly_free(&poly);
        free(config.poly);
        return 0;
}

int parse_option(const char *st) {
    if (strcmp(st, "-e") == 0) return OPT_EVAL;
    else if (strcmp(st, "-z") == 0) return OPT_ZERO;
    else if (strcmp(st, "-p") == 0) return OPT_PARSE;
    else if (strcmp(st, "-d") == 0) return OPT_DIVIDE;
    else if (strcmp(st, "-f") == 0) return OPT_FACTOR;
    else if (strcmp(st, "-h") == 0) return OPT_HELP;
    else if (strcmp(st, "--") == 0) return OPT_POLY;
    else if (strcmp(st, "-v") == 0) return OPT_VERBOSE;
    else if (strcmp(st, "-o") == 0) return OPT_ORDER;
    else return OPT_UNKNOWN;
}

void config_init(config_t *config) {
    config->divide_flag = false;
    config->eval_flag = false;
    config->factor_flag = false;
    config->parse_flag = false;
    config->zero_flag = false;
    config->verbose_flag = false;
    config->asc_order = true;

    config->poly = NULL;
}

void help_print(void) {
    printf("Usage: ./polypa [flag] [value] -- polynomial\n");
    printf(
        "-e [value] = evaluate polynomial at value\n"
        "-d [value] = divide polynomial by (x-value) linear term\n"
        "-f = try to factor out polynomial into linear terms\n"
        "-z = find integer roots of polynomial\n"
        "-p = parse polynomial into (implicitly ascending) power-ordered coefficient format\n"
        "-v = verbose\n"
        "-o [a/d] = printing order specifier - ascending/descending\n"
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
            break;

        case OPT_ZERO:
            config->zero_flag = true;
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
            break;

        case OPT_FACTOR:
            config->factor_flag = true;
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

        case OPT_ORDER:
            if (++arg >= argc) {
                fprintf(stderr, "Missing order\n");
                return 0;
            }

            char ord = argv[arg][0];

            if (strlen(argv[arg]) > 1 || (ord != 'a' && ord != 'd')) {
                fprintf(stderr, "invalid speicifier: %s\n", argv[arg]);
                return 0;
            }
            
            config->asc_order = (ord == 'a');

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

    // the first variable is the only one
    // else parser error earlier
    char var = first_variable_used(config->poly);
    

    if (config->parse_flag) {
        printf("\n");
        print_polynom(poly, '\0', config->asc_order);
        printf("\n");
    }

    if (config->eval_flag) {
        printf("\n");
        if (config->verbose_flag) {
            printf("P(%d) = ", config->eval_val);
        }
        int val = horner_eval(poly, config->eval_val);
        printf("%d\n", val);
    }

    if (config->divide_flag) {
        div_result_t res;

        if (!div_res_alloc(poly->capacity, &res)) {
            div_res_free(&res);
            return 0;
        }

        printf("\n");
        horner_divide(poly, config->divide_val, &res);

        if (config->verbose_flag) {
            
            printf("(%s) / ", config->poly);
            print_linear(config->divide_flag, var);
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
                if (config->verbose_flag) {
                    printf("Zero polynomial: all divide it\n");
                }

                else {
                    printf("0\n");
                }
            }

            if (config->zero_flag) {
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

        if (config->factor_flag) {

            printf("\n");

            if (config->verbose_flag) {
                printf("%s = ", config->poly);
            }
            
            for (size_t idx = 0; idx < res.root_pairs.root_pairs_size; idx++) {
                print_linear(res.root_pairs.pairs[idx].root, var);
                printf("^%zu", res.root_pairs.pairs[idx].multiplicity);
            }

            print_polynom(&res.remainder, var, config->asc_order);

            printf("\n");
            printf("\n");
        
        }

        if (config->zero_flag) {

            printf("\n");

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
