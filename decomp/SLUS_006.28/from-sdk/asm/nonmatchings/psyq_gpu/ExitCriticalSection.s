.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* Handwritten function */
/* libapi ExitCriticalSection: PSY-Q 4.5, lib/libapi.lib, member A37, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching ExitCriticalSection, 0x10

glabel ExitCriticalSection
    .incbin "lib/libapi.lib", 0x1988, 0x10
endlabel ExitCriticalSection
