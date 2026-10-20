/* inflate.h: written from the readings of 2026-10-14 (docs/naming.md,
   "Structs and their fields"). */
#ifndef INFLATE_H
#define INFLATE_H

#include "layout.h"

/* Huft: one entry of an inflate decoding table: gzip 1.2.4 inflate.c's struct
   huft, with its published names (the owner's ruling for published source:
   struct huft is Huft, its fields e, b, v). huft_build makes the tables from
   the pool at D_800E0CB0 (g_HuftPoolNext), inflate_block_dynamic keeps the
   literal/length and distance tables (tl, td), inflate_codes decodes with
   them. */
typedef struct Huft {
    /* +0x00 e: the number of extra bits, or an operation: 16 + j a link to a
       sub-table of j bits (v.t), 16 a literal, 15 the end of the block, 99 an
       invalid code (gzip's own reading). Written by huft_build (16 + j, 99, 16
       or 15, e[*p - s]); inflate_codes reads it (more than 16: follow v.t). */
    u8 e;
    /* +0x01 b: the number of bits in this code or sub-code. Written by
       huft_build (r.b = l, r.b = k - w); inflate_codes drops that many bits of
       its bit buffer (DUMPBITS(t->b)). The compiler pads +0x02..+0x03 after
       it, before v; the published struct declares nothing there and
       huft_build's whole-entry copies (u[h - 1][j] = r, q[j] = r) carry them. */
    u8 b;
    /* +0x04 v: the base value (v.n: a literal, a length base or a distance
       base) or, for e above 16, the address of the next table level (v.t).
       Written by huft_build; read by inflate_codes. */
    union {
        /* n: a literal, a length base or a distance base (gzip's names) */
        u16 n;
        /* t: the next level of the table, for e above 16; huft_build also
           keeps each table's link to the one before it in the table's dummy
           first entry */
        struct Huft * t;
    } v;
} Huft;

LAYOUT_SIZE(Huft, 0x08);
LAYOUT_AT(Huft, e, e, 0x00);
LAYOUT_AT(Huft, b, b, 0x01);
LAYOUT_AT(Huft, v, v, 0x04);
LAYOUT_AT(Huft, v_n, v.n, 0x04);
LAYOUT_AT(Huft, v_t, v.t, 0x04);

#endif
