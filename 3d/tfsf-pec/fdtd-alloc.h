/* fdtd-alloc.h
   Memory allocation macros for 1D, 2D, and 3D arrays.
   Based on Program 8.4 (fdtd-alloc1.h) extended with ALLOC_3D for chapter 9. */

#ifndef _FDTD_ALLOC_H
#define _FDTD_ALLOC_H

#include <stdio.h>
#include <stdlib.h>

/* 1D allocation */
#define ALLOC_1D(PNTR, NUM, TYPE)                                             \
    PNTR = (TYPE *)calloc(NUM, sizeof(TYPE));                                 \
    if (!PNTR) {                                                              \
        perror("ALLOC_1D");                                                   \
        fprintf(stderr,                                                        \
                "Allocation failed for " #PNTR ". Terminating...\n");        \
        exit(-1);                                                             \
    }

/* 2D allocation (stored as flat 1D array) */
#define ALLOC_2D(PNTR, NUMX, NUMY, TYPE)                                     \
    PNTR = (TYPE *)calloc((NUMX) * (NUMY), sizeof(TYPE));                    \
    if (!PNTR) {                                                              \
        perror("ALLOC_2D");                                                   \
        fprintf(stderr,                                                        \
                "Allocation failed for " #PNTR ". Terminating...\n");        \
        exit(-1);                                                             \
    }

/* 3D allocation (stored as flat 1D array) */
#define ALLOC_3D(PNTR, NUMX, NUMY, NUMZ, TYPE)                              \
    PNTR = (TYPE *)calloc((NUMX) * (NUMY) * (NUMZ), sizeof(TYPE));          \
    if (!PNTR) {                                                              \
        perror("ALLOC_3D");                                                   \
        fprintf(stderr,                                                        \
                "Allocation failed for " #PNTR ". Terminating...\n");        \
        exit(-1);                                                             \
    }

#endif /* _FDTD_ALLOC_H */
