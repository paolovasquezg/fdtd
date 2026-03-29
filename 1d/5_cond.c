#include <math.h>
#include <stdio.h>

#define SIZE 200
#define LOSS 0.01

int main() {
  double ez[SIZE] = {0.}, hy[SIZE] = {0.}, ceze[SIZE], cezh[SIZE], imp0 = 377.0;
  int qTime, maxTime = 300, mm;
  int frame = 0;

  char filename[100] = "cond.dat";
  FILE *snapshots;

  /* set electric-field update coefficients */
  for (mm = 0; mm < SIZE; mm++)
    if (mm < 100) { /* free space */
      ceze[mm] = 1.0;
      cezh[mm] = imp0;
    } else { /* lossy dielectric */
      ceze[mm] = (1.0 - LOSS) / (1.0 + LOSS);
      cezh[mm] = imp0 / 9.0 / (1.0 + LOSS);
    }

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
      ez[mm] = ceze[mm] * ez[mm] + cezh[mm] * (hy[mm] - hy[mm - 1]);

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