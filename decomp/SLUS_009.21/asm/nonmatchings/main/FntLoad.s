.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching FntLoad, 0xA0

glabel FntLoad
    .incbin "expected/code.bin", 0x1102C0, 0xA0
endlabel FntLoad
