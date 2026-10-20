.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching MoveImage2, 0x144

glabel MoveImage2
    .incbin "expected/code.bin", 0x113F60, 0x144
endlabel MoveImage2
