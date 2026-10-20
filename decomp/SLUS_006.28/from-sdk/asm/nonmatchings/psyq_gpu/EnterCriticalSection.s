.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* Handwritten function */
/* libapi EnterCriticalSection: PSY-Q 4.5, lib/libapi.lib, member A36, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching EnterCriticalSection, 0x10

glabel EnterCriticalSection
    .incbin "lib/libapi.lib", 0x18D9, 0x10
endlabel EnterCriticalSection
