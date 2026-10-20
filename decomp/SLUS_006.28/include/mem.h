/* mem.h: written from the readings of 2026-10-14 (docs/naming.md,
   "Structs and their fields"). */
#ifndef MEM_H
#define MEM_H

#include "layout.h"

/* The five-word request block g_MemRequest (0x800E4750): a copy or a fill that
   mem_request_run (mem_request.c) does and marks done. Its one producer,
   func_80049AC4 (btl_window.c, called first by BtlMenuOpen), fills it with kind 2;
   mem_request_run's one caller is reached by nothing. */
typedef struct {
    /* +0x00 0 none; 1 copy rows of words; any other fill 0x400 words.
       func_80049AC4 writes 2 last; mem_request_run sets 0 when done. */
    u8 kind;
    /* +0x01..+0x03 nothing reads or writes them. */
    u8 pad_01[3];
    /* +0x04 a union by kind (pass_menu.json's): the source address of a copy,
       or the word of a fill. func_80049AC4 writes 0x102C00 with kind 2, a fill
       word. */
    union {
        /* source: used by a, c, copy, kind, mem_request_run, misc_d */
        s32 source;
        /* fill: used by a, any, c, fill, func_80049AC4, kind, mem_request_run,
           misc, misc_d, other, with */
        s32 fill;
    } source_fill;
    /* +0x08 the address written to. func_80049AC4 writes 0x25E74000, no
       address of the console's memory. */
    s32 destination;
    /* +0x0C for a copy, the words of each row (rows are 0x20 words apart).
       func_80049AC4 writes 0x20. */
    s32 row_words;
    /* +0x10 for a copy, the number of rows. func_80049AC4 writes 0x20. */
    s32 rows;
} MemRequest;

LAYOUT_SIZE(MemRequest, 0x14);
LAYOUT_AT(MemRequest, kind, kind, 0x00);
LAYOUT_AT(MemRequest, pad_01, pad_01, 0x01);
LAYOUT_AT(MemRequest, source_fill, source_fill, 0x04);
LAYOUT_AT(MemRequest, source_fill_source, source_fill.source, 0x04);
LAYOUT_AT(MemRequest, source_fill_fill, source_fill.fill, 0x04);
LAYOUT_AT(MemRequest, destination, destination, 0x08);
LAYOUT_AT(MemRequest, row_words, row_words, 0x0c);
LAYOUT_AT(MemRequest, rows, rows, 0x10);

#endif
