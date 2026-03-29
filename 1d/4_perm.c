#include <math.h>
#include <stdio.h>

#define SIZE 200

int main() {
  double ez[SIZE] = {0.}, hy[SIZE] = {0.}, epsR[SIZE], imp0 = 377.0;
  int qTime, maxTime = 450, mm;
  int frame = 0;

  char filename[100] = "perm.dat";
  FILE *snapshots;

  /* set relative permittivity */
  for (mm = 0; mm < SIZE; mm++)
    if (mm < 100)
      epsR[mm] = 1.0;
    else
      epsR[mm] = 9.0;

  snapshots = fopen(filename, "w");

  for (qTime = 0; qTime < maxTime; qTime++) {

    /* update magnetic field */
    for (mm = 0; mm < SIZE - 1; mm++)
      hy[mm] = hy[mm] + (ez[mm + 1] - ez[mm]) / imp0;

    /* correction for Hy adjacent to TFSF boundary */
    hy[49] -= exp(-(qTime - 30.0) * (qTime - 30.0) / 100.0) / imp0;

    /* simple ABC for ez[0] */
    ez[0] = ez[1];

    /* update electric field */
    for (mm = 1; mm < SIZE; mm++)
      ez[mm] = ez[mm] + (hy[mm] - hy[mm - 1]) * imp0 / epsR[mm];

    /* correction for Ez adjacent to TFSF boundary */
    ez[50] += exp(-(qTime + 0.5 - (-0.5) - 30.0) *
                  (qTime + 0.5 - (-0.5) - 30.0) / 100.0);

    if (qTime % 2 == 0) {
      for (mm = 0; mm < SIZE; mm++)
        fprintf(snapshots, "%g ", ez[mm]);
      fprintf(snapshots, "\n");
    }
  }

  fclose(snapshots);

  return 0;
}