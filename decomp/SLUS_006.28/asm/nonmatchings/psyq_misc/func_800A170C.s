.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_800A170C, 0x24

glabel func_800A170C
    .incbin "expected/code.bin", 0x9070C, 0x24
endlabel func_800A170C
    .incbin "expected/code.bin", 0x90730, 0x4
