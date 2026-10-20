.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching sprintf, 0x880

glabel sprintf
    .incbin "expected/code.bin", 0x1208D0, 0x880
endlabel sprintf
