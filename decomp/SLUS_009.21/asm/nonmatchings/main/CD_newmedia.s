.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching CD_newmedia, 0x2C4

glabel CD_newmedia
    .incbin "expected/code.bin", 0x1170E8, 0x2C4
endlabel CD_newmedia
