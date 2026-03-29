#include <math.h>
#include <stdio.h>

#define SIZE 200

int main() {
  double ez[SIZE] = {0.}, hy[SIZE] = {0.}, imp0 = 377.0;
  int qTime, maxTime = 450, mm;
  int frame = 0;

  char filename[100] = "tfsb.dat";
  FILE *snapshots;

  snapshots = fopen(filename, "w");

  for (qTime = 0; qTime < maxTime; qTime++) {

    hy[SIZE - 1] = hy[SIZE - 2];

    for (mm = 0; mm < SIZE - 1; mm++)
      hy[mm] = hy[mm] + (ez[mm + 1] - ez[mm]) / imp0;

    hy[49] -= exp(-(qTime - 30.0) * (qTime - 30.0) / 100.0) / imp0;

    ez[0] = ez[1];

    for (mm = 1; mm < SIZE; mm++)
      ez[mm] = ez[mm] + (hy[mm] - hy[mm - 1]) * imp0;

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