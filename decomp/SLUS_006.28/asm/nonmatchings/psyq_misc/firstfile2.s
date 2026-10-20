.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching firstfile2, 0xC

glabel firstfile2
    .incbin "expected/code.bin", 0x928D8, 0xC
endlabel firstfile2
    .incbin "expected/code.bin", 0x928E4, 0x4
