#include <math.h>
#include <stdio.h>

#define SIZE 200

int main() {
  double ez[SIZE], hy[SIZE];
  double imp0 = 377.0;
  int qTime, maxTime = 450, mm;
  char basename[80] = "sim", filename[100];
  int frame = 0;
  FILE *snapshot;

  for (mm = 0; mm < SIZE; mm++) {
    ez[mm] = 0.0;
    hy[mm] = 0.0;
  }

  for (qTime = 0; qTime < maxTime; qTime++) {

    hy[SIZE - 1] = hy[SIZE - 2];

    for (mm = 0; mm < SIZE - 1; mm++)
      hy[mm] = hy[mm] + (ez[mm + 1] - ez[mm]) / imp0;

    hy[49] -= exp(-(qTime - 30.0) * (qTime - 30.0) / 100.0) / imp0;

    // Hy_inc (0,q) -> -1/no * e^-((q-0-30)^2 / 100)

    ez[0] = ez[1];

    for (mm = 1; mm < SIZE; mm++)
      ez[mm] = ez[mm] + (hy[mm] - hy[mm - 1]) * imp0;

    ez[50] += exp(-(qTime + 0.5 - (-0.5) - 30.0) *
                  (qTime + 0.5 - (-0.5) - 30.0) / 100.0);

    //  Ez_inc (-1/2, q+1+/2) -> e^-((q+1/2-(-1/2) -30 ) ^ 2 /100 )

    if (qTime % 10 == 0) {
      sprintf(filename, "%s.%d", basename, frame++);
      snapshot = fopen(filename, "w");
      for (mm = 0; mm < SIZE; mm++)
        fprintf(snapshot, "%g\n", ez[mm]);
      fclose(snapshot);
    }

  } /* end of time stepping */

  return 0;
}