.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _patch_gte, 0xAC

glabel _patch_gte
    .incbin "expected/code.bin", 0x7EF98, 0xAC
endlabel _patch_gte
