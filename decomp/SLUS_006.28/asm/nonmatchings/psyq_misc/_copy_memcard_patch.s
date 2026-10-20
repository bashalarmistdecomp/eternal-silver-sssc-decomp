.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _copy_memcard_patch, 0x34

glabel _copy_memcard_patch
    .incbin "expected/code.bin", 0x90838, 0x34
endlabel _copy_memcard_patch
    .incbin "expected/code.bin", 0x9086C, 0xC
