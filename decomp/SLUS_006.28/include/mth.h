/* mth.h: written from the readings of 2026-10-14 (docs/naming.md,
   "Structs and their fields"). */
#ifndef MTH_H
#define MTH_H

#include "layout.h"

/* A point of the math group's curve (MTH_CurvePointsMake, mth_curv.c): x and y as
   16.16 fixed-point words. BtlShow2CRun hands it 0x20 such points and takes
   0x40 back. */
typedef struct {
    /* +0x00 x, 16.16 fixed point: BtlShow2CRun writes whole screen x << 16 and
       reads the curve's >> 16; MTH_CurvePointsMake copies it to g_MthX and
       writes the output's. */
    s32 x;
    /* +0x04 y, 16.16 fixed point, as x. */
    s32 y;
} MthPoint;

LAYOUT_SIZE(MthPoint, 0x08);
LAYOUT_AT(MthPoint, x, x, 0x00);
LAYOUT_AT(MthPoint, y, y, 0x04);

#endif
