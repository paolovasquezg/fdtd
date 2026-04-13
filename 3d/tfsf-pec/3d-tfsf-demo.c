/* Program 9.9 - 3d-tfsf-demo.c
   3D simulation with a TFSF boundary.
   A plane wave polarized in z and propagating in x is introduced
   via the TFSF boundary.  A PEC sphere scatterer can optionally
   be present (selected at runtime inside gridInit). */

#include "fdtd-alloc.h"
#include "fdtd-macro.h"
#include "fdtd-proto.h"

int main()
{
    Grid *g;

    ALLOC_1D(g, 1, Grid);   /* allocate memory for grid structure */
    gridInit(g);             /* initialize 3D grid (with optional PEC sphere) */

    tfsfInit(g);             /* initialize TFSF boundary */
    abcInit(g);              /* initialize ABC */
    snapshot3dInit(g);       /* initialize snapshots */

    /* do time stepping */
    for (Time = 0; Time < MaxTime; Time++) {
        updateH(g);          /* update magnetic fields */
        tfsf(g);             /* apply corrections at TFSF boundary */
        updateE(g);          /* update electric fields */
        abc(g);              /* apply ABC */
        snapshot3d(g);       /* take a snapshot (if appropriate) */
    } /* end of time-stepping */

    return 0;
}
