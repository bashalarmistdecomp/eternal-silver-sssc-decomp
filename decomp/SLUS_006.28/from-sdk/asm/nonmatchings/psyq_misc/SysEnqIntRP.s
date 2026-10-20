.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi SysEnqIntRP: PSY-Q 4.5, lib/libapi.lib, member L02, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching SysEnqIntRP, 0xC

glabel SysEnqIntRP
    .incbin "lib/libapi.lib", 0x2A58, 0xC
endlabel SysEnqIntRP
    .space 0x4
