.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching StGetNext, 0xB8

glabel StGetNext
    .incbin "expected/code.bin", 0x118530, 0xB8
endlabel StGetNext
    .incbin "expected/code.bin", 0x1185E8, 0x8
