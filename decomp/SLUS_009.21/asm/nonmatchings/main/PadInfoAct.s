.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching PadInfoAct, 0xD4

glabel PadInfoAct
    .incbin "expected/code.bin", 0x11C650, 0xD4
endlabel PadInfoAct
