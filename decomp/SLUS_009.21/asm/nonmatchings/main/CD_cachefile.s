.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching CD_cachefile, 0x29C

glabel CD_cachefile
    .incbin "expected/code.bin", 0x117450, 0x29C
endlabel CD_cachefile
