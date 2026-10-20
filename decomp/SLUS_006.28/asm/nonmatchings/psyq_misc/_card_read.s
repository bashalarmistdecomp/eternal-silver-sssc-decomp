.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _card_read, 0xC

glabel _card_read
    .incbin "expected/code.bin", 0x93CE8, 0xC
endlabel _card_read
    .incbin "expected/code.bin", 0x93CF4, 0x4
