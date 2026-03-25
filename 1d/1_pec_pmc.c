// 1d -> Δx and Ez

#include <math.h>
#include <stdio.h>

#define SIZE 200

int main() {
  double ez[SIZE] = {0.}, hy[SIZE] = {0.}, imp0 = 377.0;
  int qTime, maxTime = 250, mm;

  char filename[100] = "source.data";
  FILE *snapshot;

  snapshot = fopen(filename, "w");

  for (qTime = 0; qTime < maxTime; qTime++) {

    for (mm = 0; mm < SIZE - 1; mm++)
      hy[mm] = hy[mm] + (ez[mm + 1] - ez[mm]) / imp0;

    for (mm = 1; mm < SIZE; mm++)
      ez[mm] = ez[mm] + (hy[mm] - hy[mm - 1]) * imp0;

    ez[0] = exp(-(qTime - 30.) * (qTime - 30.) / 100.);

    // snapshots
    if (qTime % 10 == 0) {

      for (mm = 0; mm < SIZE; mm++)
        fprintf(snapshot, "%g ", ez[mm]);

      fprintf(snapshot, "\n");
    }
  }

  fclose(snapshot);
  return 0;
}

// ez[mm] = ez[mm] + (hy[mm] - hy[mm - 1]) * 377;
// hy[mm] = hy[mm] + (ez[mm + 1] - ez[mm]) / 377;