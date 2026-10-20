.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching GetIntrMask, 0x18

glabel GetIntrMask
    .incbin "expected/code.bin", 0x10F434, 0x18
endlabel GetIntrMask
