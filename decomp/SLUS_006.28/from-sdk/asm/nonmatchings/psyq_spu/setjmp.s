.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libc2 setjmp: PSY-Q 4.5, lib/libc2.lib, member SETJMP, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching setjmp, 0x3C

glabel setjmp
    .incbin "lib/libc2.lib", 0x575F, 0x3C
endlabel setjmp
