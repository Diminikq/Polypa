#ifndef POLYPA_CONFIG_H
#define POLYPA_CONFIG_H

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

#include "polynom.h"
#include "horner.h"

// if the first output line is being printed,
// dont print a newline first
#define FIRST_PRINT_ENDLINE(f) do { \
    if (f) { \
        f = false; \
    } \
    else { \
        printf("\n"); \
    } \
} while (0);

enum options {
    OPT_EVAL,
    OPT_PARSE,
    OPT_ZERO,
    OPT_DIVIDE,
    OPT_FACTOR,
    OPT_HELP,
    OPT_POLY,
    OPT_VERBOSE,
    OPT_ASCD,
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

    // to limit the number of actions
    int action_cnt;

    char *poly;
} config_t;

void config_init(config_t *config);

int configure(config_t *config, int argc, const char *argv[]);
int exec_opts(config_t *config, polynom_t *poly);

int parse_option(const char *st);
void help_print(void);

char* concat_args(int arg_start, int argc, const char *argv[]);



#endif
