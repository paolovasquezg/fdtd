#include <math.h>
#include <stdio.h>

#define SIZE 200

int main() {
  double ez[SIZE] = {0.}, hy[SIZE] = {0.}, imp0 = 377.0;
  int qTime, maxTime = 200, mm;

  char filename[100] = "add.dat";
  FILE *snapshots;

  snapshots = fopen(filename, "w");

  for (qTime = 0; qTime < maxTime; qTime++) {

    hy[SIZE - 1] = hy[SIZE - 2];

    for (mm = 0; mm < SIZE - 1; mm++)
      hy[mm] = hy[mm] + (ez[mm + 1] - ez[mm]) / imp0;

    ez[0] = ez[1];

    for (mm = 1; mm < SIZE; mm++)
      ez[mm] = ez[mm] + (hy[mm] - hy[mm - 1]) * imp0;

    ez[50] += exp(-(qTime - 30.) * (qTime - 30.) / 100.);

    if (qTime % 2 == 0) {
      for (mm = 0; mm < SIZE; mm++)
        fprintf(snapshots, "%g ", ez[mm]);
      fprintf(snapshots, "\n");
    }
  }

  fclose(snapshots);

  return 0;
}