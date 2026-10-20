.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching InitPAD2, 0xC

glabel InitPAD2
    .incbin "expected/code.bin", 0x904B8, 0xC
endlabel InitPAD2
    .incbin "expected/code.bin", 0x904C4, 0x4
