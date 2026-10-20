.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching PadSetMainMode, 0x48

glabel PadSetMainMode
    .incbin "expected/code.bin", 0x11C804, 0x48
endlabel PadSetMainMode
