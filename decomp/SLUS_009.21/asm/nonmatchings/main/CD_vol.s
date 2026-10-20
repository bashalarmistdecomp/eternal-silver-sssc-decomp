.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching CD_vol, 0x88

glabel CD_vol
    .incbin "expected/code.bin", 0x116538, 0x88
endlabel CD_vol
