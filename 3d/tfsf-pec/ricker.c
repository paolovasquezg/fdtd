/* Program 8.10 - ricker.c
   Ricker wavelet source function (traveling-wave version).
   ezInc() takes both time and location as arguments. */

#include "ezinc.h"

static double cdtds, ppw = 0;

/* Initialize source-function variables */
void ezIncInit(Grid *g) {
    printf("Enter the points per wavelength for Ricker source: ");
    scanf(" %lf", &ppw);
    cdtds = Cdtds;
    return;
}

/* Calculate source function at given time and location */
double ezInc(double time, double location) {
    double arg;

    if (ppw <= 0) {
        fprintf(stderr,
                "ezInc: ezIncInit() must be called before ezInc.\n"
                "       Points per wavelength must be positive.\n");
        exit(-1);
    }

    arg = M_PI * ((cdtds * time - location) / ppw - 1.0);
    arg = arg * arg;

    return (1.0 - 2.0 * arg) * exp(-arg);
}
