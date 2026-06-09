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
    OPT_UNKNOWN
};

typedef struct {
    bool eval_flag;
    int eval_val;

    bool divide_flag;
    int divide_val;

    bool parse_flag;
    bool zero_flag;
    bool factor_flag;
    bool verbose_flag;

    const char *poly;
} config_t;

void config_init(config_t *config);

int configure(config_t *config, int argc, const char *argv[]);
int exec_opts(config_t * config, polynom_t *poly);

int parse_option(const char *st);
void help_print(void);

/* todo: 
    UNIX style => all args after -- polynomial
    -l [file] load from file
    -z find integer zeroes if exist
    -d divide by a linear term
    -f factor out the polynomial
*/
int main(int argc, const char *argv[]){

    config_t config;
    config_init(&config);

    int conf_out = configure(&config, argc, argv);
    if (conf_out == -1) return 0; // help option
    if (conf_out == 0) return 1; // error


    if (!config.poly) return 1;

    polynom_t poly;

    if (!poly_maxpow_init(&poly, config.poly)) return 1;
    
    if (!parse_polynom(config.poly, &poly)) {
        poly_free(&poly);
        return 1;
    }

    if (!exec_opts(&config, &poly)) {
        fprintf(stderr, "Error executing options\n");
        return 1;
    }

    poly_free(&poly);
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
    else return OPT_UNKNOWN;
}

void config_init(config_t *config) {
    config->divide_flag = false;
    config->eval_flag = false;
    config->factor_flag = false;
    config->parse_flag = false;
    config->zero_flag = false;
    config->verbose_flag = false;

    config->poly = NULL;
}

void help_print(void) {
    printf("Usage: ./polypa [flag] [value] -- polynomial\n");
    printf(
        "-e [value] = evaluate polynomial at value\n"
        "-d [value] = divide polynomial by (x-value) linear term\n"
        "-f = try to factor out the polynomial into linear terms\n"
        "-z = find integer roots of the polynomial\n"
        "-p = parse the polynomial into ascending power-ordered coefficient format\n"
        "-v = verbose"
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
                fprintf(stderr, "NAN: %s\n", argv[arg]);
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
                fprintf(stderr, "NAN: %s\n", argv[arg]);
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
            config->poly = argv[arg];
            break;
        
        case OPT_VERBOSE:
            config->verbose_flag = true;
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

    if (config->parse_flag) {
        for (size_t idx = poly->capacity; idx > 0; idx--) {
            printf("%d ", poly->coeffs[idx-1]);
        }
        printf("\n");
    }

    if (config->eval_flag) {
        if (config->verbose_flag) {
            printf("P(%d) = ", config->eval_val);
        }
        int val = horner_eval(poly, config->eval_val);
        printf("%d\n", val);
    }

    if (config->divide_flag) {
        
    }

    if (config->factor_flag) {

    }

    if (config->zero_flag) {

        // all coeffs are zero
        if (is_zero_polynomial(poly)) {

            if (config->verbose_flag) {
                printf("P(x) = 0 <=> forall x (zero polynomial)\n");
            }

            else {
                printf("All integers (zero polynomial)\n");
            }

            return 1;
        }

        IntArr_t divisors;
        IntArr_init(&divisors);

        size_t nonzero_idx = trunc_zeroes(poly);
        
        if (!divs_factor(abs(poly->coeffs[nonzero_idx]), &divisors)){
            IntArr_free(&divisors);
            return 0;
        }
        
        IntArr_t roots;
        IntArr_init(&roots);

        if(!find_int_roots(poly, &divisors, &roots)) {
            IntArr_free(&divisors);
            IntArr_free(&roots);
            return 0;
        }
        
        if (roots.size == 0 && nonzero_idx == 0) {
            if (config->verbose_flag) {
                printf("Polynomial has no integer roots\n");
            }

            else {
                printf("None\n");
            }
            return 1;
        }

        if (config->verbose_flag) {
            printf("P(x) = 0 <=> x = ");
        }

        for (size_t idx = 0; idx < roots.size; idx++) {

            printf("%d ", roots.data[idx]);
        }
        printf("\n");

        IntArr_free(&divisors);
        IntArr_free(&roots);
    }

    return 1;
}
