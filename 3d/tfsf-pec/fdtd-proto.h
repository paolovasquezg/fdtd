/* fdtd-proto.h
   Function prototypes for the 3D FDTD simulation (chapter 9).
   Extended to include TFSF boundary and 1D auxiliary grid. */

#ifndef _FDTD_PROTO_H
#define _FDTD_PROTO_H

#include "fdtd-grid1.h"

/* ABC */
void abcInit(Grid *g);
void abc(Grid *g);

/* Grid initialization */
void gridInit(Grid *g);
void gridInit1d(Grid *g);   /* 1D auxiliary grid for TFSF */

/* Field updates */
void updateH(Grid *g);
void updateE(Grid *g);

/* Source function */
void ezIncInit(Grid *g);
double ezInc(double time, double location);

/* TFSF boundary */
void tfsfInit(Grid *g);
void tfsf(Grid *g);

/* Snapshots */
void snapshot3dInit(Grid *g);
void snapshot3d(Grid *g);

#endif /* _FDTD_PROTO_H */
