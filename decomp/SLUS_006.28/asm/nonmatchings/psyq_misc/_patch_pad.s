.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* Handwritten function */
nonmatching _patch_pad, 0x70

glabel _patch_pad
    .incbin "expected/code.bin", 0x90540, 0x70
endlabel _patch_pad
    .incbin "expected/code.bin", 0x905B0, 0x8
