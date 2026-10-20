.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching startIntrVSync, 0x58

glabel startIntrVSync
    .incbin "expected/code.bin", 0x10F990, 0x58
endlabel startIntrVSync
