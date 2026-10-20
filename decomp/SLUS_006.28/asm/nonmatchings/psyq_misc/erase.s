.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching erase, 0xC

glabel erase
    .incbin "expected/code.bin", 0x92628, 0xC
endlabel erase
    .incbin "expected/code.bin", 0x92634, 0x4
