.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _card_info, 0xC

glabel _card_info
    .incbin "expected/code.bin", 0x92998, 0xC
endlabel _card_info
    .incbin "expected/code.bin", 0x929A4, 0x4
