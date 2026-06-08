#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "polynom.h"
#include "horner.h"

/* todo: 
    -e [val] evaluate the polynomial at val
    -l [file] load from file
    -z find integer zeroes if exist
    -d divide by a linear term
    -f factor out the polynomial
    -h help for options
*/
int main(int argc, const char *argv[]){
    if (argc != 2) {
        fprintf(stderr, "Invalid parameter count\n");
        return 1;
    }
    fprintf(stdout, "%s\n", argv[1]);

    polynom_t poly;
    if (!poly_maxpow_init(&poly, argv[1])) return 1;
    
    if (!parse_polynom(argv[1], &poly)) {
        poly_free(&poly);
        return 1;
    }
    for (size_t idx = poly.capacity; idx > 0; idx--) {
        printf("%d ", poly.coeffs[idx-1]);
    }
    printf("\n");

    printf("%d\n", horner_eval(&poly, 5));

    poly_free(&poly);
    return 0;
}

