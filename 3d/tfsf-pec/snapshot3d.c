/* Program 9.8 (modified for TFSF demo) - snapshot3d.c
   Records 2D snapshots of the Ez field over the constant-y plane
   (x-z plane through the center of the grid), matching Figure 9.15.

   Binary file format (compatible with Appendix D raw2movie.m):
     float dim1  = size_x  (number of x points = SizeX-1)
     float dim2  = size_y  (number of z points = SizeZ-1)
     floats data written row-major: for each pp (z, top to bottom),
       all mm (x, left to right)
   The MATLAB reader does:
     reshape(data, size_x, size_y)  -> size_x columns, size_y rows
     transpose -> size_y rows become columns
     flipud    -> flip vertically so pp=0 is at bottom             */

#include "fdtd-macro.h"
#include <stdio.h>
#include <stdlib.h>

static int temporalStride = -2, frameY = 0, startTime;
static char basename[80];

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
void snapshot3d(Grid *g) {
  int mm, nn, pp;
  float dim1, dim2, temp;
  char filename[100];
  FILE *out;

  if (temporalStride == -1)
    return;

  if (temporalStride < -1) {
    fprintf(stderr,
            "snapshot3d: snapshot3dInit must be called before snapshot3d.\n"
            "            Temporal stride must be set to a positive value.\n");
    exit(-1);
  }

  if (Time >= startTime && (Time - startTime) % temporalStride == 0) {

    sprintf(filename, "%s-y.%d", basename, frameY++);
    out = fopen(filename, "wb");

    /* dim1 = horizontal axis (x), dim2 = vertical axis (z) */
    dim1 = (float)(SizeX - 1);
    dim2 = (float)(SizeZ - 1);
    fwrite(&dim1, sizeof(float), 1, out);
    fwrite(&dim2, sizeof(float), 1, out);

    nn = SizeY / 2; /* mid-y plane */

    /* pp (z) is outer loop (top to bottom), mm (x) is inner loop */
    for (pp = SizeZ - 2; pp >= 0; pp--)
      for (mm = 0; mm < SizeX - 1; mm++) {
        temp = (float)Ez(mm, nn, pp);
        fwrite(&temp, sizeof(float), 1, out);
      }

    fclose(out);
  }

  return;
} /* end snapshot3d() */