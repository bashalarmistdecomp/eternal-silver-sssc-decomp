.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching Krom2Tim, 0x500

glabel Krom2Tim
    .incbin "expected/code.bin", 0x110DAC, 0x500
endlabel Krom2Tim
    .incbin "expected/code.bin", 0x1112AC, 0x4
