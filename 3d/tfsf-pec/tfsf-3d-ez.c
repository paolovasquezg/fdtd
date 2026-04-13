/* Program 9.10 - tfsf-3d-ez.c
   Three-dimensional TFSF boundary implementation.
   Assumes the incident electric field is polarized in the z direction
   and propagation is in the x direction.
   A 1D auxiliary grid is used to calculate the incident field. */

#include <string.h>       /* memcpy */
#include "fdtd-macro.h"
#include "fdtd-proto.h"
#include "fdtd-alloc.h"
#include "ezinc.h"

/* Indices for the first and last points in the total-field region */
static int
    firstX = 0, firstY, firstZ,
    lastX,      lastY,  lastZ;

static Grid *g1;   /* 1D auxiliary grid */

/* ------------------------------------------------------------------ */
/* Initialization                                                      */
/* ------------------------------------------------------------------ */
void tfsfInit(Grid *g) {

    ALLOC_1D(g1, 1, Grid);
    memcpy(g1, g, sizeof(Grid));   /* copy Cdtds, MaxTime, SizeX, ... */
    gridInit1d(g1);                /* finish 1D grid setup             */

    printf("Grid is %d by %d by %d.\n", SizeX, SizeY, SizeZ);
    printf("Enter indices for first point in TF region: ");
    scanf(" %d %d %d", &firstX, &firstY, &firstZ);
    printf("Enter indices for last point in TF region: ");
    scanf(" %d %d %d", &lastX, &lastY, &lastZ);

    ezIncInit(g);   /* initialize Ricker wavelet source */

    return;
} /* end tfsfInit() */

/* ------------------------------------------------------------------ */
/* TFSF update -- called after updateH(), before updateE()            */
/* ------------------------------------------------------------------ */
void tfsf(Grid *g) {
    int mm, nn, pp;

    if (firstX <= 0) {
        fprintf(stderr,
                "tfsf: tfsfInit must be called before tfsf.\n"
                "      Boundary location must be set to a positive value.\n");
        exit(-1);
    }

    /****** constant-x faces -- scattered-field H corrections ******/

    /* correct Hy at firstX-1/2 by subtracting Ez_inc
       coeff index is mm=firstX (book p.287 line 52) */
    mm = firstX;
    for (nn = firstY; nn <= lastY; nn++)
        for (pp = firstZ; pp < lastZ; pp++)
            Hy(mm - 1, nn, pp) -= Chye(mm, nn, pp) * Ez1G(g1, mm);

    /* correct Hy at lastX+1/2 by adding Ez_inc
       coeff index is mm=lastX (book p.287 line 58) */
    mm = lastX;
    for (nn = firstY; nn <= lastY; nn++)
        for (pp = firstZ; pp < lastZ; pp++)
            Hy(mm, nn, pp) += Chye(mm, nn, pp) * Ez1G(g1, mm);

    /****** constant-y faces -- scattered-field H corrections ******/

    /* correct Hx at firstY-1/2 by adding Ez_inc
       coeff index is nn-1 (book p.287 line 66) */
    nn = firstY;
    for (mm = firstX; mm <= lastX; mm++)
        for (pp = firstZ; pp < lastZ; pp++)
            Hx(mm, nn - 1, pp) += Chxe(mm, nn - 1, pp) * Ez1G(g1, mm);

    /* correct Hx at lastY+1/2 by subtracting Ez_inc
       coeff index is nn (book p.287 line 72) */
    nn = lastY;
    for (mm = firstX; mm <= lastX; mm++)
        for (pp = firstZ; pp < lastZ; pp++)
            Hx(mm, nn, pp) -= Chxe(mm, nn, pp) * Ez1G(g1, mm);

    /****** constant-z faces -- no H correction for Ez/Hy polarization ******/

    /****** advance the 1D auxiliary grid by one time step ******/
    updateH(g1);
    updateE(g1);
    Ez1G(g1, 0) = ezInc(TimeG(g1), 0.0);   /* hard source at node 0 */
    TimeG(g1)++;

    /****** constant-x faces -- total-field E corrections ******/

    /* correct Ez at firstX face by subtracting Hy_inc
       (book p.288 line 90: Hy1G(g1, mm-1)) */
    mm = firstX;
    for (nn = firstY; nn <= lastY; nn++)
        for (pp = firstZ; pp < lastZ; pp++)
            Ez(mm, nn, pp) -= Cezh(mm, nn, pp) * Hy1G(g1, mm - 1);

    /* correct Ez at lastX face by adding Hy_inc
       (book p.288 line 96: Hy1G(g1, mm)) */
    mm = lastX;
    for (nn = firstY; nn <= lastY; nn++)
        for (pp = firstZ; pp < lastZ; pp++)
            Ez(mm, nn, pp) += Cezh(mm, nn, pp) * Hy1G(g1, mm);

    /****** constant-y faces -- no E correction for Ez/Hy polarization ******/

    /****** constant-z faces -- total-field E corrections ******/

    /* correct Ex at firstZ face by adding Hy_inc
       (book p.288 line 108: Hy1G(g1, mm)) */
    pp = firstZ;
    for (mm = firstX; mm < lastX; mm++)
        for (nn = firstY; nn <= lastY; nn++)
            Ex(mm, nn, pp) += Cexh(mm, nn, pp) * Hy1G(g1, mm);

    /* correct Ex at lastZ face by subtracting Hy_inc
       (book p.288 line 114: Hy1G(g1, mm)) */
    pp = lastZ;
    for (mm = firstX; mm < lastX; mm++)
        for (nn = firstY; nn <= lastY; nn++)
            Ex(mm, nn, pp) -= Cexh(mm, nn, pp) * Hy1G(g1, mm);

    return;
} /* end tfsf() */
