/* Program 8.15 - grid1dez.c
   Initialization function for the 1D auxiliary grid used by the TFSF
   boundary to calculate the incident field (Ez/Hy polarization).
   The grid is terminated with a quadratically-graded lossy layer
   instead of an ABC so that reflections from the end are minimized. */

#include <math.h>
#include "fdtd-macro.h"
#include "fdtd-alloc.h"

#define NLOSS    20     /* number of lossy cells at end of 1D grid */
#define MAX_LOSS 0.35   /* maximum loss factor in lossy layer      */

void gridInit1d(Grid *g) {
    double imp0 = 377.0, depthInLayer, lossFactor;
    int mm;

    SizeX += NLOSS;     /* extend grid to accommodate lossy layer */
    Type   = oneDGrid;  /* set grid type                          */

    /* memory allocation */
    ALLOC_1D(g->hy,   SizeX - 1, double);
    ALLOC_1D(g->chyh, SizeX - 1, double);
    ALLOC_1D(g->chye, SizeX - 1, double);
    ALLOC_1D(g->ez,   SizeX,     double);
    ALLOC_1D(g->ceze, SizeX,     double);
    ALLOC_1D(g->cezh, SizeX,     double);

    /* set update coefficients
       - free space for nodes inside the 3D domain width
       - quadratically-graded loss for the absorbing tail          */
    for (mm = 0; mm < SizeX - 1; mm++) {
        if (mm < SizeX - 1 - NLOSS) {
            /* free space */
            Ceze1(mm) = 1.0;
            Cezh1(mm) = Cdtds * imp0;
            Chyh1(mm) = 1.0;
            Chye1(mm) = Cdtds / imp0;
        } else {
            /* lossy layer -- loss ramps up quadratically with depth */
            depthInLayer = mm - (SizeX - 1 - NLOSS) + 0.5;
            lossFactor   = MAX_LOSS * pow(depthInLayer / NLOSS, 2);
            Ceze1(mm) = (1.0 - lossFactor) / (1.0 + lossFactor);
            Cezh1(mm) = Cdtds * imp0 / (1.0 + lossFactor);

            depthInLayer += 0.5;
            lossFactor    = MAX_LOSS * pow(depthInLayer / NLOSS, 2);
            Chyh1(mm) = (1.0 - lossFactor) / (1.0 + lossFactor);
            Chye1(mm) = Cdtds / imp0 / (1.0 + lossFactor);
        }
    }

    return;
} /* end gridInit1d() */
