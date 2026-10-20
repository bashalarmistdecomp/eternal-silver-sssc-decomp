.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching InitCARD2, 0xC

glabel InitCARD2
    .incbin "expected/code.bin", 0x90628, 0xC
endlabel InitCARD2
    .incbin "expected/code.bin", 0x90634, 0x4
