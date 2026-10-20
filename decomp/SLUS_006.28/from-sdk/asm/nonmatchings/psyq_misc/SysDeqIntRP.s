.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi SysDeqIntRP: PSY-Q 4.5, lib/libapi.lib, member L03, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching SysDeqIntRP, 0xC

glabel SysDeqIntRP
    .incbin "lib/libapi.lib", 0x2AF7, 0xC
endlabel SysDeqIntRP
    .space 0x4
