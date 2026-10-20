.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching toupper, 0x34

glabel toupper
    .incbin "expected/code.bin", 0x1205A0, 0x34
endlabel toupper
