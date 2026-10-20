.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching DrawOTag2, 0xFC

glabel DrawOTag2
    .incbin "expected/code.bin", 0x1140A4, 0xFC
endlabel DrawOTag2
