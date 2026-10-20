.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching PadInfoMode, 0xF8

glabel PadInfoMode
    .incbin "expected/code.bin", 0x11C558, 0xF8
endlabel PadInfoMode
