/* fdtd-proto.h
   Function prototypes for the 3D FDTD simulation (chapter 9).
   Every function takes a single pointer to a Grid structure.
   Based on Program 8.13 (fdtd-proto2.h), extended for 3D. */

#ifndef _FDTD_PROTO_H
#define _FDTD_PROTO_H

#include "fdtd-grid1.h"

/* ABC */
void abcInit(Grid *g);
void abc(Grid *g);

/* Grid initialization */
void gridInit(Grid *g);

/* Field updates */
void updateH(Grid *g);
void updateE(Grid *g);

/* Source function */
void ezIncInit(Grid *g);
double ezInc(double time, double location);

/* Snapshots */
void snapshot3dInit(Grid *g);
void snapshot3d(Grid *g);

#endif /* _FDTD_PROTO_H */
