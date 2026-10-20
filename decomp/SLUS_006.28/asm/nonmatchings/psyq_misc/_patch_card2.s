.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _patch_card2, 0x70

glabel _patch_card2
    .incbin "expected/code.bin", 0x907C8, 0x70
endlabel _patch_card2
