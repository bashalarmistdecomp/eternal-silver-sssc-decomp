.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching PAD_init2, 0xC

glabel PAD_init2
    .incbin "expected/code.bin", 0x904E8, 0xC
endlabel PAD_init2
    .incbin "expected/code.bin", 0x904F4, 0x4
