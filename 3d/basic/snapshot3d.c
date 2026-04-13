/* Program 9.8 - snapshot3d.c
   Functions used to record 2D snapshots of the Ex field.
   At the appropriate time steps, two snapshots are taken:
   one over a constant-x plane and another over a constant-y plane.
   These snapshots are written to separate binary files. */

#include <stdio.h>
#include <stdlib.h>
#include "fdtd-macro.h"

static int temporalStride = -2, frameX = 0, frameY = 0, startTime;
static char basename[80];

/* ------------------------------------------------------------------ */
/* Initialization                                                      */
/* ------------------------------------------------------------------ */
void snapshot3dInit(Grid *g) {

    int choice;

    printf("Do you want 2D snapshots of the 3D grid? (1=yes, 0=no) ");
    scanf("%d", &choice);
    if (choice == 0) {
        temporalStride = -1;
        return;
    }

    printf("Duration of simulation is %d steps.\n", MaxTime);
    printf("Enter start time and temporal stride: ");
    scanf(" %d %d", &startTime, &temporalStride);
    printf("Enter the base name: ");
    scanf(" %s", basename);

    return;
} /* end snapshot3dInit() */

/* ------------------------------------------------------------------ */
/* Snapshot function -- called once per time step                     */
/* ------------------------------------------------------------------ */
void snapshot3d(Grid *g) {
    int mm, nn, pp;
    float dim1, dim2, temp;
    char filename[100];
    FILE *out;

    /* ensure temporal stride is set to a reasonable value */
    if (temporalStride == -1) {
        return;
    }
    if (temporalStride < -1) {
        fprintf(stderr,
                "snapshot3d: snapshot3dInit must be called before snapshot3d.\n"
                "            Temporal stride must be set to a positive value.\n");
        exit(-1);
    }

    /* take snapshot only when temporal conditions are met */
    if (Time >= startTime &&
        (Time - startTime) % temporalStride == 0) {

        /******** Write the constant-x slice ********/
        sprintf(filename, "%s-x.%d", basename, frameX++);
        out = fopen(filename, "wb");

        /* write dimensions as floats */
        dim1 = SizeY;
        dim2 = SizeZ;
        fwrite(&dim1, sizeof(float), 1, out);
        fwrite(&dim2, sizeof(float), 1, out);

        /* write Ex data at the mid-x plane */
        mm = (SizeX - 1) / 2;
        for (pp = SizeZ - 1; pp >= 0; pp--)
            for (nn = 0; nn < SizeY; nn++) {
                temp = (float)Ex(mm, nn, pp);
                fwrite(&temp, sizeof(float), 1, out);
            }

        fclose(out);

        /******** Write the constant-y slice ********/
        sprintf(filename, "%s-y.%d", basename, frameY++);
        out = fopen(filename, "wb");

        /* write dimensions as floats */
        dim1 = SizeX - 1;
        dim2 = SizeZ;
        fwrite(&dim1, sizeof(float), 1, out);
        fwrite(&dim2, sizeof(float), 1, out);

        /* write Ex data at the mid-y plane */
        nn = SizeY / 2;
        for (pp = SizeZ - 1; pp >= 0; pp--)
            for (mm = 0; mm < SizeX - 1; mm++) {
                temp = (float)Ex(mm, nn, pp);
                fwrite(&temp, sizeof(float), 1, out);
            }

        fclose(out);
    }

    return;
} /* end snapshot3d() */
