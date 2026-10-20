.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching ClearOTagR, 0xAC

glabel ClearOTagR
    .incbin "expected/code.bin", 0x111B28, 0xAC
endlabel ClearOTagR
