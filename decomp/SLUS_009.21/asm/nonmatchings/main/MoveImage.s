.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching MoveImage, 0xB8

glabel MoveImage
    .incbin "expected/code.bin", 0x1119A8, 0xB8
endlabel MoveImage
