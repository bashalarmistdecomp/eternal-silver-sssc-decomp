.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libgte (no exported name): PSY-Q 4.5, lib/libgte.lib, member MSC00, .text + 0x0.
   These bytes are read from Sony's own file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching func_8008FE08

dlabel func_8008FE08
    .incbin "lib/libgte.lib", 0x4C4C, 0x8
enddlabel func_8008FE08
