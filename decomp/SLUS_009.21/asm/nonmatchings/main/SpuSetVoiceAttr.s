.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching SpuSetVoiceAttr, 0x5F8

glabel SpuSetVoiceAttr
    .incbin "expected/code.bin", 0x11BB80, 0x5F8
endlabel SpuSetVoiceAttr
    .incbin "expected/code.bin", 0x11C178, 0x8
