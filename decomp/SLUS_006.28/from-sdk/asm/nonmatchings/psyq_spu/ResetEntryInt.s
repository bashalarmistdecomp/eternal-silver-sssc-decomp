.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi ResetEntryInt: PSY-Q 4.5, lib/libapi.lib, member A24, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching ResetEntryInt, 0xC

glabel ResetEntryInt
    .incbin "lib/libapi.lib", 0x16E2, 0xC
endlabel ResetEntryInt
    .space 0x4
