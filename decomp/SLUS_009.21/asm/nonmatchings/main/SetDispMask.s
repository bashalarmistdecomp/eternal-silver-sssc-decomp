.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching SetDispMask, 0x98

glabel SetDispMask
    .incbin "expected/code.bin", 0x1115A4, 0x98
endlabel SetDispMask
