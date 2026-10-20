.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* Handwritten function */
nonmatching _patch_card_info, 0x44

glabel _patch_card_info
    .incbin "expected/code.bin", 0x90658, 0x44
endlabel _patch_card_info
