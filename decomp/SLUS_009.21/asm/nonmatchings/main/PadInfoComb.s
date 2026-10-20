.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching PadInfoComb, 0xA8

glabel PadInfoComb
    .incbin "expected/code.bin", 0x11C724, 0xA8
endlabel PadInfoComb
