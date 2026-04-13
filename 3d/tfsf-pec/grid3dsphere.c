/* Program 9.11 - grid3dsphere.c
   Initialization function for a 3D grid (35 x 35 x 35).
   The user is prompted to determine if a PEC sphere of radius 8 cells
   should be present.  If not, the grid is homogeneous free space.
   Used together with the TFSF boundary (Program 9.10). */

#include "fdtd-macro.h"
#include "fdtd-alloc.h"
#include <math.h>

void gridInit(Grid *g) {
    double imp0 = 377.0;
    int mm, nn, pp;

    /* sphere parameters */
    int    m_c = 17, n_c = 17, p_c = 17, isSpherePresent;
    double m2, n2, p2, r2, radius = 8.0;

    Type    = threeDGrid;
    SizeX   = 35;
    SizeY   = 35;
    SizeZ   = 35;
    MaxTime = 300;
    Cdtds   = 1.0 / sqrt(3.0);

    printf("Is the PEC sphere present? (1=yes, 0=no) ");
    scanf(" %d", &isSpherePresent);

    /* ---------------------------------------------------------------- */
    /* Memory allocation                                                 */
    /* ---------------------------------------------------------------- */
    ALLOC_3D(g->hx,   SizeX,     SizeY - 1, SizeZ - 1, double);
    ALLOC_3D(g->chxh, SizeX,     SizeY - 1, SizeZ - 1, double);
    ALLOC_3D(g->chxe, SizeX,     SizeY - 1, SizeZ - 1, double);

    ALLOC_3D(g->hy,   SizeX - 1, SizeY,     SizeZ - 1, double);
    ALLOC_3D(g->chyh, SizeX - 1, SizeY,     SizeZ - 1, double);
    ALLOC_3D(g->chye, SizeX - 1, SizeY,     SizeZ - 1, double);

    ALLOC_3D(g->hz,   SizeX - 1, SizeY - 1, SizeZ,     double);
    ALLOC_3D(g->chzh, SizeX - 1, SizeY - 1, SizeZ,     double);
    ALLOC_3D(g->chze, SizeX - 1, SizeY - 1, SizeZ,     double);

    ALLOC_3D(g->ex,   SizeX - 1, SizeY,     SizeZ,     double);
    ALLOC_3D(g->cexe, SizeX - 1, SizeY,     SizeZ,     double);
    ALLOC_3D(g->cexh, SizeX - 1, SizeY,     SizeZ,     double);

    ALLOC_3D(g->ey,   SizeX,     SizeY - 1, SizeZ,     double);
    ALLOC_3D(g->ceye, SizeX,     SizeY - 1, SizeZ,     double);
    ALLOC_3D(g->ceyh, SizeX,     SizeY - 1, SizeZ,     double);

    ALLOC_3D(g->ez,   SizeX,     SizeY,     SizeZ - 1, double);
    ALLOC_3D(g->ceze, SizeX,     SizeY,     SizeZ - 1, double);
    ALLOC_3D(g->cezh, SizeX,     SizeY,     SizeZ - 1, double);

    /* ---------------------------------------------------------------- */
    /* Electric-field update coefficients (free space)                  */
    /* ---------------------------------------------------------------- */
    for (mm = 0; mm < SizeX - 1; mm++)
        for (nn = 0; nn < SizeY; nn++)
            for (pp = 0; pp < SizeZ; pp++) {
                Cexe(mm, nn, pp) = 1.0;
                Cexh(mm, nn, pp) = Cdtds * imp0;
            }

    for (mm = 0; mm < SizeX; mm++)
        for (nn = 0; nn < SizeY - 1; nn++)
            for (pp = 0; pp < SizeZ; pp++) {
                Ceye(mm, nn, pp) = 1.0;
                Ceyh(mm, nn, pp) = Cdtds * imp0;
            }

    for (mm = 0; mm < SizeX; mm++)
        for (nn = 0; nn < SizeY; nn++)
            for (pp = 0; pp < SizeZ - 1; pp++) {
                Ceze(mm, nn, pp) = 1.0;
                Cezh(mm, nn, pp) = Cdtds * imp0;
            }

    /* ---------------------------------------------------------------- */
    /* PEC sphere: zero all 12 E-field edges of each Yee cube whose     */
    /* center falls within 'radius' cells of (m_c, n_c, p_c).          */
    /* Squared distances are used to avoid computing square roots.      */
    /* H-field coefficients are NOT modified by the PEC.                */
    /* ---------------------------------------------------------------- */
    if (isSpherePresent) {
        r2 = radius * radius;
        for (mm = 2; mm < SizeX - 2; mm++) {
            m2 = (mm + 0.5 - m_c) * (mm + 0.5 - m_c);
            for (nn = 2; nn < SizeY - 2; nn++) {
                n2 = (nn + 0.5 - n_c) * (nn + 0.5 - n_c);
                for (pp = 2; pp < SizeZ - 2; pp++) {
                    p2 = (pp + 0.5 - p_c) * (pp + 0.5 - p_c);
                    if (m2 + n2 + p2 < r2) {
                        /* zero surrounding Ex nodes */
                        Cexe(mm, nn,     pp    ) = 0.0;
                        Cexe(mm, nn + 1, pp    ) = 0.0;
                        Cexe(mm, nn,     pp + 1) = 0.0;
                        Cexe(mm, nn + 1, pp + 1) = 0.0;
                        Cexh(mm, nn,     pp    ) = 0.0;
                        Cexh(mm, nn + 1, pp    ) = 0.0;
                        Cexh(mm, nn,     pp + 1) = 0.0;
                        Cexh(mm, nn + 1, pp + 1) = 0.0;
                        /* zero surrounding Ey nodes */
                        Ceye(mm,     nn, pp    ) = 0.0;
                        Ceye(mm + 1, nn, pp    ) = 0.0;
                        Ceye(mm,     nn, pp + 1) = 0.0;
                        Ceye(mm + 1, nn, pp + 1) = 0.0;
                        Ceyh(mm,     nn, pp    ) = 0.0;
                        Ceyh(mm + 1, nn, pp    ) = 0.0;
                        Ceyh(mm,     nn, pp + 1) = 0.0;
                        Ceyh(mm + 1, nn, pp + 1) = 0.0;
                        /* zero surrounding Ez nodes */
                        Ceze(mm,     nn,     pp) = 0.0;
                        Ceze(mm + 1, nn,     pp) = 0.0;
                        Ceze(mm,     nn + 1, pp) = 0.0;
                        Ceze(mm + 1, nn + 1, pp) = 0.0;
                        Cezh(mm,     nn,     pp) = 0.0;
                        Cezh(mm + 1, nn,     pp) = 0.0;
                        Cezh(mm,     nn + 1, pp) = 0.0;
                        Cezh(mm + 1, nn + 1, pp) = 0.0;
                    }
                }
            }
        }
    }

    /* ---------------------------------------------------------------- */
    /* Magnetic-field update coefficients (free space, always)          */
    /* ---------------------------------------------------------------- */
    for (mm = 0; mm < SizeX; mm++)
        for (nn = 0; nn < SizeY - 1; nn++)
            for (pp = 0; pp < SizeZ - 1; pp++) {
                Chxh(mm, nn, pp) = 1.0;
                Chxe(mm, nn, pp) = Cdtds / imp0;
            }

    for (mm = 0; mm < SizeX - 1; mm++)
        for (nn = 0; nn < SizeY; nn++)
            for (pp = 0; pp < SizeZ - 1; pp++) {
                Chyh(mm, nn, pp) = 1.0;
                Chye(mm, nn, pp) = Cdtds / imp0;
            }

    for (mm = 0; mm < SizeX - 1; mm++)
        for (nn = 0; nn < SizeY - 1; nn++)
            for (pp = 0; pp < SizeZ; pp++) {
                Chzh(mm, nn, pp) = 1.0;
                Chze(mm, nn, pp) = Cdtds / imp0;
            }

    return;
} /* end gridInit() */
