.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching GetVideoMode, 0x10

glabel GetVideoMode
    .incbin "expected/code.bin", 0x114204, 0x10
endlabel GetVideoMode
    .incbin "expected/code.bin", 0x114214, 0xC
