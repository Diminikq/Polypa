/**
 * Author: Dominik Makuka
 * VUT FIT Brno
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "polynom.h"
#include "polypa_config.h"

/* todo:

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

