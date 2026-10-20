.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _patch_card, 0x94

glabel _patch_card
    .incbin "expected/code.bin", 0x90734, 0x94
endlabel _patch_card
