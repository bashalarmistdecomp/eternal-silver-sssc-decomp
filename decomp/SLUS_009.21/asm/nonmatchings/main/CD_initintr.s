.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching CD_initintr, 0x4C

glabel CD_initintr
    .incbin "expected/code.bin", 0x116784, 0x4C
endlabel CD_initintr
