.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching CD_cw, 0x40C

glabel CD_cw
    .incbin "expected/code.bin", 0x11612C, 0x40C
endlabel CD_cw
