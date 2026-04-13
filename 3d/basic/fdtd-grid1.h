/* Program 8.3 - fdtd-grid1.h
   Header file that defines the Grid structure.
   Contains pointers for all possible field and coefficient arrays.
   Which pointers are actually used depends on the grid type. */

#ifndef _FDTD_GRID1_H
#define _FDTD_GRID1_H

enum GRIDTYPE {oneDGrid, teZGrid, tmZGrid, threeDGrid};

struct Grid {
    double *hx,  *chxh, *chxe;
    double *hy,  *chyh, *chye;
    double *hz,  *chzh, *chze;
    double *ex,  *cexe, *cexh;
    double *ey,  *ceye, *ceyh;
    double *ez,  *ceze, *cezh;
    int sizeX, sizeY, sizeZ;
    int time, maxTime;
    int type;
    double cdtds;
};

typedef struct Grid Grid;

#endif /* _FDTD_GRID1_H */
