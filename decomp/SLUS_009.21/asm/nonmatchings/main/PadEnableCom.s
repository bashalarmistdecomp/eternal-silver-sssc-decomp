.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching PadEnableCom, 0x118

glabel PadEnableCom
    .incbin "expected/code.bin", 0x11C8A0, 0x118
endlabel PadEnableCom
