.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching StartCARD2, 0xC

glabel StartCARD2
    .incbin "expected/code.bin", 0x90638, 0xC
endlabel StartCARD2
    .incbin "expected/code.bin", 0x90644, 0x4
