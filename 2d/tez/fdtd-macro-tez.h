#ifndef _FDTD_MACRO_TEZ_H
#define _FDTD_MACRO_TEZ_H

#include "fdtd-grid1.h"

/* macros that permit the "Grid" to be specified */
/* one-dimensional grid */
#define Hz1G(G, M)      G->hz[M]
#define Chzh1G(G, M)    G->chzh[M]
#define Chze1G(G, M)    G->chze[M]

#define Ey1G(G, M)      G->ey[M]
#define Ceye1G(G, M)    G->ceye[M]
#define Ceyh1G(G, M)    G->ceyh[M]

/* TEz grid */
#define HzG(G, M, N)      G->hz[(M) * (SizeYG(G) - 1) + (N)]
#define ChzhG(G, M, N)    G->chzh[(M) * (SizeYG(G) - 1) + (N)]
#define ChzeG(G, M, N)    G->chze[(M) * (SizeYG(G) - 1) + (N)]

#define ExG(G, M, N)      G->ex[(M) * SizeYG(G) + (N)]
#define CexeG(G, M, N)    G->cexe[(M) * SizeYG(G) + (N)]
#define CexhG(G, M, N)    G->cexh[(M) * SizeYG(G) + (N)]

#define EyG(G, M, N)      G->ey[(M) * (SizeYG(G) - 1) + (N)]
#define CeyeG(G, M, N)    G->ceye[(M) * (SizeYG(G) - 1) + (N)]
#define CeyhG(G, M, N)    G->ceyh[(M) * (SizeYG(G) - 1) + (N)]

#define SizeXG(G)   G->sizeX
#define SizeYG(G)   G->sizeY
#define SizeZG(G)   G->sizeZ
#define TimeG(G)    G->time
#define MaxTimeG(G) G->maxTime
#define CdtdsG(G)   G->cdtds
#define TypeG(G)    G->type

/* macros that assume the "Grid" is "g" */
/* one-dimensional grid */
#define Hz1(M)      Hz1G(g, M)
#define Chzh1(M)    Chzh1G(g, M)
#define Chze1(M)    Chze1G(g, M)

#define Ey1(M)      Ey1G(g, M)
#define Ceye1(M)    Ceye1G(g, M)
#define Ceyh1(M)    Ceyh1G(g, M)

/* TEz grid */
#define Hz(M, N)    HzG(g, M, N)
#define Chzh(M, N)  ChzhG(g, M, N)
#define Chze(M, N)  ChzeG(g, M, N)

#define Ex(M, N)    ExG(g, M, N)
#define Cexh(M, N)  CexhG(g, M, N)
#define Cexe(M, N)  CexeG(g, M, N)

#define Ey(M, N)    EyG(g, M, N)
#define Ceye(M, N)  CeyeG(g, M, N)
#define Ceyh(M, N)  CeyhG(g, M, N)

#define SizeX   SizeXG(g)
#define SizeY   SizeYG(g)
#define SizeZ   SizeZG(g)
#define Time    TimeG(g)
#define MaxTime MaxTimeG(g)
#define Cdtds   CdtdsG(g)
#define Type    TypeG(g)

#endif /* matches #ifndef _FDTD_MACRO_TEZ_H */
