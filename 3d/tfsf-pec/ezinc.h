/* ezinc.h
   Header file for the Ricker wavelet source function.
   Based on Program 4.16 (ezInc2.h), adapted for chapter 9
   to include fdtd-macro.h instead of a chapter-specific header. */

#ifndef _EZINC_H
#define _EZINC_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "fdtd-macro.h"

void ezIncInit(Grid *g);
double ezInc(double time, double location);

#endif /* _EZINC_H */
