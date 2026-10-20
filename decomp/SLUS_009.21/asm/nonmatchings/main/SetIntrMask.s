.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching SetIntrMask, 0x18

glabel SetIntrMask
    .incbin "expected/code.bin", 0x10F44C, 0x18
endlabel SetIntrMask
